# RAM CLIENT CAO — nguyên nhân đo được và bản thử D3D9Ex (08/09/2026, 04:50)

Chủ 04:05: *"tìm nguyên nhân ram cao. Đối với 1 client ram > 500 mb ko ổn."* Số chủ thấy: `jx_rep3.log` ghi `RAM rieng` 771–816 MB sau vài
giờ chơi (07/09 20:23 → 08/09 04:00), Task Manager tương đương.

Mọi số dưới đây đo trên tiến trình `Game.exe` đang chạy bằng công cụ chỉ đọc `ReverseTools/do_ram_client_0809.py` (psutil + VirtualQueryEx),
không sửa gì trong tiến trình.

---

## 1. RAM riêng là RAM thật, không phải chỉ số ảo

| Chỉ số (pid 47564, 04:24, cache texture 202 MB) | Giá trị |
|---|---|
| commit (PrivateUsage — số `RAM rieng` trong jx_rep3.log) | 423 MB |
| working set | 421 MB |
| **USS = trang riêng đang nằm trong RAM vật lý** | **386 MB** |
| Lần đo 04:40, cache texture 395 MB | commit 563 MB, USS **524 MB** |

→ Con số chủ thấy là RAM vật lý thật (USS chỉ thấp hơn commit 7–9 %).

## 2. Thành phần (bản đồ vùng nhớ, 2.234 vùng)

| Phần | Cỡ | Là gì | Đã kiểm |
|---|---|---|---|
| **Bản sao texture của driver** | **~0,7 MB mỗi MB texture cache** (16 vùng × 15,8 MB = 253 MB khi cache 250 MB) | Vùng private RW, mỗi vùng ~110 mảnh 0,1–0,6 MB (đúng cỡ một texture 256×256×4), **không đọc được từ ngoài** (`ReadProcessMemory` thất bại) = bộ nhớ driver ánh xạ. D3D9 cũ trên WDDM: driver phải giữ bản sao hệ thống của tài nguyên POOL_DEFAULT | ✓ đo cấu trúc vùng; hồi quy 930 mẫu phiên 20:23: `RAM = 500 + 0,36 × tex`, phiên 04:10: `213 + 0,83 × tex` |
| **Mảng tĩnh CoreClient.dll `.bss` 103 MB** | 103 MB (SizeOfImage 105,6 MB, file chỉ 2,5 MB) | `KSubWorld.obj` **54,9 MB** = lưới đường đi `VGridNode m_GridNode[MAX_CELL=2.400.000]` (20 B/ô = 48 MB) + `int m_pTempCover[MAX_CELL]` (9,6 MB) — `KSubWorld.h:36,317,325`; `KNpc.obj` **42,1 MB** = `Npc[MAX_NPC=800]` × ~52 KB (`KNpc.h:23`, client 256→800 đợt REP3); `KMissle.obj` 3,4 MB = `Missle[500]` | ✓ `dumpbin /headers` từng .obj |
| Heap của game | ~140 MB (vùng < 4 MB: 75 + 46 + 19 + 12) | bảng kỹ năng/vật phẩm/NPC, script Lua, UI, âm thanh… chưa tách được từ ngoài | ○ cần `HeapSummary` trong tiến trình |
| Mã DLL đang nằm trong RAM | CoreClient.dll 2,1 MB code; **nvd3dum.dll 60,5 MB + nvgpucomp32.dll 62,6 MB** ánh xạ, phần chạm tới nằm trong WS | driver NVIDIA | ✓ |
| raw SPR (byte RLE của SPR nén cả tệp) | 9–49 MB, đã có trong thống kê | không ngân sách riêng, chỉ thả khi node bị xoá | ✓ |

**Không thấy rò rỉ:** phần dư `RAM − 0,6 × (texture + raw)` từng giờ trong 8 giờ: 330, 297, 397, 342, 333, 318, 351, 402 MB — dao động,
không tăng đều.

**Kết luận:** RAM > 500 MB đến từ (a) cache texture lớn khi đánh đông (đỉnh 768 MB) **cộng** bản sao ~0,7× của driver, trên nền (b) ~100 MB
mảng tĩnh + (c) ~140 MB heap. Với cache 700 MB: 250 + 0,7 × 700 ≈ 740 MB, đúng số đo.

---

## 3. Bản thử D3D9Ex — `Represent3.dll.moi` 72f81e3e, `[Client] Rep3Ex=1` (commit `[RAM 08/09]`)

