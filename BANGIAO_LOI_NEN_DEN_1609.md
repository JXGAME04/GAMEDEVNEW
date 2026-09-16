# Lỗi nền đen trong map (mobile) — hồ sơ đầy đủ các lần sửa KHÔNG thành công

> **CẬP NHẬT 16/09 chiều — ĐÃ TÌM RA GỐC Ở CODE VÀ VÁ: xem `BANGIAO_NENNHIN_1609.md`** (bản 109161032, commit
> `[NENNHIN 16/09]` 74aed697). Gốc: trên mobile `PaintGroundDirect` bỏ ô chưa nạp (GetImage trả NULL / bỏ vẽ khi đang vẽ), và vùng
> **đang trên màn hình** vẫn rơi vào đường đó vì luật "cách tiêu điểm ≥ 2 = xa" sai với zoom + màn cao, cộng `SetNestRegion`
> đặt lại cờ ảnh ghép mỗi lần qua ranh vùng. Ô đen tĩnh `RegionTileDefault.spr` có y hệt trên cây PC → thiết kế map, không lấp;
> `vien_nen.pak` đã gỡ khỏi `dt_v4`. Phần dưới giữ nguyên làm lịch sử 6 lần vá dữ liệu sai.

**Trạng thái cũ (16/09 sáng): CHƯA SỬA ĐƯỢC. Dừng theo yêu cầu của chủ 16/09.**

> Chủ chốt 16/09: *"lỗi nằm ở code chứ không phải như bạn đang fix — làm mất tính năng tối ưu game"*,
> *"bạn toàn fix kiểu đối phó — trong quy tắc của tôi là fix tận gốc không đối phó"*.
>
> Tài liệu này để **phiên sau làm tiếp**: ghi đủ mọi hướng đã thử, vì sao hỏng, số liệu nào còn
> đứng vững, và những gì đã bị loại trừ — để không ai đi lại đúng những con đường này.

---

## 1. Triệu chứng theo lời chủ (nguyên văn, theo thứ tự thời gian)

| Ngày | Lời chủ |
|---|---|
| 14/09 | *"vào game qua map bị **đen màng** và di chuyển cũng bị **chớp màng**"* |
| 14/09 | *"chơi **màn hình nhỏ không bị** mà tôi **mở màn hình rộng ra là bị**"* |
| 14/09 | *"**tại sao trước không bị mà giờ bị?**"* |
| 15/09 | *"đáng ra map đó **bên ngoài map là màu đen** không phải màu"* |
| 15/09 | *"nhiều map bị chứ không phải 1 map"* |
| 16/09 | *"map tiên cốc động hết nền đen trong map nhưng **nền bên ngoài lại cùng màu với nền trong**"* |
| 16/09 | *"tiến cúc động **vẫn còn bị, vẫn tô sai chỗ** — map khác cũng có bị"* |

**Hai vế quan trọng nhất mà mọi bản vá dữ liệu đều KHÔNG giải thích được:**

1. **"chớp màng"** — ô nền đen là **tĩnh**, nó không chớp. Chớp khi di chuyển là hành vi của
   đường **vẽ / nạp / ghép**, không phải của dữ liệu bản đồ.
2. **"màn nhỏ không bị, màn rộng bị"** — dữ liệu bản đồ **không đổi theo cỡ màn hình**. Cái đổi
   theo cỡ màn hình là đường vẽ.

→ Đây là căn cứ mạnh nhất cho nhận định của chủ rằng **lỗi nằm ở code**. Tôi đã bỏ qua hai vế này
quá lâu vì bị cuốn theo số liệu ô nền.

---

## 2. Sáu lần sửa, lần nào hỏng vì sao

### Lần 1 — 14/09: bù tệp `RegionTileDefault.spr` *(ĐÃ GỠ)*

* **Làm gì:** phát hiện `\system\spr\RegionTileDefault.spr` không tồn tại ở đâu cả (13 pak điện
  thoại lẫn 41 pak cây PC), nên sinh một bản sao từ `黄稀.spr` và thả vào.
* **Kết quả:** vùng ngoài bản đồ biến thành **bãi cỏ ô-liu khổng lồ**.
* **Vì sao hỏng:** chứng minh một tệp **vắng mặt** mới xong **nửa việc**. Phải hỏi tiếp *vắng mặt
  có phải cố ý không?* Dấu hiệu đã nằm sẵn trong tay: tệp thiếu ở **cả bản PC**, và bản PC chạy
  như vậy nhiều năm không ai kêu → đó là **cách trình soạn bản đồ đánh dấu "ngoài vùng chơi"**.
