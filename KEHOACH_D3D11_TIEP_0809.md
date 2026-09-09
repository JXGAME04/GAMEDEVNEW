# KẾ HOẠCH TIẾP SAU D3D11 (chủ duyệt 08/09 16:3x: "note lại thứ tự công việc rồi làm từng cái một, bắt đầu 2 cái đầu")

Trạng thái nền: Represent3 trên D3D11 + texture bảng màu (VRAM −43 %), mảng tĩnh CoreClient đã cắt (RAM ~280 MB), hiệu ứng theo sự kiện
máy chủ (98,1 % chiêu có hình). Chi tiết đo ở `BANGIAO_RAM_CLIENT_0809.md`, `BANGIAO_HIEUUNG_KHAOSAT_0709.md`.

| # | Việc | Lợi | Công | Trạng thái |
|---|---|---|---|---|
| 0 | Mặc định `Rep3Api=11` trong mã + tự lùi D3D9 khi máy không đủ (đã có bước dò) | người chơi không phải sửa config | 0,5 giờ | XONG 16:35, Represent3.dll.moi 8bbe8f5f |
| 1 | **Nạp/giải mã sprite và ảnh nền ở luồng nền**, luồng vẽ chỉ đẩy lên GPU | hết giật khi qua map / lúc đông (đo trước: tách thời gian pak, giải mã, tạo GPU, JPEG nền) | 2 ngày | THI CÔNG XONG 16:50, Represent3.dll.moi 219abc27 CHỜ SWAP + đo lại (xem nhật ký dưới) |
| 2 | **Nướng lớp nền bản đồ từng vùng vào render target**, mỗi khung vẽ một tấm | giảm lệnh vẽ + CPU, lợi cho máy yếu / 4 tab | 2 ngày | ĐO XONG 17:3x: nền đã ghép sẵn, DrawPrimitives 1,0 ms/khung → đề nghị BỎ (xem dưới) |
| 3 | Vẽ 120–144 Hz với nội suy PaintFps (flip model + VRR) | mượt trên màn tần số cao | 1 ngày | THI CÔNG 17:4x (c9025f86), 3 `.moi` chờ swap + đo |
| 4 | Độ phân giải lớn + phóng bằng shader (2K/4K), toàn màn hình không viền | sắc nét, chuyển cửa sổ nhanh | 1–2 ngày | |
| 5 | Hiệu ứng shader: phát sáng chiêu, ngày/đêm, đổi màu trang phục theo bảng màu, mờ nền khi mở bảng | đẹp hơn không cần sprite | 0,5–1 ngày/thứ | |
| 6 | Instancing hàng nghìn sprite một lệnh | CPU vẽ (đã 0,3 µs/lệnh, lợi ít) | 1 ngày | |
| 7 | Đo GPU từng pass, cảnh báo VRAM thật | chẩn đoán | một phần đã có | |
| 8 | Lớp hiệu ứng riêng không cần ô NPC (A2) — vẽ nốt 1,8 % chiêu có người phóng chưa nạp | 100 % chiêu có hình | 2–3 ngày | chờ chủ |

Việc lẻ còn treo: `Goddess.exe.moi` bảng xếp hạng (đổi tay trong bin\multiserver), `Rep3CacheMB=1500` chặn ngân sách VRAM tự động,
RAM máy chủ 8,7 GB lúc khởi động (ổn định, chưa mổ heap), bộ đếm `notgt_ve_toado` tách khỏi `ve` (đã sửa nguồn, chờ build CoreClient).

Luật khi làm: đo trước khi sửa; không giảm trải nghiệm (không 16-bit, không hạ cache, không cắt tầm nhìn); không build/quét khi trận TK chạy;
mỗi việc xong → push origin/main + `.moi` + ghi vào tài liệu này.

## Nhật ký việc #1 — nạp tài nguyên ở luồng nền

**Đo trước (bản 8bbe8f5f, 10 phút, 20 kỳ 30 s, jx_rep3.log `[REP3-NAP]`):** trên luồng vẽ: đọc tệp sprite từ pak 4.675 lần, 632 ms
(TB 0,14 ms, max 8,8 ms); JPEG nền 3 lần, 9,4 ms/ảnh; rút khung 30.729 lần 554 ms; giải mã RLE 64.119 khung 278 ms (4 µs/khung);
tạo GPU 64.119 lần 964 ms (15 µs, max 6,6 ms = tạo trang atlas). Khung vẽ có nạp > 5 ms: 19, > 16 ms: 8, **nặng nhất 75,5 ms**;
các khung nặng trùng những kỳ đọc hàng trăm tệp sprite (nhiều NPC mới cùng lúc). ⇒ phần phải chuyển: đọc tệp sprite + giải mã JPEG.

