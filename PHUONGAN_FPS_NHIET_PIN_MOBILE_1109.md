# PHƯƠNG ÁN KHẮC PHỤC TỤT FPS · NÓNG MÁY · CPU CAO · HẾT PIN — bản mobile (Fold 7) — 11/09 12:20

> Đọc `BANGIAO_DONHIP_MOBILE_1209.md` + toàn bộ log `D:\jx1_android_log` (41 phiên Fold 7; thêm phiên **11:48–12:10 bản f 109111058, màn trong,
> chưa ai đọc**). Điện thoại không nối adb (chỉ máy ảo) → log "kéo về" là qua LAN, máy chủ 8765 đang chạy (PID 348172). Bộ đọc: scratchpad
> `phan_tich_tong.py`, `bang_ve.py` (bản 11:18) + hai bộ đếm mới cho `[VE-GIAT]`. **Chỉ phương án — chưa sửa mã, chưa commit.**
> Luật giữ nguyên: mọi thay đổi chỉ `JX_ANDROID` / tệp chỉ-Android, chuỗi Windows không đổi; mỗi mục có công tắc để đối chứng.

## 0. Kết luận trong 8 dòng

1. **Nóng / CPU / pin hiện không do logic game, không còn do SDL.** Cảnh yên: luồng chính bận 3,7–5,2 ms mỗi khung, trong đó **55–60 % là
   ghi lệnh + nộp cho lớp SDL-GPU** (không phải vẽ của game, không phải tick). Ở 120 Hz mọi chi phí ×2 so với 60 Hz: 2,1–2,8 W / CPU 48–68 %
   so với 1,6–2,0 W / 31–52 %; pin 13,6 %/giờ so với 9,7 %/giờ.
2. **Đông người (Tống Kim):** 2 500–2 900 lệnh vẽ/khung, 1 700–2 000 lần đổi texture/khung → ghi lệnh 3,4–3,5 ms + nộp 1,0–2,3 ms + vẽ CPU
   2,7–3,0 ms = **7,3–9,0 ms > 8,3 ms** (ngân sách 120 Hz) → 97–114 fps, 5–6 W, SEVERE sau ~17 phút. Đây là gốc "đông là tụt fps + nóng" còn lại.
3. **GPU tô ở độ phân giải gốc** (màn trong 2184×1968 = 4,3 Mpx) dù game chỉ vẽ 1040×936: lúc đông nghẽn GPU (nộp + chờ swapchain 25–33 ms).
   Log **chưa có GPU %** → phải đo trước rồi mới sửa.
4. **Giật lặt vặt còn lại** (12 lần/phút, 17–100 ms): **260/260 khung "chép chậm" ở phiên 11:48 là khung có tải hàng bảng màu** vào texture
   256×8192, không phụ thuộc Hz hay số KB → chi phí cố định trong driver Adreno cho copy từng hàng vào ảnh 8 MB. Sửa được ở phía ta.
5. Bản f (109111058) **không còn lỗi hiển thị** trong log (tao_hong 0, khung_khong_tex 0 suốt 22 phút) — cần chủ xác nhận bằng mắt.
6. Thứ tự làm: **A** chính sách nhịp (1–2 ngày, −25–30 % điện ngay) → **B** bảng màu (1 ngày) → **D0** đo GPU (½ ngày) → **C** atlas mảng +
   bớt đổi trạng thái (3–5 ngày) → **D1** swapchain theo khung logic (1–2 ngày) → **E** nạp khi đám đông tới (2–3 ngày) → **F** dữ liệu.
7. Không có mục nào làm giảm hình ảnh hay độ trễ; A1 chỉ đổi **mặc định** "Tự động" từ 120 xuống 60 — người chơi vẫn chọn được 90/120.
8. Kỳ vọng sau A+B+C+D: yên 120 Hz ~1,8 W (nay 2,4), đông 120 Hz ~3–3,5 W (nay 5–6) giữ 117–120 fps; ở "Tự động" 60: yên ~1,5 W, đông ~2,2 W.

## 1. Số liệu

### 1.1 Theo cảnh (Fold 7 SM8750, không sạc; W = trung bình `[MAU]`, CPU = % của MỘT nhân: tiến trình / luồng chính)

| Cảnh | Phiên | Hz / fps | W | CPU | Nhiệt | Pin |
|---|---|---|---|---|---|---|
| Yên, màn trong, **60** (bản f) | 11:11, 37 ph | 60 / 59 | **1,6–2,0** | 31–52 / 24–42 | 0 suốt | 76 → 70 % (**9,7 %/h ≈ 10 h**) |
| Yên, màn trong, **120** (bản f) | 11:48, 22 ph | 120 / 117–118 | **2,1–2,8** | 48–68 / 40–57 | 0 suốt, pin 32 → 34 °C | 70 → 65 % (**13,6 %/h ≈ 7 h**) |
| Đông (Tống Kim), màn ngoài, 120 | 02:14, 20 ph | 120 / 111–118 | 2,6–5,1 | 68–74 / 63–69 | 1 → 2 → **3 SEVERE** sau ~17 ph, pin 38,6 °C | — |
| Đông, màn trong, 120 | 10:22, 23 ph | 120 / 69–117 | 2,3–**6,0** | 62–77 / 55–70 | 0 → 2 sau 6–20 ph | — |
| Đăng nhập + tải map | 11:48 t < 50 s | 120 / ~90 | 2,3–4,7 | — | — | — |