Giả thuyết: D3D9Ex để WDDM tự quản lý phân trang tài nguyên video nên driver **không cần** giữ bản sao hệ thống → RAM riêng chỉ còn nền
~250 MB dù cache 700 MB. **Phải đo mới biết**, vì đây là hành vi driver.

Đã làm (script `ReverseTools/goi_va_ram1_rep3_d3d9ex_0809.py`, mặc định `Rep3Ex=0` = hành vi cũ y hệt):
- `D3D_Shell::Create`: `Rep3Ex=1` → `Direct3DCreate9Ex` (tra hàm động), `m_pD3DEx`, `g_nRep3ExOn=1`; hỏng thì về `Direct3DCreate9`.
- `CD3D_Device::CreateDevice`: có Ex → `CreateDeviceEx` (windowed: NULL; fullscreen: `D3DDISPLAYMODEEX` theo mode); hỏng → `CreateDevice` cũ.
- D3D9Ex **không có POOL_MANAGED**: 8 chỗ (3 texture pre-render, 4 chỗ TextureRes BITMAP16/DXT, texture chữ `KFontRes.cpp:77`, probe NPOT)
  → `DYNAMIC + DEFAULT` khi Ex (vẫn `LockRect` được); `Rep3Pool` ép = 1 (đệm SYSTEMMEM + DEFAULT).
- Log `[REP3] D3D9Ex: BAT/tat`, dòng `[REP3] Create ... ex=%d`.
- Đã thêm `Rep3Ex=1` vào `bin\client\config.ini` (bản lưu `config.ini.truoc_rep3ex`). **Tắt thử: đặt `Rep3Ex=0`.**

### 3.1 Đọc kết quả sau khi chủ vào lại game và đánh một trận

```bash
grep -a "D3D9Ex\|Create " "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_rep3.log" | tail -3
grep -a "REP3\] RAM rieng" "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_rep3.log" | tail -20
python D:/GAMEDEVNEW_wt_delta/ReverseTools/do_ram_client_0809.py
```

| Kết quả | Nghĩa | Bước tiếp |
|---|---|---|
| `RAM rieng` tăng < 0,2 MB mỗi MB texture, không còn vùng 15,8 MB không đọc được | bản sao driver đã hết | đặt `Rep3Ex=1` mặc định trong mã; RAM đỉnh ≈ 250 + heap |
| vẫn ~0,7 MB/MB | driver vẫn giữ bản sao dù Ex | còn hai đường: `Rep3Tex32=0` (A4R4G4B4, giảm nửa cả VRAM lẫn RAM, mất độ mịn màu) hoặc hạ `Rep3CacheMB` (nay nhánh vượt ngân sách đã sửa, thử 512) |
| hình/chữ lỗi (chữ không hiện, ảnh pre-render hỏng) | texture DYNAMIC+DEFAULT chưa thay được MANAGED ở chỗ nào đó | `Rep3Ex=0`, đọc `jx_rep3.log` `tao_hong`/`tex_null`, sửa chỗ đó |
| fullscreen không tạo được device | `CreateDeviceEx` cần refresh rate | có dòng `CreateDeviceEx that bai` trong debug log; đã tự lùi về `CreateDevice` |

---

## 3.2 KẾT QUẢ ĐO 09:11–09:35 (Rep3Ex=1 thật, `[REP3] D3D9Ex: BAT`): **KHÔNG giảm**

31 mẫu 30 s trong trận Tống Kim 09:22: `RAM riêng = 208 + 0,77 × (texture + raw)` — đúng hệ số 0,76 của bản D3D9 cũ. Bản đồ vùng nhớ
vẫn 18 vùng 15,8 MB không đọc được, tăng theo cache. Hình ảnh không lỗi (`tex_null 0, tao_hong 0`, fps 63), tức 8 chỗ MANAGED → DYNAMIC
chạy đúng, nhưng **driver vẫn giữ bản sao texture trong RAM dù là D3D9Ex**. Đã đặt lại `Rep3Ex=0` trong config (không lợi thì không giữ
biến số). Giả thuyết "D3D9Ex bỏ bản sao" bị **bác bỏ** bằng số đo.

## 3.3 Đòn bẩy còn lại KHÔNG đổi hình ảnh: texture bảng màu 2 byte/điểm (lossless)