**Thi công [b] (commit [NAP 08/09 b], `Represent3.dll.moi` 219abc27):** `TextureResMgr::GetImage` gặp ảnh chưa nạp trong lúc vẽ
(`m_bVeDangDien` = giữa RepresentBegin/End) → chèn mục "đang nạp", giao luồng nền (`NapNenGiao`), trả NULL (ảnh đó không vẽ khung này,
hiện ở khung sau). Luồng nền (`_beginthreadex`, ưu tiên thấp) chỉ đọc pak + giải mã: SPR = `LoadSprFile` (ZCache/XPackFile đã có khoá),
JPEG = `LoadJpegDecode` (`get_jpg_image`, khoá riêng) — không đụng device. Luồng vẽ nhận kết quả ở `RepresentBegin` (`NapNenNhan`):
SPR gắn vào mục, JPEG `LoadJpegFinish` tạo texture. Hỏi đồng bộ (`GetImageParam`/`GetImageFrameParam`/`GetImagePixelAlpha`) và mọi
gọi ngoài lúc vẽ vẫn nạp ngay; mục đang nạp mà bị hỏi đồng bộ → nạp ngay, kết quả luồng nền về sau bị bỏ. Cổng lùi `[Client] Rep3NapNen=0`.
Harness: thêm `REP_WARM=N` (vẽ cảnh N lần trước khi chụp) → ảnh **y hệt** bản trước (0 điểm khác), fps 63.
Đọc sau khi chủ chơi: dòng `[REP3-NAP]` — `tep spr`/`jpeg` trên luồng vẽ phải ≈ 0, `khung co nap >16 ms` và `max ms/khung` giảm,
`nen: giao/xong/bo_ve` cho biết số ảnh đã giao / xong / số lượt bỏ vẽ vì đang nạp.

**[c] 16:5x SỰ CỐ sau khi bật [b]: bản đồ đen nửa màn hình.** Không phải nạp hỏng (`LoadImage FAIL` chỉ là các tệp thiếu sẵn). Gốc ở Engine:
`XPackFile` giữ cache phần tử **tĩnh dùng chung mọi pak** (`ms_ElemFileCache`) nhưng khoá là **khoá riêng từng pak** (`m_ReadCritical`) →
luồng nền đọc pak sprite và luồng vẽ đọc pak bản đồ (`get_jpg_image` ảnh toàn cục lúc vào map) cùng lúc, `AddElemFileToCache`/`FreeElemCache`
của pak này ghi đè ô cache pak kia đang memcpy → ảnh bản đồ đọc rác → nửa đen. Sửa: một khoá TĨNH dùng chung cho mọi `XPackFile`
(`ms_ReadCritical`, khởi tạo một lần, giữ nguyên bố cục lớp) → `Engine.dll.moi` 8b9407ad (commit 95ca4520). Kèm: `Lib/lua54/Win32/Lua54Dll.lib`
trong git thiếu `lua4_pushboolean/pushinteger/toboolean` (DLL live có) → Engine không link; đã dựng lại lib từ nguồn và commit.
Tạm thời đã đặt `Rep3NapNen=0` trong config lúc 16:5x để chủ chơi tiếp; sau khi có Engine.dll.moi đã bỏ dòng đó (mặc định 1) → lần ChoiGame.bat
tới nhận cả Engine + Represent3 219abc27 và bật lại nạp nền. Harness (Engine mới + 40 sprite, làm nóng 3 khung): ảnh y hệt, fps 63.
Bài học: lớp pak KHÔNG an toàn đa luồng dù có CS — CS theo pak nhưng cache theo tiến trình; mọi thứ đọc pak từ luồng khác phải qua khoá chung.