Samsung tự hạ màn 120 → 60 Hz khi game chậm (`[DOI]` lúc tải map) rồi nâng lại — hệ thống đã làm việc A2/A3 một cách bị động.

### 1.2 Chi phí luồng chính mỗi khung (ms; `[VE]` = SubmitFrame tách bước, veCPU = RepresentBegin→End, `[SUM]`/`[WORLD b]` cho tick)

| Cảnh (phiên, t) | NPC/tick · đạn/tick | quad · lệnh · đổi tex / khung | veCPU | chép | **ghi** | **nộp** | chờ (ngủ) | tổng bận | fps · W · CPU |
|---|---|---|---|---|---|---|---|---|---|
| Yên 120 màn trong (11:48 t=901) | 6 · 8 | 388 · 203 · 113 | 1,6 | 0,2 | 0,85 | 0,93 | 3,2 | **3,6** (+tick 0,2×18/s) | 118 · 2,3 · 50/42 |
| Yên có đánh (11:48 t=331) | 14 · 37 | 647 · 412 · 263 | 2,1 | 0,35 | 1,37 | 1,35 | 1,6 | **5,2** | 117 · 2,6 · 65/54 |
| Đông màn ngoài (09:31 t=2667, b+c) | 128 · 837 | 2 851 · 2 909 · 1 996 | 2,7 | 0,2 | **3,4** | 1,0 | 0,4 | **7,3** + tick 1,2 | 114 · 5,2 · 86/78 |
| Đông màn trong (10:22 t=1411, b+c) | 124 · 638 | 2 505 · 2 524 · 1 729 | 3,0 | 0,2 | **3,5** | **2,3** | 0,4 | **9,0** | **97** · 6,0 · 77/70 |
| Đông màn trong sau atlas kệ (10:47 t=181, d+e) | 68 · 342 | 1 401 · 1 313 · 728 | 3,0 | 0,5 | 2,2 | 1,4 | 1,7 | **7,1** | 100 (min 64) · 4,8 · 68/60 |

Đọc số:
- Tổng bận × fps khớp CPU luồng chính đo được (3,6 ms × 118 = 42 %) → **không có vòng lặp bận/quay chờ**; "chờ" là `vkWaitForFences` ngủ thật.
  Lưới vòng bơm 1 ms (`Sleep(1)`, ~340 lần/giây) không đáng kể.
- **ghi + nộp = 1,8 ms yên → 4,5–5,8 ms đông** = phần lớn nhất và tăng nhanh nhất theo độ đông. Hồi quy cả log: ghi ≈ 1,0 ms + 1,0 ms/1 000 lệnh.
  Mỗi lệnh = 1 `SDL_BindGPUVertexBuffers` + 1 `SDL_DrawGPUPrimitives`; mỗi đổi texture = 1 `SDL_BindGPUFragmentSamplers` (SDL Vulkan: cấp + ghi
  descriptor set); mỗi đổi ps = 1 `SDL_PushGPUFragmentUniformData`.
- `[VE-GOP]` phiên 11:48 (cả 22 phút): quad không gộp vì **texture0 73 %**, pipeline 21 %, ps 4,5 %; đổi/khung TB pipeline 55, texture 131
  (max 548), **ps 150**. Phiên đông 09:31: texture0 86 %. Atlas kệ 2048² (bản e) chỉ hạ đổi texture từ ~0,7 xuống ~0,5 lần/quad vì thứ tự vẽ
  theo Y xen kẽ các NPC nằm ở trang khác nhau (35 trang × 16 MB).
- Vẽ CPU của game (veCPU) 1,6–3,0 ms, trong đó `DrawPrimitives` (lớp mô phỏng D3D9: ép UV atlas, băm trạng thái, chép ring) 1,0–1,7 ms.
- Tick logic: 0,2–1,8 ms/tick × 18 tick/s ≤ 3 % nhân — **đạn đã sửa xong** (677 viên/tick = 1,6 ms), không còn là gốc.
- UI chiếm phần lớn cảnh yên: `ve/khung` 11:48 t=901 = npc 38, skill 15, **ui 304**, khác 129 (tổng 502 đơn vị) → ~60 % lệnh vẽ ở cảnh yên là
  giao diện tĩnh vẽ lại 118 lần/giây.

### 1.3 Bảng màu = gốc "chép chậm" còn lại (đã tách bằng đồng hồ con của bản d)