`RenderToA8R8G8B8` (`TextureRes.cpp`) cho thấy điểm ảnh SPR = **chỉ số bảng màu 8 bit + alpha 8 bit theo run**; hiện được bung ra
A8R8G8B8 4 byte/điểm. Lưu **A8L8** (L = chỉ số, A = alpha, 2 byte/điểm) + bảng màu 256×1 (1 KB/sprite) và tra bảng trong **pixel shader
ps_2_0** (point sampling — đúng cách vẽ 1:1 không lọc của DirectDraw gốc) → cùng màu y hệt, **texture giảm một nửa**: 735 → ~370 MB,
bản sao driver −280 MB, VRAM −370 MB, giải mã nhanh hơn (không bung palette). Việc: shader + đường vẽ `DrawSpriteAlpha`/`DrawImage2DFlat`
đặt stage 1 = bảng màu, `CreateTexture16Bit` sinh A8L8; giữ đường 8888 cũ làm dự phòng (`Rep3Pal=0`). Công ~1–2 ngày, cần test toàn bộ
UI/chữ/hiệu ứng. Đây là cách duy nhất còn lại giảm phần texture mà không mất gì.

## 4. Các đòn bẩy khác cho RAM (chưa làm, xếp theo lợi)

| Việc | Giảm | Rủi ro / giá |
|---|---|---|
| Lưới đường đi 2,4 triệu ô tĩnh (58 MB): cấp động theo map đang đứng, hoặc `VGridNode` gọn (20 → 12 B) | 30–58 MB | đụng A* chạy bộ xuyên map (`jx1-luoi-astar-son-dac-dat-that`), phải kiểm cả Dã Tẩu/bản đồ thế giới |
| `Npc[800]` × 52 KB: bọc `#ifdef _SERVER` các trường chỉ máy chủ dùng trong `KNpc` (AI, bot, PB) | 10–25 MB | đổi layout KNpc client, không đụng gói tin; build cả hai bản |
| Heap 140 MB: đo bằng `HeapSummary` + đếm theo hệ (skill/item/script/sound) trong dòng 30 s | chưa biết | 1 build Represent3/Core |
| raw SPR: thả byte RLE sau khi đã tạo texture, đọc lại từ pak khi cần | 10–50 MB | nạp lại chậm hơn khi bị thải |
| `Rep3Tex32=0` | ½ texture + ½ bản sao | màu 4 bit/kênh, hiệu ứng có dải màu |
| LARGEADDRESSAWARE cho Game.exe | không giảm RAM, chỉ nới trần 2 GB | rà bit dấu con trỏ |

Bàn giao liên quan: `BANGIAO_HIEUUNG_KHAOSAT_0709.md` (mục 7: bản đo `[FX]`), `BANGIAO_BANGTHONG_DONGNGUOI_0609.md` 8.24.

## 5. So với client 2.0 mẫu (chủ 08/09 10:5x: "2.0 đông lắm cũng < 250 MB, sao bản tôi cao hơn nhiều")

Bằng chứng: bản mổ `represent3free.dll`/`gamecl.exe` 03/09 (`BANGIAO_REPRESENT3_VLTK2_0309.md` mục 4), PE header hai bên, `jx_rep3.log`
pid 8916 lúc 10:5x (sau trận): **RAM riêng 744 MB = texture 610 MB (vẽ khung này 201 MB) + raw spr 44 MB + phần còn lại**.

| | Client 2.0 mẫu (Level Up) | Bản dự án đang chạy | Hệ quả RAM |
|---|---|---|---|
| Định dạng texture sprite | **A4R4G4B4, 2 byte/điểm** (16 bit, màu 4 bit/kênh) | **A8R8G8B8, 4 byte/điểm** (`Rep3Tex32=1` mặc định) | ×2 |
| Ngân sách cache texture | **30/50/80/120 MB** theo RAM máy (máy này 120), TTL 10 s, bỏ 1 khung/lượt | **`Rep3CacheMB=1500`** (chủ đặt 07/09 để hết mất hiệu ứng; mặc định RAM/16 kẹp 1024) → giữ 610 MB, trong đó một khung chỉ cần 201 | ×5 |
| Pool | MANAGED: RAM giữ bản sao = VRAM (≤ 120 MB) | DEFAULT (VRAM) + bản sao driver ~0,8× | tương đương nhau |
| Raw SPR (RLE nén) giữ trong RAM | có, cache engine nhỏ | 44–57 MB, chỉ thả khi node bị xoá | +50 |
| Mảng tĩnh | `gamecl.exe` ảnh 33 MB (UPX, cả mã) → tĩnh ≈ 20–30 MB | `CoreClient.dll` `.bss` **110 MB** (lưới đường A* 58 + `Npc[800]` 42 + `Missle[3000]`) | +80 |
| Đám đông lúc đo | người chơi thật, trang bị lặp | **1.000 bot** trang bị/phái ngẫu nhiên → 2.100+ sprite khác nhau, 201 MB/khung ở 32 bit | working set lớn hơn |