**[d] 17:10 SỰ CỐ 2: "mới vào game map đang đứng đen, qua map khác bình thường".** Nền vùng của JX1 được GHÉP MỘT LẦN lúc vào vùng:
`KScenePlaceRegionC` vẽ các sprite ô nền lên ảnh tạo sẵn `_*PlaceGround*_` qua `DrawPrimitivesOnImage`; gọi `GetImage` trong đó gặp sprite
chưa nạp → nạp nền trả NULL → ô nền không được vẽ và không bao giờ vẽ lại → vùng đen. Qua map khác: lúc đó ghép ngoài RepresentBegin/End
nên nạp ngay → bình thường. Sửa: `DrawPrimitivesOnImage`, `ClearImageData`, `GetBitmapDataBuffer`, `ReleaseBitmapDataBuffer` luôn nạp
đồng bộ (RAII `Rep3NapDongBo` tắt `m_bVeDangDien` trong phạm vi hàm); chỉ `DrawPrimitives` (vẽ mỗi khung) mới được nạp nền.
`Represent3.dll.moi` 0b765879 (harness y hệt). Bài học: nạp nền chỉ hợp với lệnh vẽ được gửi lại mỗi khung; mọi chỗ dùng ảnh MỘT LẦN
(ghép lên texture, hỏi kích thước, alpha) phải đồng bộ.

**17:1x kết quả #1 sau [d] (0b765879, 3,5 phút):** kỳ vào map (ghép nền đồng bộ theo thiết kế): 3 khung > 16 ms, max 36,6 ms; **5 kỳ sau: 0 khung
> 16 ms, max 13,9 / 11,2 / 1,7 / 4,8 / 3,0 ms** (trước: 8 khung > 16 ms trong 10 phút, max 75,5). Luồng nền: giao 726, xong 617, hỏng 94
(tệp thiếu sẵn), 784 lượt bỏ vẽ vì đang nạp. Cần thêm 10 phút chơi để chốt.

## Việc #2 — đo trước (bản 43aac5d2, chờ swap)
Lớp nền vùng đã được engine ghép sẵn (một ảnh `_*PlaceGround*_` mỗi vùng, `l_bPrerenderGround`), nên "nướng nền" đã có; câu hỏi là mỗi
khung ~1.000 đơn vị vẽ đến từ đâu và tốn bao nhiêu CPU. Bản [VE a/b] đếm đơn vị vẽ mỗi khung theo loại ảnh (npc / skill / ui / map /
ảnh tạo / khác) và đo CPU luồng vẽ (tổng `DrawPrimitives` mỗi khung, cả khung Begin→End) trong dòng `[REP3-NAP]`. Harness: 96 sprite =
0,48 ms/khung ⇒ ~5 µs mỗi đơn vị; trong game ~1.000 đơn vị ≈ 5 ms/khung là chỗ đáng cắt cho máy yếu. Quyết định hướng #2 sau khi có số.

**Kết quả đo #2 (43aac5d2, 11,5 phút, 17:3x):** mỗi khung 2.154 đơn vị vẽ = npc 982, skill 525, ui 134, map 0, ảnh tạo 16, khác 497;
CPU `DrawPrimitives` 1,0 ms/khung, cả pass vẽ Begin→End 2,33 ms/khung. Lớp nền đã là MỘT ảnh ghép sẵn mỗi vùng (`l_bPrerenderGround`,
`map 0`), nên "nướng nền" không còn gì để cắt; phần CPU vẽ 1 ms/khung không đáng một render target riêng. **Đề nghị bỏ #2.**

## Việc #3 — vẽ 120–144 Hz với nội suy (thi công 17:4x, commit c9025f86 + 77a72547; chờ swap + đo)
Nội suy `PaintInterp=1` (15/08) đã có; rào cản là: trần `PaintFps` 60 trong mã, lưới vòng bơm 8 ms của `KWin32App::Run` (tối đa ~125 khung/giây
và nhịp lệch), Present luôn interval 0. Sửa (`goi_va_nhip1_ve_144hz_0809.py`):
- `S3Client.cpp`: trần 60 → 240; `PaintFps=-1` = tự theo tần số màn hình (`EnumDisplaySettings`, 59 → 60); `PaintVsync=1` = vẽ mỗi vòng, Represent3
  `Present(1)` dẫn nhịp theo vblank (không phụ thuộc đồng hồ), `Rep3Latency` mặc định 1 khi vsync (3 khi không, ini vẫn ghi đè);
  > 60 fps hoặc vsync → `g_SetLoopInterval(1)` (lưới 1 ms) và mốc vẽ phần lẻ (144 = 6,94 ms, nhịp trung bình đúng); ≤ 60 giữ cách cũ
  (neo lúc vẽ thật trên lưới 8 ms, khoảng cách đều). `timeBeginPeriod(1)` khi > 30 fps hoặc vsync.