- Phiên 11:48: 284 dòng `[VE-GIAT]` (khung > 20 ms), **258 do "chép"**, 20 do vẽ khác, 5 do nạp, 1 do chờ. Trong 260 khung chép ≥ 10 ms:
  **260/260 có tải hàng bảng màu** (226 kèm texture atlas, 32 chỉ bảng màu, 2 kèm vùng 0); **không có khung nào chép chậm mà không có bảng màu**.
  Đồng hồ con: `lenh tai` TB 23,1 ms (= vòng `SDL_UploadToGPUTexture`), `map+chep` 0,0–0,1, `bang mau` 0,0, `zero` 0, `ring` 0,1, xfer 32 MB không phình.
- Phân bố: 15–25 ms chiếm 74 % ở 120 Hz (11:48) và 61 % ở 60 Hz (11:11, 502 khung) → **không phải chờ khung bay** (nếu chờ thì phải ×2 ở 60 Hz).
  Không theo số KB (0–600 KB), không theo số hàng (1–54). Đã đọc `VULKAN_UploadToTexture` của SDL 3.2.30: chỉ barrier + `vkCmdCopyBufferToImage`,
  không có khoá/chờ → thời gian nằm trong driver Adreno khi copy **một hàng 256×1** vào ảnh 256×8192 BGRA8 (8 MB, tiling UBWC): khả năng cao
  driver đổi layout / giải-nén lại cả ảnh mỗi lần.
- Tần suất: 2 553 hàng bảng màu/22 phút (TB 57/30 s, đỉnh 469/30 s khi đám đông tới) → 12 khung giật/phút ở cảnh yên, hàng chục/phút lúc đông.
  Không gây nóng, nhưng là **cảm giác "khựng" lặt vặt** hiện nay.

### 1.4 Còn lại, nhỏ hơn

- 30–60 s đầu vào map / đám đông mới tới: `[REP3-NAP]` tệp spr đồng bộ 26–233 lần/30 s (max 38 ms), "ngoài lúc vẽ" (`GetImageParam`)
  344–511 lần/154–370 ms mỗi 30 s (11:48 t=661: 511 lần/154 ms; `[VE-NAP]` bỏ vẽ 1 120 lượt/30 s) → fps 102–112 trong 30 s. Đã giảm nhiều so
  với y2, chưa hết.
- Đăng nhập / chọn nhân vật vẫn vẽ ~90 fps; lúc tải map 2,3–4,7 W.
- Bộ tải đang bật `AutoLog=1 PaintLog=1 GuiLog=1`: ghi tệp `[VE-GIAT]` tới 12 dòng/10 s, `[PDET]` mỗi giật, Java POST mỗi 10 s — vài % CPU.
- Đổi map: tick 150–300 ms (đã biết, không liên quan nhịp).
- Dữ liệu: 3 cây (`雪松\1,2`, `菩提树3`) không vẽ do chỉ mục pak 16 bit; 40–141 sprite thiếu trong pak (đầu ngựa 015/002, quái, MiniMap, nút chat).

## 2. Gốc rễ

| # | Gốc | Bằng chứng | Ảnh hưởng |
|---|---|---|---|
| G1 | **"Tự động" = tần số màn = 120 Hz** (`JxNhip_DatMuc(0)` → `Nhip_ManHz()`), mọi chi phí CPU/GPU ×2, màn 120 Hz tốn thêm | 1.1: 60 Hz −0,6 W, −17 điểm % CPU, +30 % giờ pin ở cùng cảnh yên | nóng, pin — lớn nhất |
| G2 | **Lớp SDL-GPU ghi 1 300–2 900 lệnh/khung** vì đổi texture 700–2 000/khung (trang atlas), đổi ps 150–850/khung, bind VB mỗi lệnh | 1.2, `[VE-GOP]` | tụt fps đông (9 ms > 8,3), CPU 70–78 % luồng chính, 5–6 W |
| G3 | **GPU tô 4,3 Mpx** (màn trong) thay vì 1,0 Mpx khung logic; mỗi sprite phóng 2,1× trong pass vẽ | `[GPU] cua so 2184x1968 (backbuffer 1040x936)`, Letterbox scale; 10:22 nộp+chờ 25–33 ms | nghẽn GPU lúc đông màn trong, điện GPU (chưa đo) |
| G4 | **Copy hàng bảng màu vào texture 256×8192** | 1.3 | giật 17–100 ms, 12+/phút |
| G5 | Ngoài thế giới vẫn 90–120 fps; log bật; nạp spr đồng bộ khi đám đông tới; dữ liệu pak | 1.4 | pin lúc chờ, giật đầu map, ảnh thiếu |

Đã loại trừ (không phải gốc): tick logic (≤ 3 % nhân, đạn đã sửa), luồng nạp nền (≤ 2 % nhân), chép khung M2 (đã tắt), dựng lại swapchain M1
(đã sửa bằng SDL 3.2.30), nhịp PC (đã bật, không thua), RAM 560–690 MB (ổn), mạng.

## 3. Phương án — theo thứ tự làm

Mỗi mục: việc · chỗ sửa · kỳ vọng · cách đo · công tắc · rủi ro. Tất cả chỉ Android.

### Đợt A — Chính sách nhịp (1–2 ngày; lợi lớn nhất, rủi ro thấp nhất)