Cộng lại: 2.0 ≈ 60 nền + 30 tĩnh + heap + **≤ 120 texture** ≈ 250 MB. Dự án ≈ 177 nền/heap + tĩnh + **0,8 × (610 + 44)** ≈ 740 MB.
**Khác biệt không phải rò rỉ hay lỗi, là chính sách: 2.0 đổi CPU (giải mã lại sprite khi bị thải, màu 16 bit) lấy RAM; dự án đang
giữ mọi thứ đã giải mã ở 32 bit với ngân sách 1,5 GB.** Ngân sách 1.500 là do tôi đề xuất và chủ đặt để chữa "mất hiệu ứng" 17:xx
07/09 — mà gốc thật của lần đó là hai lỗi dọn cache (`<=` và không trừ bộ nhớ đã thả) + trần 512, nay đã sửa trong mã.

### 5.1 Hai đường về mức 2.0, chủ chọn

| | Cách | RAM dự kiến trong trận 1.000 bot | Mất gì | Công |
|---|---|---|---|---|
| **Thử ngay 5 phút** | `config.ini [Client]`: `Rep3Tex32=0` (đúng 4444 như 2.0, bộ giải mã có kiểm biên `RenderToA4R4G4B4Safe` đã có) + `Rep3CacheMB=160` (2.0 dùng 120; working set 16 bit của ta ~100 MB/khung) | ≈ 177 + 0,8 × (200 + 44) ≈ **370 MB**; đám đông thường ≈ 300 | màu 4 bit/kênh **y như 2.0 chủ đang chấp nhận**; giải mã lại nhiều hơn (đo `giai_ma` trong `jx_rep3.log`, hiện 12–393 khung/30 s) | 0 build |
| **Không mất màu** (B2 mục 3.3) | texture bảng màu A8L8 2 B/px + shader tra bảng; cùng ngân sách 160 | như trên, **màu đúng 100 %** | không | 1–2 ngày |
| + thêm | bỏ raw SPR sau khi tạo texture (đọc lại pak khi cần) −44; lưới đường cấp theo map −50; `KNpc` bọc `_SERVER` −20 | **≈ 250 MB** | không | 2–3 ngày |

Lưu ý: ngân sách 160 chỉ là "mềm" — khung nào cần hơn thì cache vẫn vượt (dọn chỉ đụng khung nghỉ > 10 s, 1 khung/lượt), nên
không lặp lại lỗi 17:xx; theo dõi `[REP3] ... fx: tex_null tao_hong` phải = 0 và `giai_ma` ms/30 s không tăng đột biến.

## 6. D3D11 — thí nghiệm quyết định B0 và lớp "D3D9 trên D3D11" (08/09 11:xx–12:xx, chủ: "bắt tay làm luôn d3d11")

### 6.1 B0 (`ReverseTools/represent3/b0_d3d11_ram.cpp`, 32 bit, RTX 3080, RAM riêng của tiến trình)

| Cách tạo 700 MB texture 256² | RAM riêng tăng | 20.000 texture 64² (312 MB) | 10.000 texture 128² (625 MB) |
|---|---|---|---|
| D3D9 DEFAULT qua SYSTEMMEM + UpdateTexture (đường game) | **+143 MB** (0,2×) | +176 MB (0,56×, ~9 KB/texture) | +176 MB |
| D3D9 MANAGED + LockRect | +750 MB (1:1) | — | — |
| **D3D11 DEFAULT tạo kèm dữ liệu** / IMMUTABLE | **+25…36 MB (0,04×)** | +119 MB (6 KB/texture) | +78 MB |
| D3D11 DEFAULT rỗng + UpdateSubresource dồn 2.800 lần | +457 MB (đệm upload không trả) | +Flush mỗi 16: +40 | — |
| D3D11 STAGING + CopyResource mỗi texture | +590 (còn 333 sau thả) | — | — |
| D3D11 DYNAMIC (Map) | +749 committed | — | — |

Đọc: hệ số 0,8×/MB đo trong game là do **hàng vạn texture nhỏ**, D3D9 tốn ~9–44 KB cố định mỗi texture; D3D11 tạo kèm dữ liệu gần như
không tốn RAM theo byte, còn ~6 KB mỗi texture. Thiết kế: tạo texture GPU **đúng lúc UpdateTexture với dữ liệu sẵn**, không dùng
UpdateSubresource/STAGING hàng loạt.