* **Kịch bản:** `android/sinh_o_nen_mac_dinh.py` — đầu tệp đã dán cảnh báo **KHÔNG CHẠY LẠI**.

### Lần 2 — 14/09: `[NENNGOAI]` tô các vùng không có dữ liệu *(ĐÃ GỠ, commit 58f8185d)*

* **Làm gì:** vá **mã nguồn** để tô nền cho những vùng không có dữ liệu.
* **Kết quả:** vẫn là bãi cỏ khổng lồ. Gỡ sạch.
* **Vì sao hỏng:** cùng một sai lầm với lần 1, lặp lại trong cùng một ngày.

### Lần 3 — 15/09: viền 1 ô, giao bằng **tệp rời** *(KHÔNG BAO GIỜ TỚI MÁY)*

* **Làm gì:** ghi lại tệp `_Region_C.dat`, tô viền 1 ô quanh mép vùng chơi + lấp hốc kín; chép
  **tệp rời** vào `D:\jx1_android_data_dt_v4`.
* **Kết quả:** máy chủ báo đã phát, nhưng máy **vẫn y như cũ**.
* **Vì sao hỏng — ba lỗi chồng nhau, cả ba là của tôi:**
  1. **Tệp rời tên GBK không thể giao xuống Android.** Thư mục bản đồ tên chữ Trung
     (`中原南区` = byte `d6 d0 d4 ad c4 cf c7 f8`). Trình tải tạo tệp bằng `new File(chuỗi)`
     (`TaiDuLieuActivity.java:457`); Android mã tên tệp theo **UTF-8**, mà **không ký tự nào mã
     UTF-8 ra được một byte lẻ ≥ 0x80** → trình tải **không thể tạo nổi** thư mục đó.
  2. `.lower()` hạ luôn **byte GBK** (`0xD6` → `0xF6`) → sinh ra thư mục `f6f0f4ade4efe7f8` mà
     game không bao giờ tìm thấy. Phải hạ **chỉ A–Z**.
  3. Cho `--chi-manifest` chạy **qua `head`** làm đứt ống → manifest **không hề được ghi lại**,
     mà lệnh vẫn im. (Và tôi còn thử URL sai bảng mã rồi tưởng máy chủ hỏng — đường dẫn GBK phải
     mã **UTF-8** khi đưa lên URL.)

### Lần 4 — 15/09: cùng bản vá đó, giao bằng **`vien_nen.pak`** *(TỚI MÁY, nhưng quá mỏng)*

* **Làm gì:** đóng các tệp vùng thành một pak, xếp **lên đầu** `package.ini`
  (`KPakList::FindElemFile` lấy pak **đầu tiên** có mã đó).
* **Chứng minh bản vá thật sự tới máy — phép đo vân tay:** mỗi khối `[ONEN] === bat dau ghep …:
  N anh ===` liệt kê đúng N ô nền theo thứ tự kèm số khung và tên ảnh. So với hai bản của cùng
  tệp vùng: **8/19 khối khớp bản ĐÃ VÁ, 0 khối khớp bản CŨ**.
* **Kết quả:** map 93 chỉ bớt **326 → 269** ô đen. Chủ: *"vẫn vậy"*.
* **Vì sao hỏng — thêm ba lỗi của tôi:**
  1. **Lệch một ô ở `rect`.** `rect=` trong `.wor` **bao gồm cả hai đầu**; map 93 có
     `rect=[94,93,109,105]` mà tôi viết `range(rc[1], rc[3])` → **bỏ hẳn cột x=109 và hàng y=105**.
     Vùng `(109,99)` đen 57/64, `(109,100)` đen 57/64, `(101,105)` đen **64/64** — chưa bao giờ
     được chạm tới.
  2. **25/324 cây dữ liệu không đọc được `rect`** → kịch bản `return None` nên **bỏ trắng cả bản đồ**
     (5 cây trong số đó vẫn có dữ liệu vùng).
  3. **map 98 làm sập** bộ sinh (một mục vùng giải nén ra **rỗng** → `struct.unpack_from` ném lỗi,
     đứt cả lượt `--tatca`).

### Lần 5 — 15/09: lấp theo **bán kính 16 ô** *(CHỦ BÁC BỎ)*

* **Làm gì:** tô mọi ô "ngoài" nằm trong 16 ô kể từ nền thật. Lý lẽ: màn vẽ 1040×936, ô 64 px →
  nửa màn 8,1 ô, nên 16 là gấp đôi tầm nhìn.