**A1. "Tự động" = 60 trên điện thoại, không phải tần số màn.** `JxPerfHudAndroid.cpp::JxNhip_DatMuc`: mức 0 → `PaintFps=60` + `DnXinHz(60)`
(màn hạ 60 Hz thật, đã đo `che_do id=3`). Nấc 90/120 giữ nguyên cho người thích mượt ("Hiệu năng"); nhãn "Tự động (60, tiết kiệm pin)".
Khoá `[Client] FpsTuDong=60` (0 = như cũ theo màn). Kỳ vọng: **cảnh yên 2,4 → 1,7 W, CPU 55 → 35 %, pin 7 → 10 giờ**; cảnh đông ước −1,5–2 W
và không còn chạm SEVERE (11:11: 37 phút 60 Hz nhiệt 0). Cảm giác: 59 khung/s đều (p95 20 ms, 57–61 khung trễ/phút — §9 mục 4 bàn giao cũ);
logic 18 tick/s + nội suy nên 60 vẫn trơn. Đo: 15 phút yên + 15 phút Tống Kim, so 1.1.

**A2. Tự hạ nấc theo nhiệt.** Java đã có `nhiet` (`getCurrentThermalStatus`) và `headroom` (`getThermalHeadroom`) mỗi 5 s trong `JxDoNhip.ghiMau`
→ thêm 1 hàm JNI đưa vào C++ (`JxNhip_NhietTu(status, headroom)`); trong C++ bậc thang 120 → 90 → 60 → 45: hạ một nấc khi `status ≥ 2`
(MODERATE) hoặc `headroom ≥ 0,90` (1,0 = máy bắt đầu giảm xung), giữ ≥ 60 s; nâng lại một nấc khi `status ≤ 1` và `headroom < 0,80` liên tục
120 s, không vượt nấc người chơi chọn. Hiện dòng nhỏ "Đang hạ FPS vì máy nóng" ở góc thông tin. Ghi `[NHIP-NHIET]` vào `jx_nhip.log`.
Khoá `[Client] FpsTheoNhiet=1`. Số đo: 02:14 lên SEVERE sau ~17 phút ở 120 Hz; headroom 0,86–0,99 khi nhiệt 2. Rủi ro: dao động — chống bằng
thời gian giữ 60/120 s và chỉ xét khi đã vào thế giới.

**A3. Tự hạ nấc theo tải.** Dùng bộ đếm sẵn có `[VE]` (tổng việc không kể chờ + veCPU): nếu 3 s liền trung bình > 0,9 × chu kỳ (7,5 ms ở 120 Hz)
→ hạ nấc (120 → 90 → 60): tránh trạng thái "lỡ vblank thành 60 fps mà vẫn trả điện 120 Hz + màn 120 Hz". Nâng lại khi < 0,6 × chu kỳ trong
30 s. Cùng bậc thang và khoá với A2 (`FpsTheoTai=1`). Kỳ vọng: cảnh 10:22 (9,0 ms) chạy 90 Hz đều thay vì 97 fps rung; điện −0,5–1 W.

**A4. Ngoài thế giới vẽ 30 fps + xin 60 Hz** (đăng nhập, chọn nhân vật, màn tải map); vào thế giới mới áp nấc người chơi. Chỗ: nơi `LoadSetting`
áp mức (UiInit / UiShell:447) + sự kiện `GAME_START`/đổi map. Kỳ vọng −0,5–1 W lúc chờ; không ảnh hưởng chơi.

**A5. Bản phát hành tắt log:** lớp ghi đè `config.ini` `AutoLog=0 PaintLog=0 [DoNhip] GuiLog=0` (giữ `ThongTinGoc` theo người chơi). Khi cần đo
mới bật trên dt_v4. Kỳ vọng vài % CPU + bớt ghi flash.

### Đợt B — Bảng màu ra khỏi texture (½–1 ngày; hết giật lặt vặt)

**B0. Kiểm 30 phút trước khi sửa:** đồng hồ con quanh **từng** `SDL_UploadToGPUTexture` trong `SubmitFrame`, tách "vào `m_pPalTex`" và "vào atlas/khác",
in trong `[VE-GIAT]` → xác nhận 100 % thời gian ở lệnh bảng màu (hiện mới suy ra từ 260/260 khung có bảng màu). Thử nhanh cùng lúc: giữ bản CPU
8 MB của bảng màu, tải theo **khối 256×16 hàng** thẳng tile (thay vì 256×1) — nếu hết chậm thì gốc là UBWC nửa tile, xong luôn với 20 dòng.