### 6.2 Thi công: lớp "D3D9 trên D3D11" — Represent3 và KFont3 giữ NGUYÊN VĂN

`D3D9on11i.h` + `D3D9on11.cpp` (CTex11/CSurf11/CVB11/CSB11) + `D3D9on11Dev.cpp` (CDev11) + `D3D9on11D3D.cpp` (CD3D11Shim) cài tập con
`IDirect3D9`/`IDirect3DDevice9`/`Texture9`/`Surface9`/`VertexBuffer9`/`StateBlock9`; shader `Rep3Shaders11.hlsl` (VS: XYZRHW +0,5 px
để texel khớp D3D9, XYZ nhân W·V·P; PS: 2 stage COLOROP/ALPHAOP + alpha test so 8 bit). Nối: `D3D_Shell::Create` → `Rep3_CreateD3D9on11()`
khi `[Client] Rep3Api=11` (lỗi → lùi D3D9 tự động); `Rep3CreateTex` thay `D3DXCreateTexture`; `Rep3Flip` (0 = bitblt, mặc định; 1 = flip).
Hàm chưa cài ghi log `[D3D11] CHUA CAI: ...` một lần (harness: 0 dòng).

### 6.3 Kiểm bằng harness (cùng sprite, cùng DLL, đổi `Rep3Api`)

| | D3D9 | D3D11 |
|---|---|---|
| Ảnh 1024×768 (8 sprite × 7 kiểu vẽ + chữ) | tham chiếu | khác **0,005 %** điểm (7 hàng cuối vùng chữ), không lệch nửa điểm |
| Vẽ 96 sprite/khung, cửa sổ hiện | 4,5 µs/sprite | **2,7 µs/sprite** (bitblt); flip 8,8 (Present +0,6 ms/khung) |
| tex32=0 (4444) / tex32=1 (8888) | đúng | đúng (4444 native trên card này, RT 16 bit đổi BGRA8) |
| Chụp `SaveScreenToFile` ngoài màn hình | thất bại | chạy (bản sao khung cuối) |

### 6.4 Đưa vào game
`bin\client\Represent3.dll.moi` + `config.ini [Client] Rep3Api=11` — chủ restart `ChoiGame.bat`, đọc `jx_rep3.log`: `[REP3] API: Direct3D 11`,
`[D3D11] thiet bi: ...`, dòng `RAM rieng ... | gpu tex N (MB) | d3d11: present TB ms, ve N lenh`. Kỳ vọng: RAM riêng ≈ nền + 0,05 × texture
(+6 KB × số texture) thay vì + 0,8 × texture → **−350…−450 MB** ở cache 570 MB. Lùi: `Rep3Api=9`. Chưa test trong game: bản đồ nhỏ
(render-to-image), toàn màn hình, gamma, Alt+Tab, mất thiết bị.

### 6.5 Lần chạy thật đầu tiên (11:34) — lỗi hình và số RAM thật

- **Chủ báo:** "lỗi hình ảnh 1 số map", "mới qua map mới sẽ bị lỗi", "di chuyển hay bị giật đen màn". Ảnh: ô nền đen, mảnh rác.
  **Gốc:** ring đỉnh — sau Present đặt `m_ringPos = 0` rồi Map `NO_OVERWRITE` ghi đè vùng GPU còn đang đọc của khung trước
  (harness không lộ vì mỗi khung vẽ y hệt). **Sửa [c]:** Map đầu tiên mỗi khung dùng `DISCARD`.
- **RAM thật với D3D11 (chưa atlas):** hồi quy 14 mẫu `RAM = 185 + 0,74 × (texture+raw)` — KHÔNG giảm so với D3D9 (0,77–0,95).
  Lý do (đo được): dòng `gpu tex` cho thấy game giữ **25.000–32.000 texture GPU, trung bình 14 KB**; B0 đo thêm texture nhỏ:
  64² × 20.000 → D3D9 +176 MB, D3D11 +119 MB (~6 KB/texture); 100² NPOT × 10.000 → D3D9 +165, D3D11 +76. Tức driver tốn ~6–9 KB
  RAM **mỗi texture** bất kể API; với 30.000 texture 14 KB thì phần cố định ≈ 0,5× byte texture. D3D11 chỉ bỏ được phần tỉ lệ theo
  byte, không bỏ được phần theo số lượng.
