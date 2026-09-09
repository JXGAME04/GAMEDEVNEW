# KẾ HOẠCH TIẾP SAU D3D11 (chủ duyệt 08/09 16:3x: "note lại thứ tự công việc rồi làm từng cái một, bắt đầu 2 cái đầu")

Trạng thái nền: Represent3 trên D3D11 + texture bảng màu (VRAM −43 %), mảng tĩnh CoreClient đã cắt (RAM ~280 MB), hiệu ứng theo sự kiện
máy chủ (98,1 % chiêu có hình). Chi tiết đo ở `BANGIAO_RAM_CLIENT_0809.md`, `BANGIAO_HIEUUNG_KHAOSAT_0709.md`.

| # | Việc | Lợi | Công | Trạng thái |
|---|---|---|---|---|
| 0 | Mặc định `Rep3Api=11` trong mã + tự lùi D3D9 khi máy không đủ (đã có bước dò) | người chơi không phải sửa config | 0,5 giờ | XONG 16:35, Represent3.dll.moi 8bbe8f5f |
| 1 | **Nạp/giải mã sprite và ảnh nền ở luồng nền**, luồng vẽ chỉ đẩy lên GPU | hết giật khi qua map / lúc đông (đo trước: tách thời gian pak, giải mã, tạo GPU, JPEG nền) | 2 ngày | THI CÔNG XONG 16:50, Represent3.dll.moi 219abc27 CHỜ SWAP + đo lại (xem nhật ký dưới) |
| 2 | **Nướng lớp nền bản đồ từng vùng vào render target**, mỗi khung vẽ một tấm | giảm lệnh vẽ + CPU, lợi cho máy yếu / 4 tab | 2 ngày | sau #1 |
| 3 | Vẽ 120–144 Hz với nội suy PaintFps (flip model + VRR) | mượt trên màn tần số cao | 1 ngày | |
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
