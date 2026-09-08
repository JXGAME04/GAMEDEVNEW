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