- **Sửa [d] — atlas trong lớp shim:** texture DEFAULT ≤ 512 px không phải render target = một ô trong trang BGRA8 1024×1024
  (lớp bin luỹ thừa 2 + 1 px chống tràn mẫu), uv được nhân/dịch lúc chép đỉnh vào ring (Represent3 không đổi); trang rỗng giữ tối đa
  một trang mỗi lớp. Kỳ vọng: vài trăm đối tượng GPU thay vì hàng vạn → RAM ≈ nền + ~0,05 × texture. `Rep3Atlas=0` để tắt.
  Harness: ảnh trùng D3D9 99,996 %, 4,3 µs/sprite. Dòng `[REP3]`: `gpu tex N (MB, P trang MB)`.
- `Represent3.dll.moi` (c+d) đặt lại, chờ chủ restart; nếu còn lỗi hình → `Rep3Atlas=0` (chỉ còn sửa c), rồi `Rep3Api=9`.

### 6.6 Lần chạy 11:51 (bản c+d: ring DISCARD + atlas) — KẾT QUẢ

| Mốc (30 s) | texture cache | RAM riêng | gpu tex | trang atlas | fps |
|---|---|---|---|---|---|
| +0,5 phút | 193 MB | 315 MB | 11.390 | 118 (472 MB VRAM) | 63 |
| +1,5 phút | 369 MB | 340 MB | 29.328 | 214 (856 MB) | 61 |
| +3 phút | 433 MB | 352 MB | 38.313 | 248 (992 MB) | 63 |
| +6,5 phút | 479 MB | **347 MB** | 45.113 | 273 (1.092 MB) | 63 |

Hồi quy: **RAM ≈ 300 + 0,1 × texture** (trước: 185 + 0,74; D3D9: 177 + 0,95). Ở texture 480 MB: **347 MB thay vì ~640 MB (−45 %)**.
Present 0,06–0,10 ms; 0,6–0,8 µs mỗi lệnh vẽ (800–3.600 lệnh/khung); khung giật 0–1/10 s (max 27 ms); 0 hàm thiếu, 0 lỗi tạo texture,
0 crash. Chủ chưa báo lại về hình sau bản c.

Còn lại: (1) phí VRAM trang 2,3× (bin pow2(w+1)) → bản [e] bin 16/24/32/48/64/96/128/192/256/384/512 (≤ 1,5× mỗi chiều), bỏ +1 px
(sprite vẽ POINT); (2) `anh_null` 200k–1,3M/30 s = 700 lượt/khung xin 200 sprite biểu cảm THIẾU TỆP (spr/Ui3/<GBK>/140–339.spr) — tốn
CPU vô ích, cần chép tệp hoặc sửa UI không xin; (3) `Rep3CacheMB=1500` → trang có thể tới ~2,5 GB VRAM: theo dõi (`VRAM con` hiện kẹp 4095).

### 6.7 12:xx — "gợn sóng khi di chuyển", VRAM, 200 sprite biểu cảm

- **Gợn sóng = xé hình (tearing):** swapchain bitblt DISCARD (Rep3Flip=0) + Present(0) ở chế độ cửa sổ bị DWM ghép giữa chừng khi game
  vẽ 63 fps trên màn 59 Hz. **Bản [f]:** mặc định `Rep3Flip=1` (flip model), `Rep3Tearing=0` (không ALLOW_TEARING) → DWM chỉ ghép khung
  trọn vẹn, Present không chặn (interval 0). Harness: ảnh trùng D3D9, present 1,8 ms khi cửa sổ hiện (cửa sổ game thật đo sau).
  `Represent3.dll.moi` e57998dc chờ restart. Muốn độ trễ thấp nhất chấp nhận xé: `Rep3Tearing=1`.
- **VRAM thật (Windows counter GPU Process Memory, Game.exe pid 5684, bản [e]):** 1.083 MB dedicated; card 10 GB, tổng máy dùng 3,1 GB.
  Dòng `[REP3]` cũ ghi `VRAM con 4095` là do kẹp UINT; bản [f] ghi `vram dùng/ngân sách MB` thật (harness: ngân sách tiến trình 9.283 MB).
- **Biểu cảm:** `ui/ChatPics.ini` khai 911 (`spr/Ui3/<GBK>/N.spr`), pak đang mount chỉ có 0..139; **140..322 nằm trong `vltkcache.pak`
  KHÔNG có trong package.ini**; 323..910 không có ở đâu (kể cả client Level Up / 2.0). Đã rút 184 tệp rời vào `spr/Ui3/<GBK>` (tệp rời
  được ưu tiên trước pak; tên thư mục = byte GBK giải theo ACP 1252) bằng `ReverseTools/pak_vltk/rut_bieucam_0809.py --rut`, và
  `Count=911 → 323` (bản lưu `ChatPics.ini.truoc_bieucam`). Cần restart client (TextPic nạp lúc khởi động).
