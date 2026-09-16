# [NENNHIN 16/09] Nền đen + chớp màn khi di chuyển trên mobile — nguyên nhân gốc ở CODE và bản vá

Tiếp nối `BANGIAO_LOI_NEN_DEN_1609.md` (6 lần vá dữ liệu đều hỏng, chủ chốt *"lỗi nằm ở code"*). Hồ sơ này ghi
**nguyên nhân gốc đã chứng minh bằng mã + log**, bản vá, cách kiểm, cách lùi. Kịch bản vá: `android/va_nguon_mobile_1609_nennhin.py`.

---

## 1. Kết luận

**Lỗi nằm ở đường vẽ nền đất khi ảnh nền ghép của một vùng chưa sẵn.** Trên PC, `KScenePlaceRegionC::PaintGround`
rơi xuống `PaintGroundDirect()` (vẽ từng ô) bất cứ khi nào `GROUND_IMG_OK_FLAG` = false, và hai đường cho **cùng điểm
ảnh** vì PC nạp sprite **đồng bộ**. Bản mobile đã đổi việc nạp sprite thành **bất đồng bộ trong lúc vẽ** (`[NAP 08/09 b]`,
`[VE 11/09]`): khi `m_bVeDangDien`, `TextureResMgr::GetImage` trả NULL cho ảnh đang nạp ở luồng nền
(`TextureResMgr.cpp:522`, `:595-611`) và `TextureResSpr::PrepareFrameData` **bỏ vẽ** khung khi hết ngân sách / khung to /
pak bận (`TextureRes.cpp:641-690`). Vì vậy trên mobile `PaintGroundDirect` = **bỏ ô chưa nạp** → ô đen, khung sau ô hiện
→ đúng chữ của chủ: *"đen màng"*, *"chớp màng khi di chuyển"*.

Ba cơ chế chỉ có trên mobile đẩy các vùng **đang trên màn hình** vào đường lỗi đó:

| # | Cơ chế | Vì sao chỉ mobile / màn rộng |
|---|---|---|
| 1 | `KScenePlaceC::PrerenderGround` coi vùng cách tiêu điểm ≥ 2 là **XA**: 1 vùng/khung + `[NENTRUOC 13/09]` hoãn tới 1,5 s chờ luồng nền chuẩn bị khung. Trong lúc chờ, vùng được vẽ bằng `PaintGroundDirect`. | Luật "8 vùng kề bên = trong tầm nhìn" đúng với 800×600 PC. Fold 7 khung 1040×936 + nhìn rộng 120–150 % (`camera_mobile.ini` ZoomMacDinh, chụm ngón — log 14–16/09 luôn có `[ZOOM] 1000 → 1350/1500`): nửa tầm nhìn tới 920 px = **1,8 vùng ngang, 1,5 vùng dọc** → vùng cách 2 hiện phần lớn thời gian. Màn gập 1436×616 cao 616 px: hàng trên/dưới không bao giờ hiện → *"màn nhỏ không bị"*. |
| 2 | Mỗi lần một vùng kề mới nạp xong (= mỗi lần qua ranh vùng), `ARegionLoaded → SetNestRegion` đặt lại `OK_FLAG` của các vùng láng giềng (cách tiêu điểm 2). `PaintGround` lập tức bỏ ảnh ghép **đúng** đang có mà vẽ trực tiếp. Khung của các ô nền đã bị `CheckBalance` dọn (ảnh ghép được dùng, ô gốc nghỉ > 10 s: log `bo` tăng ~900/30 s) → không còn gì để vẽ → đen rồi hiện lại. | PC: vẽ trực tiếp đồng bộ, không ai thấy. |
| 3 | Vào map: 49 vùng, vùng xa 1/khung + hoãn NENTRUOC → mảng đen ở rìa vài khung tới vài giây. | *"vào game qua map bị đen màng"* |

**Ô đen TĨNH không phải lỗi:** 2 958/6 272 ô của Tiên Cốc Động (map 93) trỏ tới `\system\spr\RegionTileDefault.spr`, tệp
này **không có trên cây dữ liệu PC** (`E:\...\bin\client`, 41 pak) y hệt trên điện thoại — kiểm 16/09 bằng
`android/soi_o_nen_thieu.py --map 93 --du-lieu <cây PC>`: cùng 95 vùng thiếu, cùng 2 958 ô. Đó là cách trình soạn bản đồ đánh
dấu đá/ngoài lối đi trong hang, PC hiện y như vậy. **Không được lấp** — bản vá dữ liệu `vien_nen.pak` (61 MB, bắt engine vẽ
thêm ô) đã gỡ khỏi `dt_v4`.

## 2. Bằng chứng (log Fold 7 `D:\jx1_android_log`)

