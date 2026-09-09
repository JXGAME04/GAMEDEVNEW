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