- `anh_null` trong dòng `[REP3]` gồm cả "khung ngoài tầm" (`nFrame >= m_nFrameNum`) chứ không chỉ ảnh thiếu — chưa tách.

### 6.8 12:19 (bản [f] flip) — Present CHẶN; bản [g+h+i]

Log phiên 12:19 (flip, không tearing): `present TB 6–16 ms`, fps ghim 60 (màn 59 Hz), `jx_paint` passes avg 9 ms (trước 1 ms):
DXGI xếp hàng tối đa 3 khung rồi CHẶN Present khi game vẽ 63 fps > 60 Hz. RAM 371–378 MB ở texture 525 MB (D3D9 ~680); VRAM thật
1.021 MB / ngân sách 9.283 (atlas 283 trang = 1.132 MB, 2,16× texture); gpu tex 48.439.

**[g]** `IDXGIDevice1::SetMaximumFrameLatency(1)` + `Present(0, DXGI_PRESENT_DO_NOT_WAIT)`: hàng đầy → `WAS_STILL_DRAWING` → bỏ khung
(đếm `bo`), không chờ, không xé hình → giống D3D9 cửa sổ. Harness: present 0,03 ms.
**[h]** dòng `[REP3] anh_null top:` 8 tên ảnh + khung bị bỏ qua nhiều nhất mỗi 30 s (để biết 900 lượt/khung là gì).
**[i]** atlas xếp kệ: trang theo lớp chiều cao (16…512), hàng rộng 1024, ảnh chiếm đoạn rộng w (first-fit), thả thì gộp đoạn →
phí chỉ do làm tròn chiều cao (≤ 1,5×) + đoạn thừa cuối hàng; kỳ vọng ~1,3× thay vì 2,16×.
`Represent3.dll.moi` 0ef5b550 chờ restart. Harness ảnh vẫn trùng D3D9 (0,004 %).

**Bảng so sánh các phiên (cùng máy, cùng cache 1500):**

| Phiên | Lớp vẽ | RAM riêng theo texture | RAM ở ~500 MB texture | Present | fps |
|---|---|---|---|---|---|
| 09:35 | D3D9 | 177 + 0,95× | ~650 | không đo (không chặn, không xé) | 63 |
| 11:34 | D3D11, chưa atlas | 185 + 0,74× | ~555 | flip + tearing, 0,1 ms | 63 |
| 11:51 | D3D11 + atlas [d] | ~300 + 0,1× | **347 @ 479** | bitblt 0,07 ms, có "gợn sóng" | 63 |
| 12:19 | + flip không tearing [f] | ~300 + 0,1× | **375 @ 525** | 6–16 ms, CHẶN | 60 |
| chờ | + [g] DO_NOT_WAIT, [i] kệ | như trên | như trên | ~0,05 ms, bỏ ~3 khung/s | 63 |

### 6.9 12:38 (bản g+h+i) — kết quả, và bản [j] gộp lệnh vẽ

| Mốc | texture | RAM | gpu tex | trang atlas (kệ) | VRAM dùng | present | bỏ khung | fps |
|---|---|---|---|---|---|---|---|---|
| +1 phút | 329 MB | 345 | 25.179 | 99 (396 MB) | 482 | 0,19 ms | ~30/s | 63 |
| +4 phút | 461 MB | 344 | 43.849 | 144 (576 MB) | 573 | 0,15 ms | ~31/s | 63 |

- Atlas xếp kệ: **576 MB trang cho 461 MB texture = 1,25×** (ô cố định cũ 2,16×). RAM 344 MB.
- Present không chặn (0,15 ms) nhưng **độ trễ 1 khung làm bỏ ~30 khung/s** (DWM chỉ nhận mỗi vsync thứ hai) → hiển thị ~32 fps
  dù logic 63. **Bản [j]: độ trễ 2** → bỏ ~3 khung/s, hiển thị 60.