* **Kết quả:** chủ: *"map tiên cốc động hết nền đen trong map **nhưng nền bên ngoài lại cùng màu
  với nền trong**"*.
* **Vì sao hỏng:** **bán kính là một hằng số chỉnh cho vừa một map**, không phải luật. Ra tới rìa
  bản đồ nó biến cả vành ngoài thành sân cùng màu → **mất ranh giới trong/ngoài**. Đúng kiểu
  "fix đối phó" mà chủ cấm.

### Lần 6 — 16/09: luật "nằm trong lòng cảnh" + ba nguồn đen *(VẪN HỎNG)*

* **Làm gì:** bỏ bán kính. Luật: một ô đen được tô **⇔** trên **hàng** của nó có nền thật ở **cả
  hai phía** và trên **cột** cũng có ở **cả hai phía** (4/4 hướng). Áp cho **ba** nguồn đen (xem §3).
* **Kết quả:** chủ: *"tiến cúc động **vẫn còn bị, vẫn tô sai chỗ** — map khác cũng có bị"*, và
  *"làm mất tính năng tối ưu game"*.
* **Vì sao hỏng:** vẫn là **vá dữ liệu cho một triệu chứng của code**. Và nó **có giá**: pak 61 MB
  phải tải thêm, 8 129 tệp vùng bị thay, và **những vùng trước đây không vẽ nền gì nay phải vẽ tới
  64 ô mỗi vùng** → thêm việc mỗi khung, đúng như chủ nói là ăn vào phần tối ưu.

---

## 3. Số liệu còn đứng vững (phiên sau dùng lại được, khỏi đo lại)

### 3.1. Ô nền — ba nguồn màu đen, không phải một

Đo 16/09 trên toàn bộ cây dữ liệu:

| Nguồn | Là gì | Số ô |
|---|---|---|
| 1 | Ô trỏ tới `\system\spr\RegionTileDefault.spr` (tệp không tồn tại → `GetImage` NULL → bỏ ô) | 445 188 |
| 2 | Ô **không có** trong danh sách nền của vùng (vùng tồn tại nhưng thiếu ô) | 722 239 |
| 3 | Vùng **có tệp** nhưng **mục nền dài 0** → engine không vẽ nền gì, cả ô vuông 8×8 (512×512 điểm) đen | 258 880 |

* Nguồn 3: **387 vùng** nằm trong lòng bản đồ (map 969 có 366 vùng).
* Nguồn thứ tư, **không sửa an toàn được**: vùng **không có tệp** nằm trong lòng bản đồ
  (map 21 có 291 vùng). Tạo tệp vùng mới sẽ đụng dữ liệu **chặn đường** → đổi chỗ người chơi đi
  được = **đổi gameplay**.

> **Bẫy chết người:** log `[ONEN]` chỉ thấy nguồn 1. Nguồn 2 và 3 **không có lệnh vẽ nào để mà bỏ**,
> nên `bo 0` **không** có nghĩa là hết đen. Ngày 16/09 log báo **0 ô bị bỏ** mà chủ vẫn thấy đen —
> đây chính là chỗ chứng minh **đen không nằm ở lớp nền**.

### 3.2. Đo trên máy thật (Fold 7)

* **Phiên `SM-F966U1_20260915_101851`** (`[ONEN]` ghi từng ô nền): 2 657 ô — **1 505 vẽ, 1 112 bỏ,
  0 sai số khung, 0 texture rỗng**, và **cả 1 112 ô bỏ đều xin đúng `RegionTileDefault.spr`**.
  → Vòng vẽ nền **không sai**.
* **Phiên `SM-F966U1_20260916_091820`** (sau khi đã vá): **0 ô nền bị bỏ** trên toàn bộ phần được
  ghi log, mà chủ **vẫn thấy đen**.
* Màn vẽ **1040×936** điểm (cửa sổ 2184×1968, `Rep3SwapchainLogic=100`). Ô nền 64×64 px.
* Máy chủ 8765: điện thoại (10.0.0.127) tải `vien_nen.pak` thành công 15/09 11:05:17 và
  16/09 09:18:16.

### 3.3. Những thứ ĐÃ LOẠI TRỪ (đừng đo lại)