- `Engine/KWin32App`: hàm xuất `g_SetLoopInterval(ms)` (kẹp 1..16), `Run()` đọc lại mỗi vòng — không đổi bố cục lớp.
- `PaintLog=1` → `jx_paint.log` `[SUM]` thêm: `ve: N khung, cach min/TB/max ms | span tick min..max | PaintFps vsync` để kiểm nhịp.
- Kèm việc 1 [NAP e]: `ResNode.m_nLanHong` — tệp thiếu sẵn từ lần hỏng thứ 3 thử lại mỗi 10 phút thay vì 10 giây (11 phút đo: 334 lượt vô ích).
Bản: `Engine.dll.moi` aeb99715, `Game.exe.moi` 72bb7bd4, `Represent3.dll.moi` 39bce13f (Game.exe cần Engine mới vì import `g_SetLoopInterval`;
CoreClient 32da0130 giữ nguyên, bố cục không đổi). Harness: ảnh y hệt `pal11b.bmp`. Config test: `PaintFps=120` (đo nhịp trên màn 59 Hz —
chỉ kiểm cơ chế, mắt không thấy khác), `PaintLog=1` sẵn; sau khi đo nên để `PaintFps=-1`. Người dùng màn 120/144 Hz: `PaintFps=-1` hoặc
`PaintVsync=1`. Cách đọc `[SUM]`: `cach TB` ≈ 8,3 ms ở 120, max không quá ~2 lần TB; `span tick` 56 quanh 48–64 ms.
Lib: `Lib/release/engine.lib` (+`g_SetLoopInterval`) và `CoreClient.lib` (RAMTINH) đã commit để build Game.exe từ origin/main.

