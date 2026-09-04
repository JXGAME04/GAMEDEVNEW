# CLIENT VLTK 2.0 VẼ BẰNG GÌ, TỐI ƯU RA SAO — so với dự án JX1

**03/09/2026 — CHỈ MỔ NHỊ PHÂN + ĐỐI CHIẾU NGUỒN. KHÔNG SỬA MỘT DÒNG NÀO. KHÔNG BUILD. KHÔNG SWAP.**

Tiếp nối mục 4 của `BANGIAO_GIATLUI_PHUVE_FPS_0309.md`. Câu hỏi của chủ game: *represent2free.dll của
bản 2.0 vẽ hình ảnh như thế nào, tối ưu như thế nào, vì sao đông người vẫn mượt, không mất NPC, không tụt FPS.*

---

## 1. Mẫu đã mổ

| Tệp (`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\`) | md5 (8 ký tự đầu) | Ghi chú |
|---|---|---|
| `represent2free.dll` 109.008 B | `b6ec659c` | KHÔNG nén, có .reloc, PDB `...\jxvn20\...\Represent2Free.pdb`, VS2005 |
| `represent3free.dll` 952.792 B | `25d1a55b` | KHÔNG nén, D3DX9 tĩnh, PDB `Represent3Free.pdb`, VS2005, dựng 14/08/2026 |
| `gamecl.exe` 1.313.376 B | `343fa18d` | UPX NRV2E, ảnh bung `gamecl_unpacked.bin` (phiên 81d2e6d2), VA = 0x401000 + offset |
| `enginefree.dll` 2.308.560 B | `50bd146c` | 1.535 export: KCanvas/KDirectDraw/KSprite/KSpriteCache/XPackList… |
| `config.ini` | | **`[Client] Represent=3`**, `RepresentNo=1`, `DynamicLight=0`, `FPS=0`, `UseSimpleVersion=0`, 1024×768 |

> 🔴 Bộ lọc `call` của UPX trong ảnh bung: `E8` + byte đánh dấu **0xAE** + 3 byte big-endian; đích thật =
> BE24 + **0x401004** (hiệu chỉnh bằng bỏ phiếu prologue, 3.090/4.169 trúng). Không vá thì mọi `call` đều rác.
> `gdis.py` tự vá và ghi `gamecl_fixed.bin`.

---

## 2. Trả lời ngắn

1. **Bản 2.0 KHÔNG vẽ bằng represent2free.dll.** `config.ini` đặt `Represent=3` ⇒ `gamecl.exe` nạp
   **`Represent3Free.dll` = Direct3D 9** (`0x58EBA0`: `[0x9bb0fc]!=0` → "Represent3Free.dll", ngược lại
   "Represent2Free.dll"). `represent2free.dll` chỉ là dự phòng máy yếu.
2. `represent2free.dll` **cùng thiết kế với `Represent2.dll` mà dự án ĐANG chạy** (`config.ini` dự án
   `Represent=2`; `S3Client.cpp:502-503` đọc khoá, ghi chú "chỉ cho chạy 2d mặc định"): DirectDraw + toàn bộ
   sprite được **blit bằng CPU** trong engine (`KCanvas::DrawSprite / DrawSpriteAlpha / DrawSprite3LevelAlpha /
   DrawBitmap16…` — 52 import từ engineFree). Đây là khác biệt lớn nhất giữa "2.0 mượt" và "dự án tụt FPS".
3. **Dự án có sẵn nguồn Represent3** (`Sources\Represent\Represent3\`, `KRepresentShell3.cpp` 3.801 dòng,
   nằm trong `JXAll.sln`, cấu hình Client Debug/Release → Debug|Win32/Release|Win32, cần
   `d3d9.lib dxguid.lib ddraw.lib GdiPlus.lib`). `represent3free.dll` của 2.0 là **hậu duệ cùng gốc** của
   đúng bộ nguồn này (cùng lớp `KRepresentShell3`, `TextureRes/Bmp/Spr`, `g_D3DShell`, cùng công thức LookAt
   887/1024, cùng VERTEX_BUFFER). Bin chạy thật của dự án **không có `Represent3.dll`** (chỉ có bản Debug
   29/09/2024 ở `E:\SourceTuanLe\SourceVs22\bin\client\debug\`).
4. **Không mất NPC khi đông:** client 2.0 có **800 khe NPC** (`MaxNpcCount = 800`), dự án **256**
   (`KNpc.h:23`). Ghi chú S8 26/08 trong `KProtocolProcess.cpp` đã xác nhận bảng 256 khe đầy trong Tống Kim.
5. 🔴 **ĐÍNH CHÍNH mục 4.4 của báo cáo trước:** gamecl 2.0 **CÓ nội suy vị trí lúc vẽ** (chi tiết mục 5.2).

---

## 3. represent2free.dll — cái gì bên trong

- Xuất duy nhất `CreateRepresentShell` (`0x10006560`), lớp `KRepresentShell2 : iRepresentShell`, font
  `KFontCore2<KFontRenderToolGB/BIG5/VN>` (có bộ vẽ chữ Việt riêng như dự án).
- Toàn bộ pixel do **engineFree.dll** làm trên CPU: `KDirectDraw::Init/Mode/CreateSurface/LockPrimaryBuffer`,
  `KCanvas::LockCanvas/DrawSprite/DrawSpriteAlpha/DrawSprite3LevelAlpha/DrawSpriteScreen/DrawBitmap16(Alpha)/
  DrawLineAlpha/ClearAlpha/BltSurface`, JPEG qua `get_jpg_image`. Không có d3d9.
- Không có gì để "học" từ tệp này: nó là bản DirectDraw 2004 giống `Represent2.dll` của dự án (75 KB).

---

## 4. represent3free.dll — kiến trúc và những chỗ 2.0 đã đi xa hơn nguồn dự án

### 4.1 Khung chung (giống nguồn dự án)
- `Direct3DCreate9`, thiết bị `g_Device.m_pD3DDevice` tại `[0x100de900]`; `RepresentBegin` (vt[24]
  `0x10014C70`): `TestCooperativeLevel` → xử lý `D3DERR_DEVICELOST/DEVICENOTRESET` (Invalidate/Reset/Restore) →
  `Clear(0xFF000000)` → `StartProfile` → dựng ma trận → `BeginScene`. `RepresentEnd` (vt[25] `0x1001D670`):
  `EndProfile` → `EndScene` → `Present`.
- 2 vertex buffer động (`CreateVertexBuffer` ×2 trong RestoreDeviceObjects), FVF XYZRHW|DIFFUSE|TEX1.
- **Mỗi sprite = 1 `SetTexture` + 1 `DrawPrimitive`** (14 điểm gọi `DrawPrimitive`, 11 `SetTexture`) — **không gộp
  lô**, y như nguồn dự án. Không shader (1 `SetPixelShader(NULL)`).
- Ghép nhân vật nhiều phần (≥ 4 primitive) lên texture "pre-render" rồi vẽ 1 lần: **vẫn ghép bằng CPU**
  (`LockRect` → memset → chép từng phần → `UnlockRect` → `DrawPrimitive`, `0x10019590`), giống
  `DrawSprOnTexture2D` của dự án.
- Vẽ lên ảnh (`DrawPrimitivesOnImage`, vt[20]) dùng `SetRenderTarget` — dự án cũng vậy (`:1695`).
- Thiết bị D3D9 được gọi: SetRenderState 53 chỗ, SetSamplerState 44, SetTextureStageState 39, SetTransform 8.

### 4.2 Bảng khác biệt đã xác minh trong nhị phân

| Hạng mục | Nguồn Represent3 của dự án | represent3free.dll 2.0 |
|---|---|---|
| Texture không luỹ thừa 2 (NPOT) | Không; `SplitTexture` luôn cắt khung thành 1/2/4 texture POT, `MAX_TEXTURE_SIZE 1024` | **Có.** Lúc tạo thiết bị (`0x10001700`) đọc `TextureCaps`: bit POW2 tắt → "Full NPOT"; bit NONPOW2CONDITIONAL → "conditional"; rồi **tạo thử texture 33×17 A8R8G8B8** — thất bại thì về POT. Cờ `[0x100eab1a]`. `SplitTexture` (`0x100230E0`): NPOT ⇒ **1 texture đúng kích thước khung** (kẹp ≤ 2048); không NPOT ⇒ cắt 4/2/1 như dự án |
| Tạo texture khung sprite | `CreateTexture` thiết bị, A4R4G4B4, MANAGED | `D3DXCreateTexture` (`0x1002c96b`), **A4R4G4B4, 1 mip, MANAGED** (`0x100237E3`) — cùng định dạng 16 bit |
| Texture pre-render nhân vật | 128 / 256 / 512, **A4R4G4B4** | **128 và 320, A8R8G8B8** (`0x100135D0`) — 32 bit, màu ghép nhân vật không bị rớt 4 bit |
| Ngân sách cache (`TextureResMgr` ctor) | 16 / 32 / 48 MB theo RAM ≤128/≤256/còn lại; `m_nMaxReleaseCount` 16/32/64 | **30 / 50 / 80 / 120 MB** theo RAM ≤128/≤256/≤512/>512 (`0x10024F2E..`); checkpoint **25 ms** (dự án 24) |
| Điều kiện dọn cache (`GetImage`, `0x10025960`) | `TexCacheMemUsed > ngân sách` và quá checkpoint | `SprCacheMemUsed (+0x30) > ngân sách` và quá checkpoint. Nhánh thứ hai `[0x100deaa4] − TexCacheMemUsed < 10 MB` **chết** (biến = 0, không ai ghi) |
| Cách dọn (`CheckBalance`, `0x10024F80`) | Xoá **cả** tài nguyên không dùng > **24 s**, tối đa 16/32/64 cái/lượt | Duyệt từ cuối, bỏ qua tài nguyên **vừa vẽ khung trước** (`m_bLastFrameUsed`), chỉ xét không dùng > **10 s**; **mỗi lượt chỉ bỏ 1 khung texture** (`ReleaseAFrameData`) — không còn khung nào mới xoá cả tài nguyên ⇒ không có cú xoá hàng loạt gây khựng |
| Dọn cuối khung (`RepresentEnd`) | Không | Đo dt (kẹp ≥ 10 ms), fps = 1000/dt, giữ **danh sách 100 khung**, lấy trung bình; **chỉ gọi `CheckBalance` khi fps trung bình ≥ 25** (`0x1001D7CB`) ⇒ máy đang chậm thì không dọn thêm |
| Camera | `LookAt` đặt ma trận VIEW ngay | `LookAt` (vt[22]) chỉ ghi **đích** (`+0x4f74..`); `RepresentBegin` mỗi khung vẽ: `cur += (đích − cur) × 0,85` (`0x10014D04..0x10014D88`, hằng `[0x100bc2e8]`) ⇒ **màn hình cuộn được làm mượt theo khung vẽ**, không nhảy nấc 18 Hz |
| Zoom | `setZoomFactor` | `SetOption(2/3/4)` = đặt / phóng / thu zoom (bảng nhảy 5 mục `0x10013474`) |
| Lọc texture | POINT cố định | POINT mặc định; với một kiểu vẽ đặc biệt tạm bật **LINEAR + MODULATE2X** rồi trả lại (`0x100160D8` / `0x100161C5`) |
| `RepresentIsModuleRecommended` | `Test3D()` | DirectDraw7 `GetCaps` → **`dwVidMemTotal ≥ 32 MB` && RAM vật lý ≥ 128 MB** (`0x10012930`) |
| Kiểm phiên bản | — | gamecl gọi vt[41] (`0x1001CE40`) phải trả **3**, sai thì báo "REPRESENTDLL_VERSION not right" |

Kích thước đối tượng: `sizeof(KRepresentShell3)` 2.0 = 0x4FD8; vtable 42 mục tại `0x100bc464`
(dự án 40 mục — 2.0 khác thứ tự ở đuôi, Create vẫn là [1], DrawPrimitives [19], RepresentBegin [24],
RepresentEnd [25], LookAt [22], SetOption [29]).

---

## 5. gamecl.exe — phần lõi liên quan tới "đông người"

### 5.1 Giới hạn bảng (printf `0x5CB0FE..0x5CB138`, hằng đẩy thẳng)

| | 2.0 client | Dự án client |
|---|---|---|
| MaxSubWorldCount | 1 | 1 |
| MaxPlayerCount | 2 | — |
| **MaxNpcCount** | **800** (`cmp edi,0x320` ở `0x676057`; `sizeof(KNpc)=0x12BD4`, mảng `0x1AB34F4`) | **256** (`KNpc.h:23`) |
| MaxItemCount / MaxObjCount | 512 / 256 | 512 / — |
| MaxSkillCount / Level | 3000 / 64 | — |
| **MaxMissileCount** | **1000** | **500** (`KMissle.h:10`) |
| MaxPlayerLevel / NpcLevel | 200 / 180 | — |

⇒ "đông không mất NPC" trước hết là **800 khe so với 256**. Khi bảng dự án đầy, `AddNpcSet2` trả 0 và gói sync
bị bỏ (`KProtocolProcess.cpp` ~2068-2085 có ghi chú S8 26/08 mô tả đúng ca này).

### 5.2 🔴 Nội suy vị trí lúc VẼ — có, nằm trong cây cảnh (đính chính mục 4.4 báo cáo trước)

Nút cây cảnh (scene tree) của 2.0: `+0` loại, `+4` anh em, `+8` con, `+0x10/+0x14` x,y hiện tại, `+0x18` kiểu,
`+0x1c` chỉ số NPC, `+0x24` cờ, `+0x28/+0x2a` nửa rộng/cao (word), **`+0x38/+0x3c` x,y TRƯỚC, `+0x40` mốc tick**.

- **Ghi** (`0x672F09`, trong critical section `[0x7840fc]/[0x784100]`): `prev = cur; mốc = [0x9bb0ec]; cur = mới`.
  `[0x9bb0ec]` được ghi **mỗi tick logic** tại `0x58FB8C` = `m_GameCounter × 1000 / 18` (mili-giây của tick).
- **Đọc** khi duyệt vẽ (`0x675F80`, đệ quy con → anh em): với nút loại 1:
  `t = clamp((KTimer::GetElapse() − mốc) × 18 / 1000, 0, 1)` (`0x675FC4..0x676022`, chia cho hằng 1000,0 tại
  `0x7b0de8`), rồi `x = prevX + (curX − prevX) × t`, `y = prevY + (curY − prevY) × t` (`0x676024..0x67604F`),
  cắt theo hình chữ nhật màn hình, gọi vẽ `0x670130(kiểu, idxNpc, x, y, 0, 0, 2, 0)`.
- Chuỗi đã nối: `0x672FF0` (gốc vẽ cảnh) → `0x715040` → `0x72B700` → `0x675F80`. Gốc `0x672FF0` được gọi từ thunk
  `0x5B9020` không có người gọi tĩnh (đi qua con trỏ hàm/vtable từ nhánh vẽ `0x58FC9E`/`0x58FCB7`) — mắt xích
  cuối này là suy luận, ba mắt xích trên là mã đọc được.

⇒ Ở `PaintFps=60`, nhân vật/NPC của 2.0 **trượt mượt giữa hai tick 18 Hz** chứ không nhảy nấc. Báo cáo trước
chỉ xét ~8 lệnh `call` trực tiếp của nhánh vẽ nên bỏ sót. Phần nội suy `POSSHIFT` của dự án (15/08) là tương
đương về ý tưởng, **không còn là "vượt bản gốc"**.

### 5.3 Tuỳ chọn chống đông trong `[Options]` (đọc `0x48BDDE..`, áp dụng `0x48B6F0..0x48B870` → `OperationRequest(43, …)`)

| Khoá | Mặc định | Ý nghĩa thật (đọc từ nơi dùng) |
|---|---|---|
| `VisibleNum` | 1 | Chỉ có 1 nơi đọc (`0x676092`): **≤ 0 ⇒ bỏ vẽ toàn bộ người chơi khác** (nút kiểu 6, `Npc.+0x2c ∈ {1,2}`, không phải mình). Không phải "số người tối đa" |
| `NpcTheSame` | 0 | Bật ⇒ cờ `[0x1ab34f8]`=1 và tên bộ đồ (mặc định **"ani005"**, `0x81ad00`): mọi người chơi khác gốc "MainMan" bị **thay bằng đúng một bộ ngoại hình** (`0x5EC268`), **bỏ nạp sprite trang bị/vũ khí** của họ (`0x5F3817`, `0x5F3954`) ⇒ cả đám đông dùng chung vài sprite/texture |
| `MissleOpen` + `MissleIndex` | 0 / — | Bật ⇒ `[0x2212bfc]`=1, `[0x2212c00]`=MissleIndex (1..999): khi tạo đạn, **4 bộ tên sprite của MỌI chiêu bị thay bằng mẫu số MissleIndex** (`0x6B15D9..0x6B168D`, bảng bước 0xB94) ⇒ một hiệu ứng cho tất cả kỹ năng |
| `QualityOpen` | 1 | Represent3: `SetOption(6, …)` — **ngoài bảng 0..4 ⇒ không tác dụng**; chỉ Represent2 nhận `SetOption(5, …)` |
| `PaintFPS` | 100 (thanh trượt) | song song với `[Client] PaintFps` mặc định 60 (mục 4.3 báo cáo trước) |

### 5.4 Chọn bộ vẽ
- `[Client] Represent` mặc định **3** (`0x59213A`). Nếu `[Main] RepresentNo ≠ [Client] RepresentNo` ⇒ **ép về 3**
  và ghi lại (`0x592100`) — cơ chế "đổi số phiên bản để ép mọi máy sang D3D".
- `[Client] UseSimpleVersion` (mặc định 1, `0x5929CA`) — bản rút gọn cho máy yếu, chưa mổ tiếp.

---

## 6. Vì sao 2.0 "đông vẫn mượt" — xếp theo sức nặng, đối chiếu dự án đang chạy

1. **GPU thay CPU.** Dự án đang chạy Represent2: mỗi pixel sprite đi qua vòng blit CPU đơn luồng; đông người
   ⇒ thời gian vẽ tăng tuyến tính theo số sprite × diện tích. 2.0 chạy Represent3: CPU chỉ đẩy 4 đỉnh/sprite,
   GPU tô. Dự án **đã có nguồn**, chỉ chưa build/bật.
2. **800 khe NPC + 1000 đạn** so với 256 + 500 ⇒ không rơi NPC/đạn khi Tống Kim, công thành.
3. **Nội suy vị trí lúc vẽ** (5.2) + **camera lerp 0,85/khung** (4.2) ⇒ 60 khung/giây nhìn thật sự mượt.
   Dự án có nội suy NPC/đạn (POSSHIFT) nhưng camera vẫn nhảy theo tick.
4. **Cache texture lớn và dọn "êm"**: 30-120 MB, chỉ bỏ 1 khung texture/lượt, không đụng tài nguyên vừa vẽ,
   không dọn khi fps < 25. Nguồn dự án: 16-48 MB, xoá cả tài nguyên, tới 64 cái/lượt ⇒ dễ khựng lúc đông.
5. **NPOT**: 1 texture đúng cỡ/khung thay vì 1-4 texture POT ⇒ ít `DrawPrimitive`, ít VRAM lãng phí, ít lần tạo texture.
6. **Tuỳ chọn hy sinh chi tiết**: NpcTheSame (một bộ đồ cho cả đám), MissleIndex (một hiệu ứng cho mọi chiêu),
   VisibleNum=0 (ẩn người khác). Người chơi 2.0 có thể bật khi công thành.
7. Pre-render nhân vật 32 bit (đẹp hơn, không tác động tốc độ).

Những thứ 2.0 **không** làm (đừng đi tìm): gộp lô sprite, shader, đa luồng vẽ (chỉ thấy critical section
quanh cây cảnh), nén texture.

---

## 7. Việc có thể làm — CHƯA LÀM, chờ chủ quyết

Mọi mục dưới đây đụng tới client đang chạy nên **hỏi trước** (luật 03/09):

1. **Build `Represent3.dll` (Client Release|x86)** từ nguồn có sẵn, thả vào `bin\client`, đặt `Represent=3`
   thử trên một máy. Rủi ro: nguồn dự án là bản 2004 (POT split, cache 16-48 MB, 4444 pre-render, không
   camera lerp) — chạy được nhưng chưa có các cải tiến ở mục 4.2; cần đo lại `jx_paint.log`.
2. Nếu bật Represent3 thì port dần theo thứ tự: (a) ngân sách cache + dọn 1 khung/lượt + TTL 10 s + cổng fps ≥ 25;
   (b) NPOT (dò caps + tạo thử 33×17) và `SplitTexture` 1 texture; (c) camera lerp trong `RepresentBegin`;
   (d) pre-render A8R8G8B8 128/320.
3. `MAX_NPC` client 256 → 800: đổi hằng ở `KNpc.h:23` kéo theo `KNpcSet` (m_FreeIdx/m_UseIdx), bộ nhớ
   `Npc[]` (~0x116E0 × 800 ≈ 57 MB), các mảng phụ index theo MAX_NPC, và phải rebuild toàn bộ client
   (Core + S3Client + WAuto dùng chung header). Cần rà `MAX_NPC` trong 30+ tệp trước khi đụng.
4. Tuỳ chọn kiểu `NpcTheSame`/`MissleIndex` cho WAuto/Game.exe: là thay đổi luật hiển thị — hỏi chủ.

---

## 8. Công cụ để lại (scratchpad phiên `17c6a10c`)

| tệp | dùng để |
|---|---|
| `r3dis.py <dll> str/xref/dis/func/calls/vtable/funcstart/imports` | mổ DLL không nén (pefile + capstone), chú thích IAT/chuỗi |
| `gdis.py dis/func/calls/xref/str/funcstart/calib` | mổ ảnh gamecl đã bung, **tự vá bộ lọc call UPX** → `gamecl_fixed.bin` |
| `d3dscan.py` | đếm điểm gọi phương thức IDirect3DDevice9, đọc hằng float/double |
| `TextureRes_cpp.txt` | bản dump `TextureRes.cpp` của dự án (latin-1) để đối chiếu |

Bẫy: `pefile` phải `pip install`; Bash heredoc phá `\x50` trong regex ⇒ viết script bằng Write; bảng offset
vtable IDirect3DDevice9 dễ lệch (0x10 GetAvailableTextureMem, 0x40 Reset, 0x44 Present, 0x50 SetDialogBoxMode,
0x54 SetGammaRamp, 0x5C CreateTexture, 0x94 SetRenderTarget, 0x98 GetRenderTarget, 0xA4/0xA8 Begin/EndScene,
0xAC Clear, 0xB0 SetTransform, 0xE4 SetRenderState, 0x104 SetTexture, 0x10C SetTextureStageState,
0x114 SetSamplerState, 0x144 DrawPrimitive, 0x164 SetFVF, 0x190 SetStreamSource).


---

## 9. Bổ sung 03/09 (chiều) — "Represent2 đưa qua GPU được không? Làm theo 2.0 có hơn không?"

### 9.1 Represent2 không có đường tắt lên GPU
- Represent2 = DirectDraw + vòng lặp pixel trong `KCanvas::DrawSprite*` (engine). DirectDraw không có blit alpha phần
  cứng; từ Windows 8 DirectDraw là lớp giả lập trên D3D, `Blt` chỉ chép chữ nhật. Muốn GPU tô alpha phải thay
  toàn bộ tầng vẽ sau giao diện `iRepresentShell` — đó chính là Represent3 (Core không biết bên dưới là 2 hay 3).
- Ghép "vẽ CPU rồi đẩy cả khung lên GPU" không lợi gì: DWM đã làm đúng việc đó, blit vẫn ở CPU.
- Represent3 của dự án ĐÃ build được: cây E: có `Represent3\Debug\Represent3.dll.recipe` 21/10/2025 và mốc
  `Release\Represen.86D33E62.Up2Date` 06/08/2025; nguồn D: chỉ hơn E: 3 dòng stub replay (`d4120687`).

### 9.2 Số đo từ chính client đang chạy Represent2 (`bin\client\jx_paint.log`, 153 MB, ~1,04 triệu giây)
| Số đo | Giá trị |
|---|---|
| Khung vẽ/giây bình thường ở 300-360 sprite/khung | ~62 (giữ trần 60) — máy này KHÔNG tụt đều, chỉ giật cục |
| Giây bất thường `[SEC]` (painted < 55 hoặc tick nặng) | 29.453 giây = **2,8 %** tổng |
| Trong giây bất thường: paint ms/khung p50 / p90 / p99 | **9,9 / 20,9 / 29,0 ms** |
| Giây < 50 fps (0,5 % tổng) | paint p50 **21 ms/khung** + logic **45 ms/giây** |
| `[SPIKE]` (một lượt ≥ 25 ms), 132.270 cú | **71 % do paint** (94.096) · 29 % do logic (38.172) · shift ≈ 0 |
| paint trong cú giật p50 / p90 / p99 | 24 / 29 / 60 ms |
| logic trong cú giật p50 / p90 | 3 / 66 ms (nạp map, đồng bộ dồn) |
| Kho ảnh `kho=` | 1003/2048, dọn chỉ ảnh nghỉ > 100 s ⇒ KHÔNG còn thrash cache |
| `lock=` | chỉ đo `IDirectDrawSurface::Lock/Unlock`, KHÔNG gồm vòng blit ⇒ không tách được blit khỏi phần còn lại |

Ước lượng blit: 350 sprite × ~4-5 nghìn pixel = 1,5-2 Mpx trộn alpha 16-bit mỗi khung ⇒ cỡ 5-15 ms CPU, trùng
với paint/khung đo được trong giây xấu ⇒ blit là phần lớn của "paint".

### 9.3 Kết luận
- Bật Represent3 (GPU) đánh đúng vào **71 % số cú giật và toàn bộ phần paint 10-21 ms/khung khi đông**; kỳ vọng
  paint/khung còn vài ms, số cú giật giảm cỡ 2/3, máy yếu lợi nhiều nhất. **Không** giải quyết 29 % cú giật do
  logic (tick 45 ms/giây khi đông, nạp map) và **không** giải quyết "mất NPC" (bảng 256 khe).
- "Làm theo 2.0" đầy đủ = Represent3 + MAX_NPC 800 + NpcTheSame/MissleIndex/VisibleNum + cache/NPOT/camera lerp.
  Hai mục sau đóng góp cho "đông vẫn mượt" không kém tầng vẽ.

### 9.4 Lỗ hổng tương thích phải kiểm trước khi bật Represent3 (đọc từ nguồn)
1. `SetAdjustColorList` trong R3 là **stub** (`return uCount`); Core gọi ở `CoreShell.cpp:21979` để nạp bảng biến sắc
   ⇒ NPC/vật phẩm "đổi màu theo palette" sẽ hiện màu gốc. R3 chỉ có tô màu đỉnh cho `ALPHA_COLOR_ADJUST`.
2. Sprite mới `Reserved[1]` (cộng màu, "ngựa siêu quang"): R3 chỉ xử lý `m_bNew` ở đường ghép nhân vật và 3D
   (`:1148/1371/1487`), KHÔNG có trong `DrawSpriteAlpha` (vẽ đơn) ⇒ hiệu ứng đơn lẻ có thể mất cộng màu.
   R2 xử lý qua `DrawSpriteBlendColor` ở 8 nhánh.
3. `GetBitmapDataBuffer` R3 có (`:1817`), dùng bởi `cLittleMap`, `ScenePlaceMapC` (bản đồ nhỏ), `UiPlayVideo` — phải
   kiểm bản đồ nhỏ và video.
4. Chữ: `KFontCore3` vẽ chữ qua texture, có `OutputVNText`; cần soi viền/độ đậm chữ Việt.
5. Cửa sổ: hack `SCREEN_HEIGHT + 40` và co toạ độ chuột (`S3Client.cpp:338-357, 1612`) chỉ chạy khi Represent3.
6. Replay `.jxr` chỉ ghi ở 2D (R3 chỉ lưu con trỏ).
7. Nguồn R3 dự án là bản 2004: cắt POT 1/2/4, cache 16/32/48 MB xoá hàng loạt, pre-render 4444, không camera lerp.

### 9.5 Thứ tự đề nghị (chờ chủ quyết, chưa làm gì)
1. Build `Represent3` Client Release|x86, chép vào `bin\client` của MỘT máy thử, `Represent=3`, giữ `PaintLog=1`
   để so `[SEC]/[PDET]` trước-sau; ghi hình các ca ở 9.4.
2. Vá 2 lỗ 9.4.1-9.4.2 (bảng biến sắc bằng cách tạo texture theo palette đã đổi, hoặc tô màu đỉnh; cộng màu
   `m_bNew` trong `DrawSpriteAlpha`).
3. Port từ 2.0 theo 4.2: cache (ngân sách theo RAM, bỏ 1 khung/lượt, TTL 10 s, cổng fps), NPOT, camera lerp 0,85,
   pre-render 8888.
4. `MAX_NPC` 256 → 800 (rebuild toàn bộ client) và tuỳ chọn `NpcTheSame`/`MissleIndex` cho WAuto/Game.exe.


---

## 10. THI CÔNG 03/09 (chiều tối) — đã làm cả 4 việc, chờ chủ swap

Nhánh git **`rep3-0309`** (đã push origin; worktree `D:\GAMEDEVNEW_wt_rep3`; gốc main `d31dfde1`, sau đó merge main `c13af6ee` = MAIL đợt 3 + S13i/j `68694127`).
Không sửa một tệp nào trong cây `D:\GAMEDEVNEW` đang có việc dở của phiên khác.

### 10.1 Represent3 vẽ PHẲNG như Represent2, chạy trên GPU (`Sources\Represent\Represent3`, commit `12c232a7`)
| Việc | Cách làm |
|---|---|
| Tắt góc camera 3D + ánh sáng | `g_renderModel = RenderModel2D` (cờ `[Client] Rep3Flat=1` mặc định): mọi sprite vẽ bằng toạ độ màn hình XYZRHW, `CoordinateTransform`/`X` dùng đúng công thức Represent2 (`x−m_nLeft`, `y/2−m_nTop−z·887/1024`), không ma trận view/projection, `SetOption(PERSPECTIVE)` bị bỏ qua |
| Không "cháy" hiệu ứng | BORDER (chọn mục tiêu) → **không vẽ** như Represent2 (trước: MODULATE2X làm sáng); OPACITY → đục hoàn toàn bằng alpha-test (trước: alpha texture); ALPHA/3LEVEL/NOT_BE_LIT → alpha texture × `Color.a` (trước: bỏ `Color.a`); COLOR_ADJUST → nhân màu (đúng `g_BlendColor32b` mode 0); sprite mới `Reserved[1]` → SCREEN `d + a·s·(1−d)` bằng 2 tầng texture (INVDESTCOLOR/ONE) đúng `g_DrawSpriteScreen32b` |
| Vẽ từng phần như Represent2 | `DrawImage2DFlat`: không ghép nhân vật lên texture pre-render (bỏ luôn LockRect/memset/RIO_Copy mỗi khung), RU_T_IMAGE_4 cắt theo `oImgLTPos/oImgRBPos` như `SetClipRect`; `Rep3Composite=1` để bật lại cách cũ |
| Màu đúng palette | Texture sprite **A8R8G8B8** (`Rep3Tex32=1`), giải RLE bằng `RenderToA8R8G8B8`; `Rep3Tex32=0` = 4444 như 2.0 |
| Port 2.0 | NPOT (dò caps + tạo thử 33×17, `SplitTexture` 1 texture đúng cỡ ≤ MaxTextureWidth); cache theo RAM (RAM/16, kẹp 30..480 MB, `Rep3CacheMB` ghi đè), mỗi lượt bỏ **1 khung** texture nghỉ > 10 s, bỏ qua tài nguyên vừa vẽ, dọn cuối khung khi fps TB ≥ 25; `Present` ngay không chờ vsync (`Rep3Vsync=1` để bật); `Lock` VB `D3DLOCK_DISCARD` (trước lock 0 = chờ GPU mỗi sprite) |
| Cửa sổ | Represent3 tự đặt client = nWidth×nHeight, Present 1:1; nếu gặp Game.exe cũ (+40 px) thì giữ để chuột khớp |
| Nhật ký | `bin\client\jx_rep3.log` (`Rep3Log=1`): card, NPOT, ngân sách cache |

**Đo bằng bộ thử độc lập** (`ReverseTools\represent3\rep_harness.cpp`, chạy trên thư mục junction sang `bin\client`, không ghi gì vào cây sống):
- Cùng 224 sprite/khung: **Represent2 3,1–5,0 ms/khung · Represent3 mới 0,85 ms/khung** (≈ 4–6×).
- Ảnh chụp cùng bố cục (7 kiểu vẽ × 8 sprite, sprite mới SCREEN, cắt khung, đường/khung/bóng, chữ GBK + TCVN3, LookAt):
  toàn ảnh **0,07 % điểm lệch ≥ 32/255**, các hàng sprite **0,00 %**, chữ 1,8 % (cùng font bitmap `gbk_fs10.fnt`, khác viền nhẹ), sprite SCREEN 0,07 %.
  Ảnh so: `scratchpad\rep_compare.png` (R2 | R3 | khác biệt ×4).
- Card máy chủ game: HAL, MaxTex 16384, **Full NPOT**, RAM 32 GB ⇒ ngân sách cache 480 MB.

### 10.2 Game.exe (`S3Client.cpp`)
- Bỏ hack kéo cửa sổ cao thêm 40 px và co toạ độ chuột `y·40/808`.
- **Tự lùi về Represent2**: thiếu `Represent3.dll` hoặc `Create()` thất bại ⇒ giải phóng, `g_bRepresent3=false`, khởi tạo lại Represent2, ghi `[REP3]` vào `jx_auto.log`. Không còn thoát game vì lỗi D3D.

### 10.3 MAX_NPC client 256 → 800 (`KNpc.h`), MAX_NPC_REQUEST 20 → 128 (`KNpcSet.h`)
- Rebuild sạch CoreClient (`-t:Rebuild`, header dùng chung). `CoreClient.dll` SizeOfImage 82 → 105 MB (mảng `Npc[]` tĩnh, ≈ 42 KB/khe).
- WAuto (`E:\Src_Auto_Ngoai`) chỉ nhận `nNpcIdx` của chính nhân vật qua IPC, không có mảng theo MAX_NPC ⇒ không cần build lại.
- Server không đổi (MAX_NPC server 98000 giữ nguyên).

### 10.4 Tuỳ chọn chống đông (đọc `[Client]` trong `config.ini`, chỉ client)
| Khoá | Mặc định | Tác dụng |
|---|---|---|
| `NpcTheSame=1` | 0 | người chơi KHÁC (không phải mình) mặc chung: `NpcTheSameArmor` (0), `NpcTheSameHelm` (0), bỏ phi phong; `NpcTheSameHorse`/`NpcTheSameWeapon` ≥ 0 mới ép (mặc định −1 giữ riêng). Áp ở `SyncPlayer` + `SyncPlayerMin` |
| `MissleIndex=N` | 0 | mọi đạn kỹ năng lấy `AnimFile/SndFile/AnimFileInfo` của dòng N trong missles (giữ tham số bay/va chạm của đạn gốc; dòng N phải có `AnimFile1`) |

### 10.5 Tệp giao — trạng thái thật 03/09 tối
Phiên này bị chặn lệnh ghi `.moi` vào `bin\client` (chỉ chép được `Represent3.dll`, tệp mới). Phiên wauto-d9 **không gộp rep3 vào bộ của họ** (tránh lách quyền): bộ `.moi` họ đặt lúc 17:36 (chủ ĐÃ chạy ChoiGame.bat ~17:45, không còn .moi) build từ origin/main `cef918af` (MAIL 3+4+4b + S13i/j), **không có Represent3, MAX_NPC client vẫn 256**.

| Tệp đang nằm trong `bin\client` | md5 | Nguồn | Ghi chú |
|---|---|---|---|
| `Represent3.dll` | `74ac07ad` | rep3-0309 | ĐÃ ĐẶT, game đang nạp (chủ bật `Represent=3` từ 17:40) — bản này texture MANAGED nên **RAM gấp đôi**, thay bằng 94a33924 bên dưới |
| `CoreClient.dll` (ĐÃ SWAP ~17:45, `.truoc` = bản 16:42) | `6bbcda8f` (2.508.800) | wauto-d9, main `cef918af` | không rep3: MAX_NPC 256, không `NpcTheSame`/`MissleIndex` |
| `Game.exe` (ĐÃ SWAP ~17:45, `.truoc` = bản 16:38) | `bd5cb88e` (1.401.856) | wauto-d9, main `cef918af` | không rep3: còn hack +40 px, chưa tự lùi Represent2 |
| `CoreClient.dll.moi` | `e151cbfc` (2.515.456) | wauto-d9, main `87757f14` (D5) | đặt 17:45, CHỜ ChoiGame.bat; không rep3 |
| `CoreServer.dll` | `bad8e293` | wauto-6a | không đụng |

**Bộ rep3 = tập cha** (nhánh `rep3-0309` `b231f5a2` = main `91302e35` gồm MAIL 3→7 + S13k + Tống Kim wauto-c0 + rep3 + vá RAM), build sạch `-t:Rebuild`, **CHƯA đặt** — chủ tự chép (hoặc cấp quyền ghi cho phiên):

| Tệp | md5 | Chép từ → tới |
|---|---|---|
| `CoreClient.dll` | `5b999107` (2.521.088) | `D:\GAMEDEVNEW_wt_rep3\Sources\Core\ClientRelease\CoreClient.dll` → `bin\client\CoreClient.dll.moi` |
| `Game.exe` | `21c4a10b` (1.401.344) | `D:\GAMEDEVNEW_wt_rep3\Sources\S3Client\Release\Game.exe` → `bin\client\Game.exe.moi` |
| `Represent3.dll` | `94a33924` (123.904) | `D:\GAMEDEVNEW_wt_rep3\Sources\Represent\Represent3\Release\Represent3.dll` → `bin\client\Represent3.dll` (chép ĐÈ khi game ĐÃ THOÁT; ChoiGame.bat không xử lý `.moi` cho tệp này) |

Thứ tự: (1) bộ thư của wauto-d9 đã swap ~17:45 và đang chạy (`.truoc` = f2ad5ca3/24762253); họ còn đặt thêm `CoreClient.dll.moi` e151cbfc (D5, gốc lỗi hộp thư) chờ ChoiGame.bat — có thể chạy nó trước để thử thư, hoặc chép đè luôn bộ rep3 (đã gồm D5); (2) thoát game, chép 2 tệp trên thành `.moi` (lệnh dưới), đặt `Represent=3` trong `[Client]` của `config.ini`, thoát game, chạy `ChoiGame.bat` lần nữa; (3) xem `bin\client\jx_rep3.log` (card, NPOT, ngân sách cache) và `jx_auto.log` dòng `[REP3]` nếu bị lùi về Represent2.
```
copy /Y D:\GAMEDEVNEW_wt_rep3\Sources\Core\ClientRelease\CoreClient.dll E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\CoreClient.dll.moi
copy /Y D:\GAMEDEVNEW_wt_rep3\Sources\S3Client\Release\Game.exe       E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Game.exe.moi
copy /Y D:\GAMEDEVNEW_wt_rep3\Sources\Represent\Represent3\Release\Represent3.dll E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Represent3.dll
```

### 10.6 Quay lui
- Lỗi Represent3 ⇒ Game.exe mới tự dùng Represent2; muốn tắt hẳn: `Represent=2` trong `config.ini`. `Represent2.dll` không đổi.
- Lỗi khác ⇒ đổi lại `CoreClient.dll.truoc` / `Game.exe.truoc` (ChoiGame.bat giữ bản cũ).

### 10.7 Chưa làm / cần chủ quyết
- Chưa có giao diện bật `NpcTheSame`/`MissleIndex` trong WAuto (chỉ config.ini).
- Camera lerp 0,85 của 2.0 không port: chế độ phẳng dùng nội suy POSSHIFT sẵn có của dự án.
- Nhánh `rep3-0309` (`b231f5a2`, đã push) = main `91302e35` (MAIL 3→7 + S13i/j/k + Tống Kim wauto-c0) + rep3 + vá RAM/LOAD, CHƯA merge vào `main` (chờ chủ quyết sau khi thử). Ai build client sau phải lấy từ `rep3-0309` (hoặc gộp nó vào main), nếu không sẽ mất Represent3 phẳng + MAX_NPC 800 + 2 tuỳ chọn.

### 10.8 RAM Represent3 gấp đôi Represent2 (chủ báo 03/09 ~17:55) — nguyên nhân, số đo, vá

**Số đo trong game** (Game.exe PID 40032, Represent3 74ac07ad, RTX 3080, so với GPU counter của tiến trình):

| Giờ | RAM riêng tiến trình | VRAM tiến trình (dedicated) |
|---|---|---|
| 17:54 | 513 MB | 237 MB |
| 17:57 | 611 MB | 329 MB |
| 17:59 | 457 MB | 230 MB |
| 18:1x | 1.340 MB (đông người) | — |

RAM tăng/giảm **1:1 theo VRAM** ⇒ mỗi texture tồn tại 2 bản: VRAM + bản sao trong RAM tiến trình. Gốc: sprite tạo ở `D3DPOOL_MANAGED` (D3D9 luôn giữ bản chủ trong RAM), texture 8888 = 4 B/px, ngân sách 480 MB. Represent2 chỉ giữ dữ liệu RLE 8 bit (≈ 1 B/px) trong cache engine.

**Đo bằng bộ thử** (300 sprite × 40 khung, cùng danh sách, chạy ngoài màn hình; `ReverseTools\represent3\rep_harness.exe`, thư mục `run\` junction sang `bin\client`):

| Bản | RAM riêng sau khi vẽ | Ghi chú |
|---|---|---|
| Represent2 | 97 MB | cache sprite của engine |
| Represent3 74ac07ad (MANAGED) | 105–107 MB | = R2 + bản sao texture |
| Represent3 mới, `Rep3Pool=0` (MANAGED) | 107 MB | đối chứng |
| **Represent3 mới, `Rep3Pool=1` (DEFAULT)** | **68 MB** | thấp hơn Represent2 29 MB |

**Vá (Represent3.dll 94a33924, commit `06649ed6` + `2a5df16b`, marker `[REP3 03/09 RAM]` / `[REP3 03/09 LOAD]`):**
1. Texture sprite → `D3DPOOL_DEFAULT` (chỉ VRAM): đổ qua texture tạm SYSTEMMEM + `UpdateTexture`; mất device → `TextureResSpr::InvalidateDeviceObjects` bỏ hết khung, vẽ lại thì tạo lại. `Rep3Pool=0` quay về MANAGED.
2. Hit-test chuột (`GetPixelAlpha`) đọc từ RLE gốc thay vì `LockRect` texture (DEFAULT không lock được; cách cũ còn ép đồng bộ GPU). Bộ thử: kết quả y hệt bản cũ (15 điểm / tổng 3825).
3. Ngân sách cache = VRAM: RAM/16 kẹp 60..384 MB (máy chủ 384), đọc `Rep3CacheMB` SAU khi đọc ini (trước đây đọc trong ctor nên khoá này không tác dụng); vượt ngân sách → bỏ 8 khung/lượt, nghỉ > 1 s.
4. Thống kê mỗi 30 s vào `jx_rep3.log` (`Rep3StatSec`, 0 = tắt): `RAM rieng, WS | VRAM con | cache N muc: texture MB (ve khung nay MB, ngan sach MB), raw spr MB | nap, bo | fps TB`.
5. Không cache mục NULL khi nạp ảnh thất bại (mã gốc chèn node NULL, không bao giờ nạp lại → ảnh "trong suốt" cả phiên); thử nạp lại + ghi `[REP3] LoadImage FAIL type=… : <tên>` (tối đa 200 dòng). Vụ nền hộp thư 625×447 trong suốt 17:50 của wauto-d9 hoá ra do ini thiếu `\` đầu đường dẫn (đã sửa 18:05); bộ thử vẽ sprite đó bằng cả 2 bản Represent3 khớp Represent2 0,00 %.

Kỳ vọng trong game: RAM ≈ Represent2 (mất phần bản sao ≈ VRAM đang dùng, 200–400 MB tuỳ cảnh). Kiểm bằng dòng `RAM rieng` trong `jx_rep3.log` sau khi thay DLL.

### 10.9 Giao diện WAuto cho `NpcTheSame` / `MissleIndex` (03/09 tối, commit `04e1d93e`; nhánh `20bfcdf5` = main `b6c42d57` gồm MAIL D8) — hoàn tất việc dở ở 10.7

Đọc kèm `BANGIAO_REP3_SAP_VA_RAM_0309.md` (phiên khác đã vá sập UCL / giật / RAM, `Represent3.dll` 113eeb1d đang chạy; đính chính 10.8: số đo bộ thử ở 60 sprite quá nhỏ, POOL_DEFAULT vẫn ăn ~0,45–0,66 MB RAM mỗi MB texture, 2.0 dùng 8888).

**Chỗ đặt:** tab **Cơ bản**, dưới khung "Tự động đăng nhập" (thêm kẻ ngang + 2 hàng, ID 628..631, tab cao 330 → 362):
- ☐ `Người chơi khác mặc chung 1 bộ (đông người nhẹ máy)` → `autoData.bWANpcTheSame`
- ☐ `Mọi chiêu dùng chung 1 hiệu ứng, dòng:` [__] → `autoData.bWAMissle` + `nWAMissleIndex` (mặc định 1)
Lưu như mọi ô khác (`SaveRoleData` → `APdata\<ID>.dat`, kích hoạt qua BN_CLICKED / EN_KILLFOCUS); tệp `.dat` cũ ngắn hơn vẫn đọc được (memset 0 + đọc phần có) → mặc định tắt.

**Đường đi:** WAuto gửi gói mới `IPCHienThi` (`PRT_HIENTHI`, cuối enum) **mỗi lượt GAMELOOPINTV cho mọi game đang kết nối, KHÔNG cần tick auto** → `S3Client.cpp` case `PRT_HIENTHI` → `CoreShell` `ATYPE_HIENTHI` ghi `g_nWAOptNpcTheSame`/`g_nWAOptMissleIndex` + mốc giờ → `REP3_NpcTheSame` (KProtocolProcess.cpp) và `REP3_MissleIndex` (KMissle.cpp) dùng giá trị WAuto khi còn mới (< 5 s); tắt WAuto thì 5 s sau về `[Client]` trong `config.ini` như cũ.

**Hành vi:** `NpcTheSame` áp cho người chơi **vừa xuất hiện** (SyncPlayer/SyncPlayerMin); bật/tắt thì người đang thấy giữ nguyên tới khi xuất hiện lại; chính mình không đổi. `MissleIndex` áp ngay chiêu kế tiếp (đọc mỗi lần bắn); dòng N phải có `AnimFile1`, không thì giữ đạn gốc.

**Struct dùng chung đổi:** `autoData` +3 int ở CUỐI (trước constructor), `PROTTOOLID` +`PRT_HIENTHI`, `IPCHienThi` mới — `Sources\Core\Src\ipc_shared.h` và `E:\Src_Auto_Ngoai\WAuto\WAuto\ipc_shared.h` **giống nhau**; `CoreShell.h` +`ATYPE_HIENTHI` cuối enum. WAuto cũ + client mới: client kiểm `Size` nên không đọc rác; WAuto mới + client cũ: gói `PRT_HIENTHI` bị bỏ qua. Cây WAuto E: không có git — bản lưu `*.cu_0309_truoc_hienthi`; bộ vá `ReverseTools\represent3\rep3_wauto_hienthi.py` áp lại được cho cả hai cây.

**Kiểm:** hộp thoại biên dịch trong `WAuto.exe` đã đọc lại bằng pefile: 4 control mới đúng ID/vị trí (628 kẻ 334, 629 checkbox 338, 630 checkbox 350, 631 ô số 350). **Chưa bấm thử trong game** (WAuto của chủ đang chạy, không mở bản thứ hai chồng lên).

**Bộ tệp (tập cha của mọi thứ trên `rep3-0309` `20bfcdf5` = main `b6c42d57` + rep3 + vá sập/RAM của phiên kia + hiển thị):**

| Tệp | md5 | Kích thước | Chép từ → tới |
|---|---|---|---|
| `CoreClient.dll` | `3d706aa2` | 2.530.304 | `D:\GAMEDEVNEW_wt_rep3\Sources\Core\ClientRelease\CoreClient.dll` → `bin\client\CoreClient.dll.moi` |
| `Game.exe` | `714e71d3` | 1.401.344 | `D:\GAMEDEVNEW_wt_rep3\Sources\S3Client\Release\Game.exe` → `bin\client\Game.exe.moi` |
| `WAuto.exe` | `316f7abd` | 415.744 | `E:\Src_Auto_Ngoai\WAuto\WAuto\Release\WAuto.exe` → thoát WAuto, đổi `WAuto.exe` cũ → `.truoc`, chép bản mới (ChoiGame.bat không đổi WAuto) |
| `Represent3.dll` | `113eeb1d` (đang chạy) | — | giữ |

**Cập nhật 22:5x:** chủ tắt game, tôi ĐÃ đặt `bin/client/CoreClient.dll.moi` 3d706aa2 + `Game.exe.moi` 714e71d3 (chờ `ChoiGame.bat`). `WAuto.exe`: bản 316f7abd của tôi đã bị bản 72862beb (22:47, phiên wauto-9b thi công 'Ác chính': AC_* sau 3 trường của tôi, PRG_VITRI) đè trong `E:/Src_Auto_Ngoai/WAuto/WAuto/Release`; 22:53 wauto-9b ĐÈ cả 3 `.moi` bằng bộ "Ác chính" (nhánh acchinh-0309 9bfe9780 = rep3-0309 20bfcdf5 + tính năng của họ, TẬP CHA của bộ tôi): CoreClient.dll.moi dff1bfc8 · Game.exe.moi aed2d11d · WAuto.exe.moi 72862beb — chủ chạy ChoiGame.bat + đổi tay WAuto.exe.

### 10.10 Đo lại 03/09 22:38–22:45 (Tống Kim, Represent3 113eeb1d) — trả lời "2 nút không đổi gì" và "không thấy địch"

**Vì sao 2 ô mới không đổi gì:** cây sống lúc đó = `CoreClient.dll` 9e7a516d + `Game.exe` eb8c65dc (build từ `main`, chưa có `PRT_HIENTHI`) + `WAuto.exe` a6d74466 (mới). WAuto gửi gói mới, client cũ bỏ qua → không có tác dụng. Cần cặp rep3 (`CoreClient` 3d706aa2 + `Game.exe` 714e71d3). Ngoài ra tệp `APdata/<ID>.dat` cũ ngắn hơn struct nên ô "dòng" ra 0 = tắt → vá di trú (dòng mặc định 1) trong `WAuto.exe` **316f7abd** (`rep3_wauto_hienthi_b.py`).

**Vẽ (jx_paint.log + jx_rep3.log, phiên pid 32840, 231 s trong trận):** fps TB 63 suốt phiên; 1 giây bất thường / 231 s (0,43 %); 6 cú giật ≥ 25 ms, toàn logic; texture VRAM đỉnh 393 MB (ngân sách 512), "vẽ khung này" tới 108 MB (khung rất đông), RAM riêng 274 → 436 MB, nạp 2216 / bỏ 6160 mục trong ~4 phút. Represent2 cùng ngày (pid 704, 2 h 26): 9,2 % giây bất thường, 47,8 cú giật/phút do paint. ⇒ Về vẽ, Represent3 tốt hơn rõ; không có dấu hiệu "không vẽ được sprite" (600 dòng `LoadImage FAIL` đều là biểu tượng chat `spr/Ui3/表情/140..339.spr` không có trong pak, thử lại 10 s/lần — vô hại).

**"Không thấy địch, không thấy chiêu nhưng bị chết" KHÔNG phải do Represent3:** bảng NPC client tối đa 55/256 khe, `nguoi` (người chơi trong bảng) chỉ 10–16, `mocoi` tới 34, khe hỏi NPC `dangcho=0` suốt phiên (không cạn 19 khe), **9.280 gói NPC bị bỏ vì cách ≥ 40 ô** (`S6_XaQuaTam`, FIX-D 26/08; máy chủ cũng chỉ gửi trong `MAX_SYNC_RANGE` 40 ô, `KNpc.cpp:763`). Tức là client chỉ biết địch trong vòng 40 ô và bảng chưa bao giờ đầy; nếu địch trong màn hình mà không thấy thì phải xem gói đồng bộ (mốc [S8-NAN] lệch vị trí 279 px xuất hiện 5 lần) — thuộc phần đồng bộ TK của phiên wauto-c0/9b (họ đã phải hỏi máy chủ vị trí địch vì cùng triệu chứng). Việc cặp rep3 làm được: `MAX_NPC` 800 + `MAX_NPC_REQUEST` 128 (2.0 dùng 800) — chỉ giúp khi bảng/khe đầy, phiên này chưa chạm.

**Đề nghị thử tiếp:** đặt cặp rep3 + WAuto.exe 316f7abd, vào trận, lúc thấy "bị đánh mà không thấy ai" ghi lại giờ; tôi đối chiếu `[S6-BANG] dung/nguoi/boxa`, `SYNCMIN-REQNPC`, `[S8-NAN]` và dòng `RAM rieng` đúng phút đó.

### 10.11 "Không thấy địch và chiêu nhưng bị chết" ở Tống Kim — mổ mã máy chủ + client 2.0 (03/09 đêm)

**Client 2.0 (gamecl, bảng handler tại this+8, khớp 129/146 số hiệu gói của dự án):** cùng dòng mã với ta — `SyncNpcMin` (0x65C8A0) bỏ gói NPC cách người chơi > 35 ô (ta 40 ô), NPC lạ thì `IsNpcRequestExist` → gửi `c2s_requestnpc` → `InsertNpcRequest` (0x66BF00); `SkillCast` (0x659C60) bỏ chiêu của NPC chưa có trong bảng; `NpcRemove` chỉ chạy khi máy chủ bảo. Khác biệt duy nhất đáng kể: bảng NPC 800 (ta 256, rep3 = 800). ⇒ 2.0 "không mất địch" là nhờ **máy chủ VNG**, không phải client.

**Máy chủ của ta (KRegion.cpp / KNpc.cpp), số đo phiên 22:38:** region = 16×32 ô, phát trong 3×3 region (≈48×96 ô, `MAX_SYNC_RANGE 32` kiểm một chiều); client bỏ 9.280 gói vì ≥ 40 ô (`S6_XaQuaTam`) — đúng dải "220 bot ở rìa 40–48 ô" của trận test; trong bảng client chỉ ≤ 16 người + 27 quái, không NPC nào bị client tự gỡ (S6-CAM/VANH/FIX-B = 0), bảng 55/256.
- `KRegion::BroadCast(…, nMaxCount, …)`: ngân sách người nhận DÙNG CHUNG cho cả 9 region, duyệt theo thứ tự danh sách (có con trỏ xoay), **không theo khoảng cách**. `NormalSync` (vị trí liên tục) dùng `NPC_SYNC_BROADCAST_LIMIT 500`; nhưng **gói một lần** — `ProcessState` (phát chiêu, `s2c_skillcast`), `DoHurt`, `DoRun`, `SetCamp/SetCurrentCamp` — dùng `MAX_BROADCAST_COUNT 100`. Với 200+ người trong 9 region (bot rìa), mỗi cú phát chiêu chỉ tới ~100 người đầu danh sách; người đứng cạnh kẻ đánh có thể bị bỏ ⇒ không thấy chiêu, không thấy nó chạy tới, chỉ thấy máu tụt. Vị trí chỉ được vá lại khi tới lượt `NormalSync`.
- `KRegion::Activate`: mỗi region mỗi tick chỉ `NormalSync` **5 NPC** (`kNpcSyncChunkSize`); region 150 NPC ⇒ mỗi NPC ~30 tick (1,7 s) mới có vị trí mới, NPC lạ hiện muộn thêm 1 vòng hỏi-đáp.
- Không có "ép đồng bộ kẻ đánh về nạn nhân": máy chủ trừ máu (`s2c_npchurt`/life) mà không bảo đảm nạn nhân đã có NPC kẻ đánh.

**Hướng (xếp theo hiệu quả/độ rẻ), chờ chủ chọn — mã máy chủ đang do phiên Tống Kim (wauto-c0/9b) giữ:**
1. Nâng `MAX_BROADCAST_COUNT` 100 → 500 (bằng NormalSync) cho gói một lần; hoặc tốt hơn: `BroadCast` ưu tiên người trong 40 ô trước, người rìa sau. Đổi 1 hằng trong `KRegion.h` + rebuild CoreServer. Băng thông: gói chiêu ~30 B × 300 người, không đáng kể.
2. `kNpcSyncChunkSize` 5 → thích nghi theo số NPC trong region (ví dụ max(5, N/8)) để mỗi NPC có vị trí ≤ 0,5 s.
3. "Sync theo đòn đánh": khi NPC A gây sát thương cho người chơi P mà P chưa có A (máy chủ biết qua bảng đã sync), gửi ngay `SendSyncData(A→P)` — bảo đảm kẻ đánh luôn hiện.
4. Client: đặt cặp rep3 (bảng 800 + 128 khe hỏi) — bỏ trần 256/19 như 2.0.
5. Đo trước khi sửa: thêm đếm ở `KRegion::BroadCast` "số người bị bỏ vì hết ngân sách" theo loại gói (log máy chủ mỗi 10 s) và ở client đếm `s2c_skillcast` nhận được / NormalSync — một trận test là biết hướng 1 đúng bao nhiêu.

**Bổ sung từ phiên wauto-75 (giựt lùi/S13), cùng gốc:** chủ báo *"bot TK chạy tới địch nhưng không đánh, lâu sau mới đánh vài cái"* — máy chủ ghi 585 bot chết/phút (bot vẫn đánh), tức lỗi hiển thị; client của chủ trong 2 phút với ~258 "người" quanh mình nhận 4.991 lệnh chạy từ 405 NPC nhưng chỉ 366 gói chiêu từ 95 NPC (181/phút, ~1–2 % lượng chiêu thật). Mã: `KRegion.cpp:1431` `nMaxCount--` nằm NGOÀI khối `if (kết nối && trong tầm && !sleep)` và `KRegion::AddPlayer` (1217) thêm cả bot vào `m_PlayerList` ⇒ 258 bot ăn sạch ngân sách 100 trước người thật. **wauto-75 nhận vá tối thiểu F4** (chỉ trừ ngân sách khi thật gửi; bot không có kết nối nên không ăn ngân sách nữa), build CoreServer từ origin/main mới nhất, đặt `CoreServer.dll.moi` và báo chủ. Hướng 1–3 còn lại vẫn chờ chủ.

### 10.12 Thi công hướng 1 + 5 (chủ chọn 03/09 đêm) — `CoreServer.dll.moi` 486cb9aa

Nhánh **`broadcast-0309`** (worktree `D:\GAMEDEVNEW_wt_bc`, gốc `origin/main e32e7be6` = MAIL D9 + TK đợt 5b + S13k), commit `15d099b3`, đã push. Bộ vá: `ReverseTools\goi_va_broadcast_0309.py` (chạy SAU `git apply` diff F4 của wauto-75). Chỉ đổi `KRegion.cpp` (hàm `BroadCast`) và `KRegion.h`; không đổi gói, không đổi client.

| Phần | Nội dung | Của ai |
|---|---|---|
| F4 | `nMaxCount--` chỉ khi THẬT gửi (có kết nối, trong tầm, không ngủ) → bot (không kết nối) và người ngoài tầm không ăn ngân sách; duyệt VÒNG TRÒN tối đa N node để con trỏ xoay không bỏ sót người thật đứng trước nó; dòng `[BC-DEM] 10s: goi= gui= cat_vi_het_ngan_sach= node_duyet=` | wauto-75 (diff nguyên văn) |
| Hướng 1 | `MAX_BROADCAST_COUNT` 100 → 500 cho gói một lần (phát chiêu `ProcessState`, `DoHurt`, `DoRun`, `SetCamp`/`SetCurrentCamp`/`RestoreCurrentCamp`); NormalSync vẫn 500 | wauto-ca |
| Hướng 5 | `[BC-LOAI] 10s loai:goi/gui/cat: 94:a/b/c 85:… 76:…` — theo byte đầu gói (ProtocolType, `Headers\KProtocolDef.h`: 76 syncnpcmin, 84 npcwalk, 85 npcrun, 90 npchurt, 91 npcdeath, 92 chgcurcamp, 94 skillcast), tối đa 8 loại nhiều nhất + mọi loại có bị cắt; tự reset mỗi 10 s | wauto-ca |

Ghi vào `bin\server\jx_auto_server.log` (config.ini `[AutoLog] On=1` đang bật). Chi phí: vài phép cộng mỗi lần phát; băng thông tăng theo số NGƯỜI THẬT nhận gói một lần (≈30 B/gói/người).

**Swap (máy chủ):** tắt `GameServer.exe` → chạy `bin\server\ChayGameServer.bat` (tự đổi `CoreServer.dll.moi` → `CoreServer.dll`, bản cũ `.truoc`). Bản đang chạy trước đó: `bdd9bb46` (TK đợt 5+5b, đã có trên main). Quay lui: đổi lại `.truoc`.

**Nghiệm thu:** vào Tống Kim đông, sau 1–2 phút xem `jx_auto_server.log`: `[BC-DEM]` có `cat_vi_het_ngan_sach=0`; `[BC-LOAI]` cột `gui` của loại 94/85/90 ≈ số người thật trong tầm × số gói; phía client (script của wauto-75) số gói chiêu nhận ≈ số lệnh chạy nhận theo NPC quanh mình (trước: 366 chiêu/95 NPC so với 4.991 chạy/405 NPC). Nếu `cat` vẫn > 0 nghĩa là > 500 người thật trong 9 vùng — khi đó mới cần ưu tiên theo khoảng cách.

### 10.13 GỐC THẬT của "máy chủ báo not all data was written + Game.exe sập" (04/09 rạng sáng)

Chủ nhắc: *"tìm nguyên nhân lỗi để fix chứ không fix đối phó"*. Dưới đây là chuỗi nhân quả đã lần tới tận mã, kèm số đo.

**1. Cơ chế sập (mã, không phải phỏng đoán) — `Sources\MultiServer\Common\SocketServer.cpp`, biên vào `common.lib` → `heaven.dll` → `GameServer.exe`:**
```
CIOBuffer::SetupWrite()   m_wsabuf.len = m_used;  m_used = 0;      // len = số byte cần gửi
enumIO_Write_Completed:   pBuffer->Use(dwIoSize);                   // used = số byte THẬT gửi được
                          WriteCompleted(pSocket, pBuffer);         // used != len -> CHỈ printf
                          pBuffer->Release();                       // phần CÒN LẠI BỊ VỨT
```
`WSASend` gửi thiếu (socket nghẽn) thì phần đuôi **không bao giờ được gửi lại**. Luồng TCP mất một khúc **giữa gói** ⇒ client đọc `WORD` độ dài rơi vào giữa dữ liệu ⇒ mọi ranh giới gói sau đó lệch ⇒ `KProtocolProcess` xử lý rác ⇒ `0xC0000005`. Dòng chữ chủ chụp được chính là lúc byte bị vứt. Đây là **lỗi có sẵn nhiều năm**, chỉ nổ khi đường truyền nghẽn.

**2. Vì sao nghẽn (số đo `[BC-DEM]`/`[BC-LOAI]`, bản b đang chạy):** máy chủ gửi cho các client thật **trung bình 21.086 gói/giây, đỉnh 30.644 gói/giây** (≈ 1 MB/s), trong khi chỉ có 1–3 kết nối thật (netstat: GameServer 8 kết nối, phần lớn là tiến trình phụ). Thành phần: đồng bộ vị trí NPC (loại 77) ≈ 6.400/s, đồng bộ vị trí người chơi/bot (75) ≈ 6.300/s, lệnh chạy (86) ≈ 1.100–1.800/s, còn lại là đánh/trúng đòn/chiêu. Toàn máy chủ: 142.000 lượt `BroadCast`/giây, quét 11,5 triệu nút danh sách/giây.

**3. Vì sao trước đây không nghẽn:** trần `MAX_BROADCAST_COUNT 100` **vô tình** là bộ hạn dòng — bot (không có kết nối) ăn hết suất nên client thật chỉ nhận ~44 gói/giây. Vá F4 (chỉ trừ suất khi thật gửi) sửa đúng chỗ sai nhưng **gỡ mất cái van tình cờ đó**, phơi ra thiết kế thật: mỗi thực thể một gói, phát cho mọi client trong 9 vùng, tới 18 lần/giây, không có hạn dòng theo client.

**4. Hai lỗi độc lập phải sửa (không phải vá đối phó):**
| # | Lỗi thật | Sửa đúng gốc |
|---|---|---|
| A | Tầng mạng máy chủ **vứt byte khi ghi thiếu** → hỏng luồng → client sập | Gửi lại phần còn thiếu (đẩy lại phần đuôi vào hàng đợi ghi) trong `SocketServer.cpp`; luồng không bao giờ hỏng dù tải nào |
| B | Không có hạn dòng theo client; 21.000 gói/giây là quá sức mọi client | Ưu tiên theo loại: gói vị trí (75/77) được phép bỏ bớt vì lặp lại; gói chiêu/trúng đòn/chết không bao giờ bỏ. Kèm giảm nhịp đồng bộ theo khoảng cách |

**5. Đã sửa đúng và đã bỏ cái sai:** trần 100 → 500 (hướng 1) là **sai mô hình** — số đo cho `cat_vi_het_ngan_sach = 0`, tức trần chưa từng cắt người thật; nâng nó chỉ gỡ thêm van. **Đã trả về 100** (commit `a144fa49`). Lọc tầm hai chiều `|dx|,|dy| ≤ 32` (commit `2703a52e`) giữ lại vì kiểm cũ `nDX <= 32` không lấy trị tuyệt đối là lỗi thật.

**6. `CoreServer.dll.moi` = `014a2b98` (đang chờ, thay bản b `8634b9be` đang chạy):** trần về 100 + `[BC-NGUOI]` đo **theo từng người nhận** (số client, ai nhận nhiều nhất bao nhiêu gói/byte mỗi giây) + `[BC-TOP]` in 8 loại gói nhiều nhất theo lượt gửi (bản trước in nhầm 8 số hiệu nhỏ nhất nên giấu mất 87 đánh / 91 trúng đòn / 95 chiêu). Một lần khởi động lại máy chủ là đủ số liệu chốt cho fix B.

**7. Nhật ký sập 04/09 xác nhận đúng chuỗi trên** (`jx_crash.log`):

| Giờ | Mã lỗi | Nơi rơi | Đọc ra |
|---|---|---|---|
| 00:05:42 | `0xC0000374` HỎNG VÙNG NHỚ ĐỘNG | `RtlFreeHeap` | vùng nhớ đã bị ghi tràn từ trước |
| 00:08:06 | `0xC0000005` GHI vào `0x01300000` | `strcat` ← `KProtocolProcess.cpp` (nhánh xử lý gói chat/extend) | chuỗi trong gói **không có ký tự kết thúc** nên `strcat` chạy quá biên |

Ngăn xếp lần 00:08:06 từ khung [2] trở đi là rác (`?+0x3D08D82`, `0xC0CF8D82` lặp lại) ⇒ **ngăn xếp đã bị đè**. Đây đúng là dấu vết của việc đọc một luồng gói đã lệch: trường độ dài rơi vào giữa dữ liệu, chuỗi không kết thúc, `strcat` ghi tràn, hỏng cả ngăn xếp lẫn vùng nhớ động. Hai kiểu mã lỗi khác nhau (0xC0000005 và 0xC0000374) trong 3 phút là cùng một gốc.

Kèm theo đó là chỗ đáng gia cố: client chép chuỗi từ gói mạng bằng `strcat`/`strcpy` mà **không chặn theo độ dài gói**. Sau khi sửa gốc A + B thì nên chặn biên các chỗ này để một gói hỏng không bao giờ đè được bộ nhớ.

### 10.14 GỐC SẬP CLIENT: chép dữ liệu gói bằng độ dài lấy từ gói, không kiểm biên (04/09 00:3x)

Ba lần sập gần nhất đều rơi vào hàm chép chuỗi, và thanh ghi khớp từng con số:

| Giờ | Rơi tại | Thanh ghi | Đọc ra |
|---|---|---|---|
| 00:32:40 | `memcpy` ← `KPlayerChat::GetChat` (KPlayerChat.cpp:678) | ECX = 0x5080 = **20.608** | chép 20.608 byte vào `szBuf[256]` |
| 00:21:13 | `memcpy` ← `KPlayerTeam::ReceiveInvite` (KPlayerTeam.cpp:112) | ECX = 0xDB84 = **56.196** | chép 56.196 byte vào `szName[32]` |
| 00:08:06 | `strcat` ← nhánh chat/extend | ghi 0x01300000 | chuỗi không có ký tự kết thúc |

**Mã sai:**
```
char szBuf[MAX_SENTENCE_LENGTH];                      // 256 byte trên ngăn xếp
memcpy(szBuf, &pChat->m_szSentence[...], pChat->m_wSentenceLen);   // độ dài WORD LẤY TỪ GÓI, tới 65535
szBuf[pChat->m_wSentenceLen] = 0;                     // ghi thêm một byte ngoài biên
```
`Param.szName[32]` và `Param.cChatPrefix[16]` cũng nhận `m_btNameLen` / `m_btChatPrefixLen` (byte, tới 255) mà không kiểm. Ở `ReceiveInvite` còn nặng hơn: `sizeof(TEAM_INVITE_ADD_SYNC) - pInvite->m_wLength - 1` **tràn ngược** khi `m_wLength` lớn, cho ra số byte khổng lồ.

**Ý nghĩa:** chỉ cần **một** gói hỏng là bẹp ngăn xếp và sập, không cần đường truyền quá tải. Luồng lệch (máy chủ vứt byte khi ghi thiếu, mục 10.13) chỉ là nguồn cung cấp gói hỏng. Đây là lỗi thứ ba, độc lập, và là lý do client vẫn sập cả khi máy chủ đã nhẹ tải.

**Đã vá** (`ReverseTools\goi_va_client_kiembien_0409.py`, nhánh `net-0309` commit `f698d749`, gốc `acchinh-0309 9bfe9780` = đúng nhánh của bộ đang chạy): kiểm biên trước khi chép; gói sai thì **bỏ gói** và ghi `[BIEN-XAU]` một lần mỗi giây vào `jx_auto.log`. Gói hợp lệ không đổi hành vi.

**Tệp:** `bin\client\CoreClient.dll.moi` = **`a2280915`** (2.533.376) — đã kiểm có đủ `[AC]` (Ác chính của wauto-9b), `TKDich`, `NpcTheSame` như bản đang chạy `dff1bfc8`, cộng `BIEN-XAU`. Chủ chạy `ChoiGame.bat` là có; `Game.exe` và `WAuto.exe` giữ nguyên.

**Còn lại chưa giải thích được:** khoá đồng bộ 5 NPC mỗi vùng mỗi nhịp giới hạn một client ở ~1.620 gói/giây, nhưng đo được 61.806 gói/giây (gấp 38 lần). Bản đo `[BC-TUVUNG]` (nhánh `broadcast-0309` `61e9c0c7`) đếm số **vùng khác nhau** gửi tới một client để chốt: đúng thiết kế phải ≤ 9.

### 10.15 Chốt gốc bằng thực nghiệm, rồi sửa đúng chỗ (04/09 01:xx)

**Thực nghiệm đã chốt:** lùi F4 (commit `489a3e18`, `CoreServer.dll` `da312a06`) là **hết sập**. Trước 486cb9aa không sập lần nào; đặt 486cb9aa vào là sập 8 lần trong 40 phút ở 5 hàm khác nhau; lùi ra là hết. Vậy chuỗi nhân quả đã được kiểm chứng chứ không còn là suy luận:

```
trần 100 người nhận tính cho CẢ BOT  →  bot ăn hết suất  →  client thật chỉ nhận ~44 gói/giây
   ├─ hệ quả 1: KHÔNG THẤY ĐỊCH, KHÔNG THẤY CHIÊU (triệu chứng chủ báo từ đầu)
   └─ F4 bỏ trần đó ra  →  client nhận 21.000–94.844 gói/giây  →  nghẽn socket
         →  WSASend ghi thiếu  →  heaven.dll VỨT phần đuôi  →  luồng TCP lệch giữa gói
         →  client đọc rác  →  sập ở bất kỳ hàm nào chạm dữ liệu đó
```

**Sửa đúng chỗ (không lùi, không đối phó) — `CoreServer.dll.moi` = `72348ac7`:**

| Việc | Cách làm |
|---|---|
| Bot không ăn suất người thật | chỉ trừ ngân sách khi **thật sự gửi** (F4) |
| Không để dội gói | **hạn mức theo từng client**: mỗi giây tối đa `[Server] BroadCastGoiToiDa` (mặc định 1500) gói **vị trí** |
| Vẫn thấy chiêu và đòn đánh | hạn mức **chỉ** áp cho gói vị trí (75 người chơi, 77 NPC, 85 đi, 86 chạy). Gói chiến đấu **không bao giờ bị bỏ**: 76 đồng bộ đầy đủ, 84 gỡ NPC, 87 đánh, 88 phép, 91 trúng đòn, 92 chết, 93 đổi phe, 95 phát chiêu, 148 chiêu tức thì, 207 hiện sát thương |
| Đo được | `[BC-DEM]` thêm `bo_vi_tri=` và `han_muc=` |

Ước tính: client nhận tối đa ~1.500 gói vị trí/giây (trước: 44) cộng toàn bộ gói chiến đấu, băng thông ~150–250 KB/giây thay vì 9,6 MB/giây.

**Sửa gốc làm hỏng luồng (tuỳ chọn, bước 2) — `heaven.dll.moi` = `a793834b`:** `SocketServer.cpp` khi `WSASend` ghi thiếu thì **gửi nốt phần còn lại** trên cùng hàng đợi (đúng thứ tự) thay vì vứt; không gửi nốt được thì đóng kết nối hẳn để client không bao giờ đọc rác. Chưa cần dùng ngay: hạn mức ở trên làm socket không còn nghẽn. Bản đang chạy `fa5f0012` (2.953.728) khác cỡ bản build mới (1.124.864) vì cấu hình biên dịch, cùng x64 và cùng một hàm xuất `CreateInterface`, nguồn không đổi từ ảnh chụp tháng 4. `ChayGameServer.bat` **không** tự đổi tệp này.

**Cách chạy (04/09 01:2x, chủ yêu cầu làm luôn bước 2):** đã thêm một dòng `call :capnhat heaven.dll` vào `bin\server\ChayGameServer.bat` (bản gốc lưu `ChayGameServer.bat.truoc_0409`) nên **một lần tắt server + chạy bat là đổi cả hai tệp**: `CoreServer.dll` → `72348ac7` và `heaven.dll` → `a793834b` (bản cũ giữ `.truoc`). Kiểm bản mới trước khi nối vào: cùng x64, cùng một hàm xuất `CreateInterface`, **cùng đúng 5 thư viện phụ thuộc** (advapi32, kernel32, user32, version, ws2_32). Quay lui: đổi `heaven.dll.truoc` → `heaven.dll`. **Tôi không tự tắt GameServer** (luật của chủ).  Client giữ `16c0d5ca` (đã có kiểm biên). Nếu vẫn còn dòng "not all data was written" trong cửa sổ máy chủ thì mới làm bước 2:
```
ren heaven.dll heaven.dll.truoc
ren heaven.dll.moi heaven.dll
```
Quay lui: đổi ngược hai tên đó.

**Chỉnh tay nếu cần:** `bin\server\config.ini` mục `[Server]`, `BroadCastGoiToiDa=1500` (100–20000). Thấy địch chưa mượt thì nâng dần; thấy máy chủ in cảnh báo ghi thiếu thì hạ.

## 10.16 Cắt băng thông tận gốc: không gửi lại gói ngoại hình khi không có gì đổi (04/09 01:5x)

**Câu hỏi của chủ:** "còn cách nào tốt hơn không mà không bị giới hạn" (thay vì hạn mức `BroadCastGoiToiDa`).

**Sự thật đo được.** Gói `s2c_syncplayermin` (loại 75) nặng **234 byte**, và nó **không chứa toạ độ** — toạ độ đi bằng gói NPC (loại 77, 99 byte). Nội dung 234 byte đó là:

| Trường | Byte | Đổi khi chạy? |
|---|---|---|
| `GameTitle[64]` | 64 | không |
| `TongName[32]` | 32 | không |
| `TongTitle[32]` | 32 | không |
| `MateName[32]` | 32 | không |
| `bMeridianLevel[12]` | 12 | không |
| trang bị, ngựa, cấp bậc, danh vọng, phúc duyên, trùng sinh, tốc độ… | ~55 | không |

Nhật ký thật lúc 01:55 (1 client, tên CaiBang), ba cửa sổ 10 giây liên tiếp:

| Cửa sổ | Tổng | Loại 75 (ngoại hình) | Tỉ lệ |
|---|---|---|---|
| 1 | 1.258 KB/10s (122 KB/s) | 783 KB | 62% |
| 2 | 1.007 KB/10s (98 KB/s) | 666 KB | 66% |
| 3 | 1.280 KB/10s (125 KB/s) | 755 KB | 59% |

Tức **máy chủ gửi đi gửi lại tên bang, tên bạn đời, danh hiệu ~1,5 lần/giây cho mỗi người trong tầm nhìn**, chiếm ~62% toàn bộ băng thông.

**Vá (commit `43e15ae6`, nhánh `broadcast-0309`; tích hợp `9bf29ea5` nhánh `int-0409`).** Chỉ phía máy chủ, **không đổi cấu trúc gói, client không cần đổi**:
- `PS_Bam` (FNV-1a) băm nội dung gói trong `KNpc::NormalSync`. Giống hệt lần trước **và** chưa tới kỳ làm mới thì **không phát**.
- Kỳ làm mới `[Server] BroadCastLamMoi` giây (mặc định 5, kẹp 1..60) — lưới an toàn.
- `KNpc::SendSyncData` (một client vừa hỏi riêng NPC này = họ vừa nhìn thấy mình) **xoá dấu vết** ⇒ lần sync kế tiếp phát lại ngay, người mới thấy không bị thiếu trang bị. Quan trọng vì `NPC_SYNC` (gói đầy đủ) **không mang ngoại hình**.
- `[PS-BO]` ghi `gui= bo= (%% bo)` mỗi 10 giây.
- Hai mảng `s_adwPSBam[98000]`, `s_adwPSLuc[98000]` = 784 KB tĩnh, nằm trong `#ifdef _SERVER`.

**Bản chờ swap (01:58):** `CoreServer.dll.moi` = **dd5057d6** (18.328.064) do wauto-e7 build lại từ `mail-0309` đã gộp `43e15ae6` + lọc bot khi phát lương bang (`KTongJX2.cpp`). Đã kiểm nhị phân: có đủ `BroadCastLamMoi`, `[PS-BO]`, `goi ngoai hinh`, `BroadCastGoiToiDa`, `BC-TUVUNG`, `auction_item`, `bangluong`. Bản 15fb8c21 của tôi và 3bda2f1a cũ đều bị thay, không mất gì. Chủ chạy `ChayGameServer.bat` để đổi (bat đã đổi cả `heaven.dll`).

**Dự kiến:** loại 75 giảm ~86–93% ⇒ tổng còn ~55–68 KB/s thay vì 98–125 KB/s. Đo lại bằng chính `[BC-NGUOI]` + `[BC-TOP]` + `[PS-BO]`.

**Ba cách còn lại (chưa làm):** (a) gói vị trí NPC 99 byte cũng mang máu tối đa/nội lực tối đa/4 tốc độ/nhóm nhiệm vụ mỗi nhịp — tách còn ~25 byte, nhưng **phải đổi cả hai đầu**; (b) không đồng bộ người đứng yên; (c) thưa dần theo khoảng cách.

