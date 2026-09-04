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