- `anh_null top`: (1) tên RỖNG khung 0: ~35.000 lượt/s (~550/khung) = đơn vị vẽ không có ảnh (ô trang bị trống?) — GetImage trả
  NULL ngay, rẻ; (2) `MA_HR_015_*` / `FM_HR_015_*` / `FM_HR_012_HR01` 0,5–1k/s: **bộ sprite tóc/mũ HR_015 (nam+nữ) và FM_HR_012
  KHÔNG CÓ trong pak nào** (kể cả client Level Up / 2.0) → bot đội kiểu 15 không vẽ phần đó; (3) `MA_LW_000_AT05 (k107)`: xin
  khung 107 vượt số khung của sprite → dữ liệu hoạt ảnh lệch. Đây là việc dữ liệu, chủ quyết.
- **[j] gộp lệnh vẽ:** quad (strip 4 đỉnh: sprite, bitmap, ô màu) liên tiếp cùng trạng thái (cùng trang atlas, blend, sampler,
  raster, hằng số VS/PS, viewport, layout) → gom thành MỘT Draw danh sách tam giác; trạng thái chụp lúc đưa lệnh; xả khi đổi
  trạng thái, lệnh khác loại, Present, Clear, SetRenderTarget, đọc lại khung, cập nhật/thu texture. Harness: ảnh trùng D3D9,
  1,9 µs/sprite. Trong game kỳ vọng số Draw giảm vài lần (thống kê `gop N quad -> M Draw`). `Rep3Batch=0` tắt.
  `Represent3.dll.moi` ff84f787 chờ restart.

### 6.10 12:49 (bản [j]) — "có bị giảm FPS không?" → CÓ ở bản j, sửa ở bản [m]

- Bản [j] trong game: app 63 fps, present 0,1 ms, **bỏ đúng 31 khung/s** (bất kể độ trễ 1 hay 2) → hình chỉ cập nhật ~32 lần/s
  = giật, dù số fps ghi 63. Gộp lệnh: 2,8 triệu quad → 2,16 triệu Draw (−23 %), 0,3–0,4 µs/lệnh, pass vẽ avg 1 ms = ngang D3D9.
- Harness nhịp 63 fps (REP_FPS=63, cửa sổ hiện hoặc trước): độ trễ 1 → present 0,7 ms bỏ 0; độ trễ 2/3/4 → 0,07 ms bỏ 0; bitblt 0,14 ms.
  Không tái hiện → game vẽ theo TỪNG ĐỢT nhiều khung sát nhau; DO_NOT_WAIT vứt khung thừa của đợt thay vì trải lên các vsync kế.
- **[m] mặc định như D3D9 cửa sổ:** 3 backbuffer (`Rep3Buffers`), hàng đợi 3 khung (`Rep3Latency`), Present CHỜ khi hàng đầy
  (`Rep3NoWait=0`) → không bỏ khung, mọi khung đều lên màn hình, app khoá ở 60 = tần số màn hình (63 → 60 là số trên log, không
  phải trải nghiệm: màn 59,94 Hz không hiện hơn 60). Thời gian trong Present sẽ ghi vài ms = thời gian chờ vsync thay cho ngủ nhịp.
  `Represent3.dll.moi` e900d5cf chờ restart. Muốn không chờ (độ trễ thấp, chấp nhận vứt khung): `Rep3NoWait=1`.

### 6.11 13:05 (bản [m]: 3 buffer, Present CHỜ) — VẪN bỏ 31 khung/s → bản [n] đối tượng chờ + chẩn đoán

Log 13:05: `khong cho 0`, 3 buffer, độ trễ 3 — nhưng `present 0,06 ms` (không chờ) và `bo 31,2/s` y hệt: DXGI trả
`DXGI_ERROR_WAS_STILL_DRAWING` cho `Present(0, 0)` dù KHÔNG có DO_NOT_WAIT. Harness cùng DLL, cùng nhịp 63 fps, cùng cửa sổ 1024×768,
kể cả 2 Present/khung: 0. Game chỉ gọi RepresentEnd một lần/khung (UiShell.cpp:361). Chưa rõ vì sao chỉ game bị.
**[n]:** swapchain có `FRAME_LATENCY_WAITABLE_OBJECT` (`Rep3Waitable=1`): đầu mỗi khung chờ tới khi hàng trình chiếu còn chỗ
→ Present luôn có buffer; chờ ở đây tương đương D3D9 chờ khi hàng đầy. Kèm chẩn đoán: 12 lần đầu Present trả mã khác S_OK ghi
`Present tra 0x..., N ms sau Present truoc, flags`; lúc tạo swapchain ghi style/exstyle/số cửa sổ con/client/flags.
`Represent3.dll.moi` f43f8bfd chờ restart. Bản đang chạy hiện hình cập nhật ~32 lần/s → nên restart sớm.