| Nghi ngờ | Kết luận | Căn cứ |
|---|---|---|
| Thiếu ô nền mặc định | **Không phải lỗi** — vắng mặt là cố ý, bản PC cũng vậy | §2 lần 1 |
| Atlas KHỐI (`Rep3AtlasKhoi`) | Không phải | tắt vẫn đen |
| Đường render-target thế giới (`TheGioiRT`) | Không phải | tắt vẫn đen |
| Nhánh tô 0 của atlas (`Rep3GpuBoBanCpu=0`) | Không phải | log xác nhận `bo ban CPU 0 texture`, vẫn đen |
| Bảng màu (palette) | Không phải | 313 hàng cấp phát, 0 hàng chưa tải lên, 0 hàng đen, 0 lô mất; phép thử màu khoá cho thấy ô đen **không hề tới** bảng màu |
| Bảng trạng thái ps | Không phải | 6–7 mục, 0 tràn |
| Mọi đường flush giữa khung | Không phải | phiên phản biện chuyên về cơ chế chặn hết mọi đường |
| Commit `dbebc13f [TAI 14/09]` | **Không phải** | `bo>0` đã có từ 13/09 19:33; 1040×936 có từ 11/09 |
| `g_uRep3FxAnhNull` ~75 000/30 s | **Nhiễu, không phải mất hình** | phần lớn đến từ `GetBoundBox2D` (`KRepresentShell3.cpp:2286`) — **hỏi kích thước**, tên rỗng, không phải lệnh vẽ |
| Cache đuổi texture (`bo` ~880/30 s) | Đúng như bản gốc 2.0 | `CheckBalance` bỏ **1 khung/lượt**, chỉ thứ nghỉ **> 10 s** (`TextureResMgr.cpp:65-112`) |

### 3.4. Hai phép A/B đã làm SAI — phiên sau đừng lặp lại

1. `TheGioiRT=0` **không có tác dụng** vì `bZoom` còn gồm `m_nTgLe` do lia cảnh
   (`KRepresentShell3.cpp:504`).
2. `ZoomCanh=0` / `LiaCanh=0` trong `config.ini` **không có tác dụng** vì
   `settings/camera_mobile.ini [MacDinh]` **ghi đè** (`JxLiaCanh.cpp:602-603`).

→ Muốn tắt thật thì phải sửa **camera_mobile.ini**, và **đăng ký tiêu chí log trước khi thử**.

---

## 4. Hướng chủ chỉ: lỗi ở CODE — những đầu mối chưa ai đụng tới

Xếp theo mức khớp với triệu chứng:

1. **"Chớp màng" khi di chuyển.** Chưa hề có ai giải thích được. Ô nền tĩnh không chớp. Cần gắn log
   **theo khung** vào đúng lúc chớp, không phải log tổng kết 30 giây.
2. **Phụ thuộc cỡ màn hình.** Dữ liệu không đổi theo màn hình → nghi đường ghép/vẽ theo kích thước:
   `KScenePlaceRegionC::PrerenderGround` → `ClearImageData` (xoá bằng GPU, `[XOANEN 13/09]`) →
   `DrawPrimitivesOnImage` → `RIO_CopySprToBufferAlpha`. **Cần xem ảnh đích được xoá và được chép
   có đúng cỡ ở mọi độ phân giải không** — bẫy "blit đích = cỡ khung" đã từng cắn một lần ở bước lia cảnh.
3. **"Trước không bị, giờ bị".** Chưa trả lời dứt điểm. Phải dựng lại mốc: bản nào là bản cuối cùng
   chủ xác nhận **không** bị, rồi nhị phân theo commit — chứ không đoán theo cảm giác.
4. **Lia cảnh / zoom** (`LiaCanh`, `ZoomCanh`, `camera_mobile.ini`) là **tính năng mới của mobile**,
   PC không có. Chúng cho camera đi tới chỗ mà bản PC không bao giờ hiện. Phải A/B **cho đúng** (xem §3.4).
5. `[PGND-V]` chỉ ghi **16 dòng** trong một phiên 3 phút → **bản thân việc ghi log bị giới hạn**.
   Đừng kết luận "không có ô nào bị bỏ" từ chỗ log im.

---

## 5. Trạng thái đang phát (tính đến 16/09)

| Thứ | Giá trị |
|---|---|
| APK | **109151015** (không đổi suốt các lần vá này — mọi bản vá đều là **dữ liệu**) |
| `data/vien_nen.pak` | 64 080 672 byte, 8 129 mục, xếp **đầu** `package.ini` |
| Khoá log | `Rep3ONenLog=40` (vẫn bật) |
| Các khoá tối ưu | `Rep3AtlasKhoi=1`, `TheGioiRT=1`, `Rep3GpuBoBanCpu=1`, `LiaCanh=1`, `ZoomCanh=1` |
| Máy chủ 8765 | chạy lại 16/09 (đã chết qua đêm 15→16/09, xem §6) |

### Gỡ toàn bộ bản vá dữ liệu (trả về đúng như trước)

