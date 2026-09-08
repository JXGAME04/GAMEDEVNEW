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