**Nhật ký #3 (18:1x, chủ đã swap 3 `.moi`, chơi ở `PaintFps=120`, màn 59 Hz):** chủ báo "nội suy mượt hơn hẳn". `[SUM]` 25 kỳ: 1.190–1.201
khung/10 s (đúng 120), khoảng cách 1/8/16 ms (min/TB/max), có kỳ đông người max 27–31 ms; span tick 45–69 ms (bình thường 48–68).
Nguyên nhân max 16–19: một tick logic 10–25 ms hoặc một lượt vẽ nặng (2.000 đơn vị) chặn luồng chính, khung tới bị trễ rồi vẽ dồn.
Sửa [NHIP b] (2865f840): trễ > 2 ms thì neo lại mốc, không vẽ dồn; `[SUM]` thêm `ve TB/max` và `tick TB/max` để thấy chi phí một lượt vẽ.
Giới hạn thật: 120 khung/giây chỉ giữ được khi một lượt vẽ < 8 ms; vẽ nặng nhất đo được (Represent3) 4,7 ms + duyệt cảnh Core → chỗ đông
~90–100 khung/giây, không hơn; muốn hơn phải cắt CPU vẽ (việc #2 dạng khác: bớt đơn vị vẽ/khung) hoặc tách luồng vẽ (lớn).

**[MAU 08/09] lỗi lộ ra khi vẽ dày: "số mất máu trôi nhanh hơn"** — `KNpc::PaintBlood` giảm `m_nBloodTime` MỖI KHUNG VẼ (bản gốc vẽ = tick
18/s, số bay 40 tick = 2,2 s); ở 60 fps số bay 0,67 s (nhanh 3,3 lần, từ 15/08 chưa ai để ý), 120 fps 0,33 s. Sửa (2865f840): giảm một lần
mỗi tick logic (`m_dwBloodTick`, cùng kiểu `m_dwLastDirTick`), độ cao cộng phần lẻ tick `g_nPaintAlpha` (POSSHIFT đặt) để bay mượt. KNpc thêm
1 DWORD → `CoreClient.dll.moi` e7a8124d + `Game.exe.moi` 49696247 cùng lúc. Đây là bộ đếm theo-khung-vẽ thứ 3 (sau làm mượt hướng và
alpha bóng mờ 15/08): LUẬT mọi bộ đếm thời gian trong hàm vẽ phải chốt theo `SubWorld[0].m_dwCurrentTime`.
Chủ cũng báo "nội suy thiếu phần kỹ năng": vị trí ám khí đã nội suy từ 15/08 (`KMissle` prev/tick/draw), khung hình sprite chiêu vẫn
18/s (dữ liệu chỉ có bấy nhiêu khung); chưa rõ chủ thấy gì (chiêu bay giật nấc / hiệu ứng lệch thân khi chạy / hiệu ứng bám người chạy
trước / hoạt ảnh giật) → đã hỏi lại.
Màn hình chủ: RTX 3080, 2560x1440 hỗ trợ 59/60/120/144 Hz (đang 59) → chủ tự đổi trong Settings → Display → Advanced display → 144 Hz.

**[NHIP c] 19:3x chủ báo “khi fps cao thấy người chơi và bot di chuyển như tốc biến” (đã để `PaintFps=-1` → 144 trên màn 144 Hz).**
Đo `[SUM]`: 1.440 khung/10 s (đúng 144), khoảng cách 4/6/10 ms, **span tick 53..58 ms** (cảnh nặng 45..69, nạp map 474).
Gốc: (1) mốc tick lấy SAU `Breathe()`+`UiHeartBeat()` nên xê dịch theo thời gian chạy logic; (2) nội suy chia cho khoảng tick
LIỀN TRƯỚC — khoảng thật ngắn hơn thì alpha chưa tới 1000 đã sang tick mới → **vị trí nhảy một đoạn mỗi tick (18 lần/giây)**;
dài hơn thì đóng băng cuối chu kỳ. Sau một cú giật (474 ms) số chia sai gấp 8 lần → nhảy rất mạnh.
Sửa (96e651f2): mốc tick lấy TRƯỚC `Breathe`; số chia = **trung bình trượt** của khoảng tick (kẹp 40..90 ms) × 0,97 (tới 1000 hơi
sớm: đóng băng 2–4 ms dễ chịu hơn một cú nhảy). Lùi: `[Client] PaintSmooth=0`. `[SUM]` thêm `kep N` = số khung bị kẹp trần alpha.
Kèm: bật `PaintVsync=1` trong config — ở 144 fps không vsync, khung được vẽ cách nhau 4–10 ms trong khi màn quét đều 6,94 ms nên
khung bị nhân đôi/bỏ bớt (judder). Chỉ đổi S3Client → `Game.exe.moi` 3b470554 (CoreClient e7a8124d giữ nguyên).

**Rà soát kèm theo (`quet_bo_dem_theo_khung_ve_0809.py`):** quét mọi bộ đếm chạy theo khung vẽ trong Core + S3Client + Represent3.
Không còn chỗ nào: `m_nFrame` trong `PaintInfo` (sao trùng sinh) chốt `GetTickCount`, `ServerImage::GetNextFrame` chốt
`m_dwCurrentTime`, `m_nPointPos` của bản đồ nhỏ chỉ là chỉ số đệm. Ba chỗ sai đã sửa hết (hướng quay, alpha bóng mờ, số sát thương).

**Số liệu vẽ để dành cho việc #2 kiểu khác:** `[REP3]` đếm `anh_null` 0,2–4,7 triệu mỗi 30 s, phần lớn là **tên ảnh RỖNG** (`(k0)`)
— tức mỗi khung có hàng trăm đơn vị vẽ được gửi xuống Represent3 rồi bỏ vì không có ảnh. Đây là chỗ cắt CPU vẽ rẻ nhất còn lại
— **đã truy ra và sửa, xem [VE c] dưới**.

**[VE 08/09 c] (4b17bdc9) — đây chính là việc #2 dạng đúng: bớt đơn vị vẽ, không nướng nền.** Nguồn `KNpcRes::Draw`:
ô BÓNG gửi vô điều kiện dù NPC không có sprite bóng; vòng vẽ THÂN NGƯỜI gửi mọi ô trong bảng thứ tự, kể cả phần trang bị
người chơi KHÔNG mặc (tên rỗng). Vòng vẽ HIỆU ỨNG trang bị ngay trên đã có cửa chặn này từ 04/09, vòng thân người bị bỏ sót.
An toàn: `KSprControl::Release()` xoá CẢ tên lẫn `m_dwNameID`, và `SetSprFile`/`SetFileName` với tên rỗng đều gọi `Release()`
→ tên rỗng thì id cũng 0, `GetImage` không thể ra ảnh. Kèm: không gọi `DrawPrimitives` khi `nPos = 0`.
Kỳ vọng: `khac` trong `[REP3-NAP]` về gần 0, `anh_null` giảm mạnh, `DrawPrimitives` bớt ~20–25 % thời gian.
`CoreClient.dll.moi` 30109252 + `Game.exe.moi` 3b470554 (swap cùng lúc).

## 20:2x — Ba việc chủ báo sau khi lên 144 Hz

**1. “fps 144 nhưng đông giảm mạnh” — ĐÃ ĐỊNH VỊ, không phải bộ vẽ.** Dấu dò có sẵn trong `jx_paint.log` trả lời ngay:
dòng `[LOGIC]` cho biết khung nặng nằm ở TICK LOGIC (`bre` 30–39 ms, `uihb` và `sau` bằng 0); dòng `[TICK]` chia tiếp
`world` **38–81 ms mỗi tick**, `net` và `scene` ≈ 0. `world` = `KSubWorldSet::MainLoop` → `SubWorld[0].Activate()`.
Bộ vẽ lúc đông chỉ 5,1 ms/khung. Tick chạy 18 lần/giây nên 40 ms/tick đã ăn 72 % CPU, mỗi tick nuốt 5–6 khung ở 144 Hz.
Bản đo tiếp `[WORLD]` (42c5ac95, `CoreClient.dll.moi` a620462a) chia nhỏ: xoá cờ NPC / quét vùng / cân bằng cache,
kèm số vùng đang chạy và số NPC mỗi tick, in mỗi 10 giây khi `PaintLog=1`. Đo xong mới biết cắt chỗ nào.

**2. “tên nhân vật – danh hiệu màu đậm tối hơn” — tìm ra một lỗi thật, nhưng chưa khớp hết triệu chứng.**
`KFont3::SetBorderColor` lật ngược ý nghĩa alpha 0: chú thích ngay trên hàm ghi “alpha = 0 nghĩa là KHÔNG vẽ viền”,
thân hàm lại `if (a == 0) a = 0xFF;` — ép viền ĐEN ĐẶC. `KNpc::PaintInfo` khai `dwBorderColor = 0` (KNpc.h:922) và mọi
nhân vật không được chọn đều gọi không truyền màu viền, nên mọi tên, tên bang, danh hiệu đều bị vẽ thêm một lớp viền đen
(KFont3 vẽ hai lượt: viền rồi thân chữ). Không bỏ cứng vì có thể là chủ ý cho chữ dễ đọc: thêm cờ `[Client] VienChu`
(1 = như cũ, 0 = tôn trọng bên gọi), đã đặt 0 trong config test — `Represent3.dll.moi` 4e7bfdf6 (1dddd85a).
Điều chưa giải thích được: cơ chế này làm MỌI tên đậm lên, không riêng nhân vật ở xa. Đã bác bỏ hai giả thuyết khác:
hệ cảnh không thể gọi vẽ một NPC hai lần trong một khung, và lưới ánh sáng động (ứng viên “xa = tối”) đang bị tắt.

**3. PAK — tôi đã sai, chủ đúng.** Trong 358 tệp client báo `LoadImage FAIL`, **184 tệp CÓ THẬT** (toàn bộ là biểu cảm
chat `\spr\Ui3\<thư mục GBK>\140..323.spr`). Gốc nằm ở `bin\client\package.ini`: khai 40 pak, trong đó **4 pak không tồn tại**
(`script.pak`, `settings.pak`, `sprvlngaothe2.pak`, `ui.pak`) và **bỏ sót 5 pak có thật** (`serverlistfree.pak`, `sprgame.pak`,
`update05.pak`, `vlngaothe1.pak`, `vltkcache.pak`) — client không mở 5 pak đó. Cân nhắc trước khi thêm: `vltkcache.pak` là bản
tháng 1, thêm vào có thể che tài nguyên mới hơn tuỳ thứ tự ưu tiên, nên để chủ quyết.
174 tệp còn lại thiếu thật, truy được về bảng `settings\npcres\`: tóc 015 (nam+nữ) và 012 (nữ) → nhân vật đội kiểu tóc đó
bị trọc; `LadyHorseFront/Middle/Back` ghép sai đường dẫn (ảnh ngựa chỉ nằm ở `\spr\npcres\man`) nên nữ cưỡi ngựa mất bộ phận;
`LadyShoulder` thiếu 1.984/2.194 ô; `ManLeftWeapon` mã 000 (tay không); và bóng NPC do mã tự ghép tên `<tên>b.spr` mà
không kiểm tra tồn tại (`KNpcResNode.cpp:579`, 4.918/8.455 ô bóng không có tệp).
**Cách tra đúng, ghi lại để khỏi sai lần nữa:** một tệp chỉ thiếu khi vắng CẢ tệp loose LẪN chỉ mục pak; ID pak =
`g_FileName2Id` của `'\' + đường dẫn đã bỏ '\' đầu, chữ thường` (`KPakList.cpp:93-107`). Script `scratchpad/kiem_thieu_that.py`.