**B1. Bảng màu = storage buffer 8 MB** thay cho texture 256×8192: `SDL_CreateGPUBuffer(GRAPHICS_STORAGE_READ, 8192×1024)`; hàng mới tải bằng
`SDL_UploadToGPUBuffer` trong cùng copy pass từ `m_texStage` (đường ring 1–2 MB/khung đã đo 0,0–0,5 ms → không có chi phí ảnh);
`SDL_BindGPUFragmentStorageBuffers(pass, 0, &buf, 1)` một lần mỗi pass; shader `Rep3ShadersGPU.frag`: `layout(std430, set = 2, binding = 3)
readonly buffer PalBuf { uint pal[]; }` và `PalTex` đọc `pal[row*256 + idx]` giải BGRA; `SDL_GPUShaderCreateInfo.num_storage_buffers = 1`;
dịch lại bằng `ReverseTools/mobile_x64/dich_shader_gpu.py` (glslc NDK, sinh `Rep3ShadersGPU_spv.h`). Tấm `tb[2]` bỏ → mỗi lệnh bớt 1 sampler.
Khoá `[Client] Rep3PalBuffer=1`. Kỳ vọng: khung chép ≥ 10 ms **12/phút → ≈ 0**, `[VE-GIAT]` "chép" biến mất, `nộp` bớt (ít descriptor).
Rủi ro: driver Adreno với SSBO 8 MB — thấp (ring buffer đang chạy cùng cơ chế). Phương án phụ nếu B0 cho thấy khối 16 hàng đủ: giữ texture,
đổi `PalAlloc` cấp theo khối 16 hàng + tải cả khối.

### Đợt C — Bớt lệnh và đổi trạng thái ở lớp SDL-GPU (3–5 ngày; gốc "đông là tụt fps + nóng")

**C1. Atlas thành texture mảng 2D** (`SDL_GPU_TEXTURETYPE_2D_ARRAY`, 2048² × 8 lớp mỗi "cụm" 128 MB; cụm mới khi đầy — tối đa 3 cụm trong ngân
sách 393 MB). Chỉ số lớp đi theo đỉnh cùng ô `PALROW` (+4 byte đã có: 13 bit hàng bảng màu + 6 bit lớp + 5 bit tổ hợp ps của C2); shader
`sampler2DArray g_t0`, `texture(g_t0, vec3(uv, lớp))` / `texelFetch(..., ivec3)`; tải `dst.layer = trang`; `CAtlasMgrGpu::NewPage/Alloc` cấp
(cụm, lớp, x, y) thay vì texture riêng; texture > 512 (map, UI lớn, RT) vẫn riêng nhưng tạo dạng mảng 1 lớp để dùng chung shader.
Kỳ vọng: đổi texture0 **1 700–2 000/khung → ≤ 10** (chỉ khi đổi cụm/texture riêng), lệnh/khung 2 500 → 300–500 (còn chặn bởi pipeline/ps),
**ghi 3,5 → ~1,0–1,3 ms**, nộp 2,3 → ~1,5 ms. Chỗ: `D3D9onGPURes.cpp` (CAtlasMgrGpu), `D3D9onGPUDev.cpp` (RgAtlasUv, DrawInternal, SubmitFrame
bind), `.vert/.frag`. Khoá `[Client] Rep3AtlasMang=1`. Rủi ro: cấp 128 MB một lần lúc vào map (đo bằng `[VE-GIAT]` tạo GPU), bộ nhớ như cũ.

**C2. Uniform ps thành chỉ số theo đỉnh.** `PSCB` (5 × 16 byte: colorOp/alphaOp/alphatest) thực tế chỉ có vài chục tổ hợp → bảng ≤ 32 tổ hợp
đẩy **một lần mỗi khung**, quad mang chỉ số 5 bit trong ô `PALROW`. Kỳ vọng: đổi ps **150–850/khung → 1**, quad không còn tách vì ps (4,5–2,3 %),
bớt `SDL_PushGPUFragmentUniformData`.

**C3. Vẽ bằng `first_vertex`** (`SDL_DrawGPUPrimitives(pass, n, 1, ringOff/stride, 0)`), bind ring **một lần mỗi pass** (stride đồng nhất — `[VE-GOP]`
"stride 0" chưa từng tách vì stride). Bớt 1 `vkCmdBindVertexBuffers` mỗi lệnh (~1 µs × 2 500 = 2–3 ms/khung lúc đông trước C1; sau C1 nhỏ hơn).

**C4 (tuỳ chọn, chỉ khi C1–C3 chưa đủ):** trong `KScenePlaceC` sắp lại các quad **không chồng nhau** trong cùng dải Y theo texture/pipeline
(quét bao hình chữ nhật một lượt) → gộp thêm khi còn đổi pipeline (55–197/khung = xen kẽ chế độ blend).

Kỳ vọng tổng đợt C ở cảnh 10:22 (đông màn trong): **9,0 → ~6 ms/khung → giữ 117–120 fps thay vì 97; CPU luồng chính 70–78 % → 50–55 %; điện −0,5–0,8 W.**
Bước sau (nếu cần thêm dư địa): `DrawPrimitives` 1,3–1,7 ms — nướng sẵn UV atlas + trạng thái vào bộ nhớ đệm đỉnh của từng khung sprite; cache
giao diện tĩnh (304 đơn vị UI/khung ở cảnh yên) vẽ vào texture khi thay đổi → thêm −1 ms/khung cảnh yên.

### Đợt D — GPU: đo rồi cho swapchain bằng khung logic (½ + 1–2 ngày)