* Đường ghép **không** mất ô: `tex_null = 0` trong mọi phiên 13–16/09; `[PGND-V] bo > 0` chỉ ở map 93 trước khi có
  `vien_nen.pak` và toàn bộ là `RegionTileDefault.spr`; phiên 16/09 09:18 (`[ONEN]` 40 khối): 2 681 ô VẼ, 0 BỎ, mọi vùng
  thường đủ 64 ô lưới 8×8 (toạ độ lẻ = vật phủ nền như xương, mảnh ghép).
* Đường trực tiếp **có** mất ô: `[VE-GIAT] ... nen: bo ve 476` ngay khung vào map; `bo ve` tổng 4 500–10 400 lượt/phiên
  ở màn 1040×936 với zoom 1350–1500.
* Vùng xa được ghép trong lúc **đang hiện**: `[PGND-R] vung (93,96) #35: 0 ảnh` … `XA 18.4/8`, `XA 19.3/38` — nhánh XA làm
  việc ngay sau khi vào map 93 trong khi khung nhìn 1040×936 × 1,1 phủ tới vùng cách 2.
* Lịch sử: lần đầu chủ báo (14/09 ~22:00) ngay sau khi vào Tiên Cốc Động lần đầu với zoom 135 % (phiên 222342); các
  thay đổi liên quan lên trước đó: `[NENTRUOC 13/09]` hoãn vùng xa, `[ZOOM 13/09]` + `[CAMERA 13/09]` nhìn rộng.
  Commit BANDONHO 14/09 bị gỡ vì tưởng gây đen nền — thật ra log của nó chỉ có `bo > 0` do `RegionTileDefault.spr` của map 93.

## 3. Bản vá (chỉ `JX_MOBILE`; `ios/kiem_android_tuongduong.py --pc` = ĐẠT, PC biên dịch y hệt)

Bất biến được khôi phục: **một vùng đang trên màn hình không bao giờ đi qua đường vẽ bỏ ô.**

1. `KScenePlaceRegionC`: cờ `m_bJxNenAnhCu` = "ảnh nền đang cầm có nội dung của chính vùng này" (bật cuối
   `PrerenderGround`; tắt ở `Clear`, `LeaveProcessArea`, `EnterProcessArea` khi đổi ảnh, `KScenePlaceC::RepresentShellReset`).
   `PaintGround` vẽ ảnh nền khi `OK_FLAG` **hoặc** có ảnh cũ → đặt lại `OK_FLAG` chỉ còn nghĩa "cần ghép lại", ảnh cũ vẫn
   hiện tới khi ghép lại xong (vật phủ mới từ vùng kề hiện sau 1–2 khung, như PC ghép ngay).
2. `KScenePlaceC::PrerenderGround`: vùng **kề bên HOẶC đang trên màn hình** (giao `m_RepresentArea` + 64 px — đúng phép
   thử `Paint()` dùng để chọn vùng vẽ nền) đi nhánh "kề bên". Vùng đang hiện mà **chưa có ảnh nào** thì ghép NGAY, không
   chịu ngân sách 8 ms (hoãn = khung này vẽ bằng đường bỏ ô). Vùng đã có ảnh cũ vẫn chịu ngân sách như cũ.
   Vùng **không** trên màn hình giữ nguyên nhánh XA (1 vùng/khung + hoãn NENTRUOC) → tối ưu 13/09 giữ nguyên, không tốn
   thêm việc cho vùng khuất.
3. Mốc nhật ký: `[NENNHIN] vung (x,y) cach tieu diem dx,dy dang tren man hinh -> ghep theo nhanh ke ben (...)` trong
   `jx_paint.log` (PaintLog=1), 24 dòng đầu. Chuỗi `[NENNHIN` trong `libCoreClient.so` là mốc kiểm gói.

Giá phải trả: ghép đồng bộ thêm cho các vùng cách 2 **đang hiện** (mỗi vùng ~5 ms khi khung đã có, tới ~45 ms lần đầu
khi phải nạp sprite). Đó đúng là việc PC vẫn làm cho vùng trong tầm nhìn; vùng khuất không đổi.

## 4. Kiểm sau khi chủ chơi

* Nhìn: vào Tiên Cốc Động / Tương Dương với nhìn rộng 130–150 %, đi qua ranh vùng: **không còn ô đen trong lối đi, không
  chớp**. Vùng đá ngoài lối đi trong hang vẫn đen (giống PC — đây là thiết kế).
* Log: `jx_paint.log` có `[NENNHIN]`; `[VE-GIAT] nen: bo ve N` vẫn có thể > 0 (NPC / hiệu ứng bỏ 1–3 khung như thiết kế
  `[VE 11/09]`) nhưng không còn là ô nền. `[PGND] ms=` lúc vào map có thể cao hơn một lần (ghép ngay các vùng đang hiện).

## 5. Lùi

Không có công tắc ini (bản vá là luật vẽ). Lùi = APK trước (`109151015`, md5 `1964e241…`), giữ `vien_nen.pak` gỡ vì
nó không liên quan tới lỗi.