```bash
python android/vien_dat_pak.py --go && python android/may_chu_tai_du_lieu.py --thu-muc D:/jx1_android_data_dt_v4 --chi-manifest
```

Lệnh này gỡ `vien_nen.pak` khỏi `data\`, trả `package.ini` về 14 pak như cũ, rồi sinh lại manifest.
Máy của chủ sẽ tự xoá pak đó ở lần cập nhật sau (`TaiDuLieuActivity.donPakCu` xoá `data/*.pak`
không còn trong manifest).

---

## 6. Bẫy khâu giao bản — đã cắn trong đợt này

* **Máy chủ 8765 chết là app IM LẶNG vào game với dữ liệu cũ.** `kiemVaTai()` bắt ngoại lệ của
  `docManifest()`; nếu máy đã có `config.ini` thì chỉ chớp dòng *"Không nối được máy chủ tải… Vào
  game với dữ liệu đã có"* trong **1,5 giây** rồi vào game → nhìn y hệt "không có cập nhật mới".
  Đêm 15→16/09 mất một đêm vì đúng chuyện này; máy chủ **không tự chạy lại sau khi khởi động máy**.
* **`chuan_bi_du_lieu.ps1:22` chép đè `package.ini`** từ cây PC → xoá mất dòng pak thêm tay, pak vẫn
  nằm trên đĩa mà **không ai đọc**. Đã thêm chốt vào `android/kiem_dt_v4.py`.
* **Luôn chạy `python android/kiem_dt_v4.py` trước khi báo chủ bất cứ điều gì**, kể cả khi chỉ trả
  lời "đã lên chưa".
* **heredoc của bash nuốt dấu gạch chéo** → viết kịch bản bằng công cụ ghi tệp rồi chạy.

---

## 7. Công cụ để lại (đều đã tự kiểm)

| Tệp | Việc |
|---|---|
| `android/vien_nen_dung.py` | Bộ sinh theo luật "nằm trong lòng cảnh" (lần 6) |
| `android/vien_ve_ban_do.py` | **Vẽ bản đồ ra ảnh** — xám nền thật, xanh sẽ tô, đỏ giữ đen. Nhìn một lần hơn đọc mười bảng số |
| `android/vien_vong_tron.py` | Phép thử vòng tròn: đọc ra ghi lại phải giống hệt từng byte (**5 102/5 102 đạt**) |
| `android/vien_dong_pak.py` | Đóng pak; tự kiểm 100 % mã phải có sẵn trong pak cũ |
| `android/vien_kiem_pak.py` | Đọc y hệt engine theo `package.ini`, đối chiếu từng byte |
| `android/vien_dat_pak.py` | Đặt / **gỡ** pak khỏi thư mục phát |
| `android/vien_do_vantay.py` | **Đo vân tay** khối `[ONEN]` trong log máy thật với dữ liệu trước/sau — biết chắc máy đang đọc bản nào |
| `android/vien_kiem_bo_sot.py` | *Vùng có trong pak* trừ *vùng đã xử lý* — chính phép này bắt được lỗi lệch-một-ô |
| `android/vien_do_vung_rong.py` | Đếm vùng "mục nền dài 0" nằm trong lòng bản đồ |
| `android/kiem_dt_v4.py` | Kiểm thư mục phát trước khi báo chủ |

---

## 8. Tôi sai ở đâu, nói thẳng

1. **Bám một giả thuyết quá lâu.** Có hai vế — *"chớp màng"* và *"màn rộng mới bị"* — mà bản vá dữ
   liệu **không thể nào** giải thích. Tôi vẫn vá dữ liệu suốt ba ngày.
2. **Đối phó chứ không tận gốc.** Bán kính 16 ô là hằng số chỉnh cho vừa một map. Chủ đã đặt luật
   rõ ràng và tôi vi phạm.
3. **Không tính giá phải trả.** Bản vá bắt tải thêm 61 MB và bắt engine vẽ thêm ô nền ở những vùng
   trước đây không vẽ gì — ăn thẳng vào phần tối ưu đã làm. Chủ phải là người chỉ ra điều đó.
4. **Bốn lỗi im lặng trong chính công cụ của tôi** (lệch một ô ở `rect`, bỏ trắng bản đồ thiếu
   `rect`, sập vì vùng rỗng, hạ chữ thường làm hỏng byte GBK) — mỗi lỗi đều làm số liệu trông đẹp
   trong khi việc chưa hề được làm.
5. **Báo "đã lên" mà không kiểm lại máy chủ**, để chủ mất một đêm chờ một bản chưa bao giờ tới máy.