**D0. Đo trước (½ ngày):** `JxDoNhip.ghiMau` ghi thêm vào `[MAU]`: GPU bận `/sys/class/kgsl/kgsl-3d0/gpu_busy_percentage`, xung GPU
`/sys/class/kgsl/kgsl-3d0/gpuclk` (hoặc `devfreq/*/cur_freq`), xung CPU từng cụm `/sys/devices/system/cpu/cpu{0,2,6}/cpufreq/scaling_cur_freq`
(HUD C++ đã có danh sách đường sysfs — kiểm Fold 7 đọc được hay trả `-`). Nếu Samsung chặn sysfs → dùng chênh W giữa D1 bật/tắt.

**D1. Swapchain = khung logic (1040×936 / 1436×616), màn hình tự phóng.** SDL 3.2.30 (`SDL_gpu_vulkan.c`) trên Android đặt kích thước swapchain
= `SDL_GetWindowSizeInPixels` **kẹp trong `[minImageExtent, maxImageExtent]`** (`VULKAN_ClaimWindow` dòng 9829–9836, sự kiện đổi cỡ 9716–9717,
`CreateSwapchain` 4670–4679); loader Vulkan của Android báo min 1×1 / max 4096×4096 → swapchain nhỏ hơn cửa sổ là hợp lệ, **HWC/DPU phóng lên
màn, không tốn pass GPU nào**. Vá qua `android/va_sdl3_donhip.py` (chạy tự động lúc CMake): hint `JX_SWAPCHAIN_W/H` ghi đè
`swapchainCreateWidth/Height` ở ba chỗ trên + in `minImageExtent/maxImageExtent` một lần vào log. Phía Represent3: đặt hint = backbuffer trước
`SDL_ClaimWindowForGPUDevice` khi `[Client] Rep3SwapchainLogic=1`; `Letterbox(swW, swH)` đọc kích thước thật từ acquire → tỷ lệ tự về 1,0, không
cần sửa thêm; `SDL_GetWindowSizeInPixels` không đổi → chạm/toạ độ không đổi; chụp màn hình (`m_pLastFrame`) vẫn đúng.
Kỳ vọng: điểm GPU phải tô **÷4,4 (màn trong) / ÷3,1 (màn ngoài)**; hết "nghẽn GPU lúc đông" (nộp + chờ 25–33 ms → < 10 ms); điện GPU giảm (đo
D0 mới biết, ước 0,3–0,8 W lúc đông). Hình ảnh **y hệt**: hiện GPU cũng phóng từng sprite 2,1× lọc tuyến tính, chữ bitmap 1×; nay phóng cả
khung một lần. Rủi ro: HWC không nhận tỷ lệ → SurfaceFlinger phóng bằng GPU một lần mỗi khung (vẫn rẻ hơn nhiều; kiểm bằng
`dumpsys SurfaceFlinger --latency` khi có adb hoặc so W). Nếu cần nét hơn: nấc "Độ phân giải" 1,0× / 1,5× trong Cài đặt. Phương án dự phòng =
đề xuất (ii) cũ (vẽ vào RT rồi phóng một lần, thêm ~0,3 ms GPU).

### Đợt E — Nạp khi đám đông tới / vào map (2–3 ngày; hết 30–60 s tụt đầu map)

**E1.** `GetImageParam` / `LoadSprFile` đồng bộ trên luồng chính (344–511 lần/30 s, tệp spr max 38 ms): khi NPC được sinh (`KNpcRes::SetSprFile`,
trước khi vào tầm nhìn) đưa **tệp .spr** vào hàng nền ngay (mở rộng `NAPNPC`), trả tham số kích thước từ header cache; `NapHoiKhongDe` đã có cho
sprite đang nạp nền → áp cho cả trường hợp chưa mở tệp. Đo: `[VE-NAP]` "ngoài lúc vẽ" → < 20 lần/30 s, `[REP3-NAP]` khung có nạp > 16 ms ≈ 0.

**E2.** Nạp trước theo bản đồ lúc màn tải: danh sách sprite NPC/quái của map + trang bị phổ biến (từ log `[VE-NAP]` các phiên) → luồng nền nạp
trong lúc màn tải hiển thị; đổi map 150–300 ms tick nằm sau màn tải nên không cần sửa.

### Đợt F — Dữ liệu (không liên quan nhiệt, làm khi rảnh)

Đóng lại pak điện thoại để 3 SPR nén-theo-khung không nằm ở mục ≥ 65 536 (hoặc < 65 536 mục/pak); xin đủ tài nguyên client (đầu ngựa 015/002,
`enemy141…169`, `boss130/202`, `passerby*`, `critter*`, MiniMap, nút kênh chat) — hết "ảnh null" 100–200 nghìn lượt/30 s và ngựa cụt đầu.

## 4. Kiểm chứng và thứ tự

- Mỗi đợt một APK (gộp `origin/mobile-0809` trước, `va_sdl3_donhip.py` chạy tự động lúc CMake, lên dt_v4, khởi động lại máy chủ 8765), bật lại
  `PaintLog=1 GuiLog=1` khi đo; hai bài × 15 phút: **yên màn trong** và **Tống Kim màn ngoài**, ở nấc 120 và nấc "Tự động"; ghi vào bảng 1.1/1.2.
- Tiêu chí đạt: **A** yên "Tự động" ≤ 1,8 W, cách khung p95 ≤ 20 ms, đông không lên nhiệt 3 trong 20 phút · **B** khung chép ≥ 10 ms < 1/phút ·
  **C** đông: lệnh/khung < 600, đổi texture < 50, ghi < 1,5 ms, fps p10 ≥ 110 ở 120 Hz · **D** đông màn trong: nộp + chờ < 10 ms, GPU % giảm
  ≥ 40 % hoặc W −0,3 trở lên · **E** khi đám đông tới không có cửa sổ 10 s < 100 fps ngoài 10 s đầu map.
- Thứ tự: **A → B → D0 → C → D1 → E → F**. A và B độc lập, có thể làm song song (A ở `JxPerfHudAndroid.cpp` + Java, B ở `D3D9onGPUDev.cpp` + shader).
  Sau A đã đủ để chủ chơi lâu không nóng; C và D là để giữ 120 fps ở cảnh đông cho người chọn "Hiệu năng".
- Ước công: A 1–2 ngày · B ½–1 · D0 ½ · C 3–5 · D1 1–2 · E 2–3 · F ½ (chờ tài nguyên).

## 5. Những gì không làm

- Không tối ưu kiểu bớt vẽ NPC/hiệu ứng, không hạ chất lượng ảnh, không đổi nhịp tick 18/s (luật chủ 11/09: sửa riêng mobile, không giảm cảm giác).
- Không sửa logic đạn/NPC thêm (đã đủ), không đụng chuỗi Windows, không đổi SDL ngoài hint kích thước swapchain (bản 3.2.30 giữ nguyên).
- Không dùng M2/chép khung (đã tắt), không đổi số khung bay (2 là bắt buộc — đã đo).

## 6. Tư vấn "sửa tận gốc, không đụng trải nghiệm" (chủ hỏi 11/09 12:30)

> **Đã làm 13:20 11/09 (commit `ccb66888`, dt_v4 = 109111313):** mục 1 (bỏ khung giống = `[BKG 11/09]`, kèm bộ đếm `[VE-BKG]`), mục 4 (bảng màu =
> storage buffer `[PALBUF 11/09]`, kèm đồng hồ "lệnh tải bảng màu" ở cả hai kiểu), A4 (`[FPSNGOAI 11/09]`), đo (c) GPU % + xung CPU/GPU vào `[MAU]`
> (`[MAU 11/09]`). Chi tiết + bài test: `BANGIAO_DONHIP_MOBILE_1209.md` §0 khối 13:20 và §10. Còn lại theo thứ tự: C → D1 → E → A2.
> **Kết quả 14:50:** cảnh yên CPU 53 → 36 %, 2,39 → 1,99 W, SoC về xung nghỉ, khung chép chậm 229 → 1; Tống Kim GPU 82–99 % bận → **D1 làm 15:15**
> (commit `9851000b`, dt_v4 = 109111459, `[D1 11/09]`, thử máy ảo màn giả 2080×1208 OK). **Kết quả D1 15:25** (Tống Kim): fps 109 → 115,
> nộp 1,6 → 0,8 ms, GPU 83 % @ 648 → 73 % @ 336 MHz, 4,61 → 3,35 W, nhiệt 3 → 2 và máy nguội dần → GPU hết nghẽn, **CPU thành nút thắt**
> (96 / 85 % ở cửa sổ 800 đạn/tick). **Đợt C bước 1 làm 15:50** (commit `00a09114`, dt_v4 = 109111545, `[GOP 11/09]`: C2 trạng thái tầng texture
> theo đỉnh + C3 bind ring một lần; mảng shader PC giữ nguyên byte nhờ macro). **Kết quả bước 1 16:10:** đổi trạng thái pixel 1 105 → 0 mỗi khung,
> giá mỗi lệnh vẽ 1,37 → 1,07 µs (−22 %), CPU 80/75 → 77/73 % dù cảnh nặng gần gấp đôi; đổi texture vẫn 1 139/khung → đúng phần C1.
> **C1 đã dựng + thử máy ảo 16:10** (commit `[MANG 11/09]`, APK 109111608, atlas thành texture mảng 2D), **chưa đẩy bộ tải** vì chủ còn đang đo
> bản 109111545. Còn: A2 (lưới an toàn nhiệt) → E (nạp sprite nền) → BKG b (xin 60 Hz khi đứng yên).

**Gốc thật sự là gì.** Engine JX1 là engine 2D kiểu D3D9 tức thời: mỗi sprite = một lệnh vẽ mang texture + trạng thái riêng, vẽ lại toàn bộ
màn hình mỗi khung, ở PC 60 Hz driver D3D9 gánh được. Trên điện thoại ở 120 Hz, cùng mô hình đó đi qua lớp mô phỏng SDL-GPU (mỗi lệnh 1–3 µs
CPU, mỗi đổi texture một descriptor set) và GPU tô ở độ phân giải gốc gấp 4,4 lần khung logic → chi phí mỗi khung cao, rồi nhân 120. Việc
hạ FPS (A1) chỉ **giảm số lần trả giá**, không giảm **giá mỗi khung**. Sửa tận gốc = giảm giá mỗi khung và **không trả giá cho khung không đổi**.

Bốn gốc và cách sửa **pixel y hệt, độ trễ y hệt**, xếp theo lợi/công:

1. **Khung không đổi thì không trình chiếu** (mới, chưa có trong đợt A–F). Ở cảnh yên, hai khung liên tiếp phần lớn giống nhau từng byte
   (logic 18 tick/s; nội suy chỉ đổi toạ độ vật đang di chuyển). Lớp `CDevGpu` đã có sẵn toàn bộ khung dưới dạng dữ liệu (`m_cmds` + `m_ring`,
   100–300 KB): so với bản khung trước (`memcmp`, ~20–50 µs); **giống hệt và không có texture chờ tải → bỏ `SubmitFrame(true)`**, không acquire
   swapchain, không nộp, GPU không tô; khác một byte → trình chiếu như thường. Màn hình hiện đúng những pixel đó, chạm/di chuyển được vẽ ngay
   khung kế → người chơi không phân biệt được. Đứng yên trong thành: trình chiếu tự rơi về ~18–30 khung/giây (theo tick), ghi + nộp + GPU
   (≈ 2 ms CPU + toàn bộ GPU mỗi khung) biến mất; Samsung thấy ít khung sẽ tự hạ màn xuống 60/48 Hz (`[DOI]`) → thêm điện màn hình. Bước 2 (tuỳ
   chọn): sau 3 khung giống nhau thì chỉ chạy mã vẽ khi có tick / chạm / đổi camera → bớt nốt veCPU 1,6 ms. **Làm bộ đếm trước** ("khung giống
   khung trước" mỗi 30 s trong `[VE]`) để biết tỷ lệ thật trên Fold 7 rồi mới bật bỏ trình chiếu; công tắc `[Client] Rep3BoKhungGiong=1`.
   Rủi ro: hiệu ứng nhấp nháy theo khung (con trỏ chat, chớp) làm khung khác → chỉ mất cơ hội bỏ, không sai hình. Công: 1 ngày.
2. **Đóng gói lệnh tại nguồn** (đợt C): atlas thành texture mảng 2D + trạng thái ps theo đỉnh + bind ring một lần → số lệnh chỉ còn phụ thuộc
   số lần đổi chế độ blend (55–197/khung) thay vì số sprite. Đây là gốc của "đông là tụt fps": 2 500–2 900 lệnh → 300–500, ghi 3,5 → ~1 ms,
   nộp 2,3 → ~1,5 ms. Không đổi thứ tự vẽ, không bớt sprite → hình y hệt.
3. **GPU tô đúng số điểm cần** (D1): swapchain = khung logic, màn hình phóng bằng phần cứng hiển thị. Hết nghẽn GPU lúc đông màn trong, điện
   GPU giảm; kiểm bằng mắt một lần vì nếu game đang lọc điểm thì ảnh sau phóng mịn hơn một chút (có công tắc để so).
4. **Bảng màu ra khỏi texture 256×8192** (đợt B): hết 12–50 cú khựng/phút, không đổi gì về hình.

Cộng thêm E (nạp tệp spr ở luồng nền khi NPC được sinh) để hết tụt 30–60 s đầu map; A4 (30 fps ngoài thế giới) vì không có gì để xem.

**Vai trò của FPS trong hướng này:** giữ "Tự động" = 120 trên máy 120 Hz như chủ muốn; A2 (hạ nấc khi Android báo MODERATE/SEVERE) chỉ là
lưới an toàn giống mọi game mobile — chỉ chạy khi máy thật sự nóng, lúc đó hệ điều hành cũng sắp giảm xung và fps sẽ tụt tệ hơn nếu không hạ.
Không đặt mặc định 60 (A1) nữa; nấc 60/90 vẫn để người chơi tự chọn khi muốn tiết kiệm pin.

**Chứng minh gốc trước khi sửa** (mỗi thứ ½ ngày, chỉ thêm bộ đếm, không đổi hành vi): (a) tỷ lệ khung giống khung trước ở cảnh yên/đông;
(b) đồng hồ con từng lệnh tải bảng màu (B0); (c) GPU % + xung CPU/GPU vào `[MAU]` (D0). Ba số này quyết định thứ tự làm và cho phép so trước/sau
bằng cùng bài đo 15 phút yên + 15 phút Tống Kim.

**Thứ tự đề nghị theo hướng tận gốc:** đo (a)(b)(c) → B (nhỏ, hết khựng) → 1 (bỏ khung giống, lợi lớn nhất cho pin lúc chơi bình thường) →
C (đông) → D1 (GPU) → E → A2 + A4. Kỳ vọng cuối: yên 120 Hz ≈ 1,5–1,8 W (nay 2,1–2,8), đông 120 Hz ≈ 3–3,5 W và giữ 117–120 fps (nay 5–6 W,
97–114 fps), pin chơi bình thường ≈ 9–10 giờ (nay ≈ 7).
