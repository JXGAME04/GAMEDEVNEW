# BÀN GIAO PHIÊN 08/09 — NHỊP VẼ 144 Hz, HIỆU NĂNG CLIENT VÀ LỖI ÁM MÀU CÒN MỞ

Đọc file này trước khi làm tiếp phần nhịp vẽ / hiệu năng client.
Kèm theo: `KEHOACH_D3D11_TIEP_0809.md` (thứ tự việc #0–#8 chủ duyệt), `BANGIAO_RAM_CLIENT_0809.md`,
`BANGIAO_HIEUUNG_KHAOSAT_0709.md`.

---

## 0. TRẠNG THÁI ĐANG CHẠY (cuối phiên 08/09)

Cây chạy thật: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client`.
Không còn tệp `.moi` nào chờ — chủ đã swap hết.

| Tệp | md5 | Nội dung |
|---|---|---|
| `CoreClient.dll` | 1756f5e6 | bản đã dọn sạch mọi bộ đo sai |
| `Game.exe` | ac0d4766 | nhịp vẽ mới, `PaintFps` chặn trần cả khi vsync |
| `Represent3.dll` | 8eb74a38 | đã gỡ bộ đo nhịp trình khung |
| `Engine.dll` | aeb99715 | `g_SetLoopInterval` + khoá pak dùng chung |

`config.ini` mục `[Client]` chỉ còn năm khoá nhịp vẽ, không còn khoá rác:

```
PaintFps=-1      ; -1 = theo tần số màn hình; 0 = tắt hẳn nhịp vẽ riêng; trần 240
PaintInterp=1    ; 1 = nội suy vị trí; 0 = như trước 15/08 (18 bước/giây)
PaintVsync=1     ; 1 = trình khung theo vblank
PaintSmooth=2    ; 0 = như trước 08/09 | 1 = trung bình trượt | 2 = đồng hồ lý tưởng (khuyên dùng)
PaintLog=1       ; ghi jx_paint.log
```

Nhánh làm việc: `D:\GAMEDEVNEW_wt_delta` (đẩy `git push origin HEAD:main`). Toàn bộ đã đẩy origin/main.

---

## 1. ĐÃ LÀM XONG VÀ ĐANG CHẠY

### 1.1 Việc #1 — nạp tài nguyên ở luồng nền (LIVE)
Sprite và JPEG đọc + giải mã ở luồng nền, luồng vẽ chỉ đẩy lên GPU.
Commit: `33ff2742` [NAP b], `95ca4520` [NAP c], `b1d5f1c6` [NAP d], `c9025f86` [NAP e].

Hai sự cố đã sửa trong lúc làm, **nhớ để không lặp lại**:
- **[NAP c] bản đồ đen nửa màn hình.** Engine `XPackFile` giữ cache phần tử **tĩnh dùng chung** cho mọi pak
  nhưng khoá lại là khoá **riêng từng pak**, hai luồng đọc hai pak ghi đè nhau. Sửa: một khoá tĩnh chung.
  **LUẬT: mọi cache tĩnh của Engine phải khoá chung nếu có luồng thứ hai đọc pak.**
- **[NAP d] map đang đứng bị đen khi mới vào game.** Nền vùng được ghép **một lần** qua `DrawPrimitivesOnImage`;
  nạp nền trả NULL thì ô nền mất vĩnh viễn. Sửa: RAII `Rep3NapDongBo` ép nạp đồng bộ trong mọi hàm dùng ảnh
  **một lần** (ghép, hỏi kích thước, alpha). **LUẬT: nạp nền chỉ dùng cho lệnh vẽ gửi lại mỗi khung.**

Kết quả đo: 20 kỳ 30 giây liên tiếp có **0 khung nạp > 16 ms** (trước: 8 khung/10 phút, đỉnh 75,5 ms).
Cổng lùi `[Client] Rep3NapNen=0`.

### 1.2 Việc #2 — nướng lớp nền: ĐÃ ĐO XONG, ĐỀ NGHỊ BỎ
Đo được 2.154 đơn vị vẽ mỗi khung (npc 982, skill 525, ui 134, map 0, ảnh tạo 16, khác 497),
`DrawPrimitives` 1,0 ms/khung, cả pass vẽ 2,33 ms. Lớp nền **đã** là một ảnh ghép sẵn mỗi vùng
(`l_bPrerenderGround`, cột `map 0`) nên không còn gì để nướng. Chủ chưa trả lời dứt khoát, mặc định là bỏ.

Thay vào đó làm **[VE c]** (`4b17bdc9`): bỏ 300–900 **ô vẽ rỗng** mỗi khung trong `KNpcRes::Draw`
(ô bóng gửi vô điều kiện, và vòng vẽ thân người gửi cả phần trang bị người chơi không mặc).
An toàn vì `KSprControl::Release()` xoá cả tên lẫn `m_dwNameID`. CPU vẽ giảm còn khoảng một nửa lúc cảnh nhẹ.

### 1.3 Việc #3 — vẽ 120–144 Hz (LIVE)
Commit: `c9025f86` [NHIP a], `2865f840` [NHIP b], `96e651f2` [NHIP c], `7486aac5` [NHIP d], `33516657` [NHIP e].

- Trần `PaintFps` 60 → 240; `PaintFps=-1` tự lấy tần số màn hình (59 → 60).
- `PaintVsync=1` trình khung theo vblank; `Rep3Latency` tự về 1 khi vsync.
- Engine thêm hàm xuất `g_SetLoopInterval`: lưới vòng bơm 8 ms → 1 ms khi cần
  (lưới 8 ms chỉ cho tối đa ~125 khung/giây).
- **[NHIP c]** mốc nhịp lấy **trước** `Breathe()` (trước đó lấy sau nên xê dịch theo thời gian chạy logic).
- **[NHIP d]** `PaintSmooth=2` = **đồng hồ lý tưởng**: alpha neo vào mốc lý tưởng của nhịp
  `(m_GameCounter-1)*1000/18` và chia cho nhịp danh định, thay vì chia cho khoảng nhịp lẻ liền trước.
  Đo được: số lần nhịp cắt ngang khi chưa đi hết đoạn **giảm từ 57/115 xuống 2/114**.
- **[NHIP e]** sửa lỗi do chính phiên này gây ra: bật `PaintVsync` thì code vẽ mỗi vòng bơm và **bỏ qua hẳn**
  `PaintFps`. Nay `PaintFps` vẫn chặn trần. **Cảnh báo cho phiên sau: mọi phép thử hạ `PaintFps` trước commit
  `33516657` đều VÔ NGHĨA vì con số đó không có tác dụng.**

Số đo ở 144 Hz: 1.439 khung mỗi 10 giây, khoảng cách khung 4/6/9 ms, nhịp trình khung TB 6,95 ms
với độ lệch chuẩn 0,16–0,42 ms, khung trễ 0–5 trên 4.315.

### 1.4 Các sửa lẻ đã LIVE
- **[MAU]** (`2865f840`) số sát thương trôi theo **nhịp logic** thay vì theo khung vẽ. Trước đó ở 60 fps nó
  trôi nhanh 3,3 lần, 120 fps 6,7 lần. Đây là **bộ đếm theo-khung-vẽ thứ ba** bị bắt (sau làm mượt hướng quay
  và alpha bóng mờ, 15/08). Đã quét toàn bộ đường vẽ, **không còn chỗ nào** (`quet_bo_dem_theo_khung_ve_0809.py`).
  **LUẬT: mọi bộ đếm thời gian trong hàm vẽ phải chốt theo `SubWorld[0].m_dwCurrentTime`.**
- **[NAP e]** (`c9025f86`) tệp thiếu sẵn: từ lần hỏng thứ 3 trở đi giãn nhịp thử lại từ 10 giây lên 10 phút
  (đo 11 phút thấy 334 lượt thử vô ích).
- **[MOCOI]** (`e6d53df4`) `KNpcRes::Init` xoá `m_SceneID` mà **không gỡ nút khỏi cây cảnh**, trong khi
  `KNpcRes::Remove` gỡ đúng; `KNpc::Load` gọi `Init` mà không gọi `Remove` trước. Khe NPC đổi chủ ~7,5 lần/giây
  nên sinh nút mồ côi. **Đây là rò rỉ thật, sửa đúng, nhưng KHÔNG phải nguyên nhân lỗi ám màu** (xem mục 3).

---

## 2. ĐÃ ĐO XONG NHƯNG CHƯA SỬA — VIỆC TIẾP THEO

### 2.1 Tụt FPS lúc đông người — ĐÃ ĐỊNH VỊ, chưa cắt
Dấu dò có sẵn trong `jx_paint.log` đã trả lời:
- Dòng `[LOGIC]`: khung nặng nằm ở **tick logic** (`bre` 30–39 ms), `uihb` và `sau` bằng 0.
- Dòng `[TICK]`: trong tick thì `world` **38–81 ms**, `net` và `scene` ≈ 0.
- `world` = `KSubWorldSet::MainLoop` → `SubWorld[0].Activate()`.
- Bộ vẽ lúc đông chỉ 5,1 ms/khung → **không phải lỗi bộ vẽ**.

Bản đo `[WORLD]` (commit `42c5ac95`, còn trong mã, bật bằng `PaintLog=1`) chia tiếp:
`xoa_co` 0,01 ms, `can_bang` 0,03 ms, **`quet_vung` 0,4–7,1 ms trung bình, đỉnh 19,1 ms**,
9/9 vùng đang chạy, 89–165 NPC mỗi nhịp → khoảng **45 µs mỗi NPC mỗi nhịp**.

**Việc tiếp theo:** chia nhỏ bên trong `KRegion::Activate` (AI / di chuyển / đạn / đồng bộ) để biết cắt chỗ nào.
Tick chạy 18 lần/giây nên 40 ms một nhịp đã ăn 72 % CPU và mỗi nhịp nuốt 5–6 khung ở 144 Hz.

### 2.2 `package.ini` khai sai — chờ chủ quyết
`bin\client\package.ini` khai 40 pak, trong đó:
- **4 pak KHÔNG tồn tại**: `script.pak`, `settings.pak`, `sprvlngaothe2.pak`, `ui.pak`.
- **5 pak CÓ THẬT mà không khai**: `serverlistfree.pak`, `sprgame.pak`, `update05.pak`, `vlngaothe1.pak`,
  `vltkcache.pak` → client không mở, coi như không có.

184 trong 358 tệp client báo `LoadImage FAIL` nằm ở nhóm này (toàn bộ là biểu cảm chat trong `\spr\Ui3\`).
**Chưa tự thêm** vì `vltkcache.pak` là bản tháng 1, thêm vào có thể che tài nguyên mới hơn tuỳ thứ tự ưu tiên.

**BẪY đã mắc, ghi lại:** một tệp chỉ là thiếu khi vắng **cả** tệp rời **lẫn** chỉ mục pak. ID pak =
`g_FileName2Id` của `'\' + đường dẫn đã bỏ '\' đầu, chữ thường` (`KPakList.cpp:93-107`).
Script tra: `scratchpad/kiem_thieu_that.py`.

### 2.3 174 tệp thiếu thật
Truy được về bảng `settings\npcres\`: tóc 015 (nam và nữ) và 012 (nữ) không có tệp nên nhân vật đội kiểu đó
bị trọc; `LadyHorseFront/Middle/Back` ghép sai đường dẫn (ảnh ngựa chỉ nằm ở `\spr\npcres\man`);
`LadyShoulder` thiếu 1.984/2.194 ô; `ManLeftWeapon` mã 000; bóng NPC do mã tự ghép `<tên>b.spr`
mà không kiểm tra tồn tại (`KNpcResNode.cpp:579`, 4.918/8.455 ô bóng không có tệp).
Hậu quả còn lại: `anh_null` khoảng 300 đơn vị mỗi khung.

### 2.4 Việc lẻ còn treo từ trước
- `Goddess.exe.moi` bảng xếp hạng — đổi tay trong `bin\multiserver`.
- `Rep3CacheMB=1500` đang chặn ngân sách VRAM tự động.
- A2: lớp hiệu ứng riêng cho 1,8 % chiêu còn thiếu người phóng.
- RAM máy chủ 8,7 GB lúc khởi động, ổn định, chưa mổ heap.

---

## 3. LỖI ĐANG MỞ — "ÁM MÀU / NHOÈ KHI DI CHUYỂN Ở FPS CAO"

### 3.1 Triệu chứng chủ mô tả (nguyên văn, theo thứ tự phát hiện)
1. "những nhân vật nào ở xa là thấy màu của tên nhân vật - danh hiệu có màu đậm tối hơn"
2. "ai đứng gần là thấy màu bình thường còn ai đứng xa bị ám đỏ hơn"
3. "lúc chụp vào ảnh thì thấy bình thường - còn trong game ai chạy ngoài màn hình vào đều thấy bị"
4. "khi đứng yên thì màu hiển thị đầy đủ đúng màu; khi kích chuột di chuyển là màu ám; di chuyển đến nơi
   thì màu lại đúng; người khác di chuyển thì màu lại bị ám; người khác đứng yên tôi đứng yên thì màu đúng"
5. "khi di chuyển tên npc - bot - người chơi nó có nhòe ra"
6. Ở 59 Hz **vẫn bị nhưng mờ hơn**, 144 Hz thì rõ.
7. `PaintInterp=0` **hết hẳn** nhưng di chuyển giật.
8. `PaintFps=72` nhịp đều: **nhẹ hơn** nhưng còn.

### 3.2 Các khâu ĐÃ ĐO, tất cả đều SẠCH
| Nghi ngờ | Cách đo | Kết quả |
|---|---|---|
| Màu tên sai theo phe | Bộ đếm phe chia gần/xa | Chỉ có phe 1 (cam) và 2 (hồng), không có phe đỏ, không có phe ngoài bảng |
| Nhân vật bị vẽ hai lần | Bộ đếm vẽ trùng trong một khung | **0,00 mỗi khung**, một NPC tối đa 1 lần |
| Hệ chiếu sáng động | Tắt hẳn bằng khoá `AnhSang=0` | Vẫn bị |
| Cú nhảy nội suy | Ghi vị trí vẽ 900 khung | Giảm từ 57/115 xuống **2/114** mà vẫn bị |
| Nhịp trình khung | Đồng hồ hiệu năng, 4.315 khung/30 s | TB **6,95 ms**, lệch chuẩn **0,16–0,42**, khung trễ 0–5 (0,1 %) |
| Nội dung một khung | Ảnh chụp của chủ | **Đúng màu, không có gì lạ** |

Ngoài ra đã loại bằng đọc mã: lớp D3D11 tôn trọng lọc điểm (`D3DTEXF_POINT`), có bù nửa điểm ảnh của D3D9
(`+0.5` trong vertex shader), phép thử alpha và khối trạng thái đều hoạt động; bóng mờ `KNpcBlur` chỉ bật
cho kỹ năng `walkrunshadow` chứ không bật khi đi bộ.

### 3.3 Kết luận hiện tại và mức tin cậy
Mọi khâu **bên trong máy** đều đã đo và đều sạch. Điều kiện duy nhất xác định được: mức độ bị **tỉ lệ với số
lần hình dịch trên màn hình mỗi giây**, và biến mất hoàn toàn khi hình chỉ dịch 18 lần/giây.

Giả thuyết còn lại, **chưa được kiểm chứng**: đây là nhoè do màn hình giữ mẫu (sample and hold). Màn LCD giữ
nguyên một khung suốt chu kỳ quét; khi mắt bám theo vật trượt mượt thì ảnh bị kéo nhoè trên võng mạc, nét chữ
mảnh nhoè vào nền đất nâu đỏ nên thành ám đỏ. Khi nội suy tắt, vật đứng yên trọn 55 ms nên mắt thấy ảnh tĩnh sắc.
Điều này cũng giải thích vì sao ảnh chụp một khung luôn đúng.

**Điểm chưa khớp, phiên sau phải để ý:** nếu thuần là nhoè giữ mẫu thì hạ xuống 72 khung/giây (thời gian giữ
dài gấp đôi) đáng lẽ phải nhoè **hơn**, nhưng chủ báo **nhẹ hơn**. Chưa giải thích được mâu thuẫn này.

### 3.4 Bước tiếp theo đề nghị
1. **Bật tính năng giảm nhoè chuyển động trên màn hình** (Blur Reduction / ELMB / ULMB / 1ms Motion Blur
   Reduction, tuỳ hãng). Đây đúng là tính năng sinh ra để xử lý nhoè giữ mẫu. Nếu hết thì kết luận 3.3 đúng.
2. Nếu vẫn còn: quay video tốc độ cao màn hình (240 fps trở lên) chĩa vào tên một nhân vật đang chạy.
   Trong game đã đo hết mọi khâu nên chỉ còn cách nhìn thẳng vào cái màn hình đang hiện gì.
3. Chưa thử: ép trình khung bằng `DXGI_PRESENT` với swap chain có `DXGI_SWAP_EFFECT_FLIP_DISCARD` +
   `Waitable Object` (đợi đúng vblank ở CPU thay vì chặn trong `Present`). Chỉ nên làm nếu bước 1 và 2 chỉ ra
   vấn đề nằm ở đường trình khung.

### 3.5 Cách dùng tạm cho chủ
- `PaintInterp=0`: hết hẳn ám màu, đổi lại di chuyển giật như trước 15/08.
- `PaintInterp=1` + `PaintFps=72`: mượt, ám nhẹ.
- `PaintInterp=1` + `PaintFps=-1` (144): mượt nhất, ám rõ nhất.

---

## 4. CÁC BẢN ĐÃ THỬ VÀ ĐÃ GỠ — PHIÊN SAU ĐỪNG LÀM LẠI

Gỡ khỏi cả mã nguồn lẫn `config.ini`, xoá luôn script (commit `26884c37`, `33516657`, `e756c2ed`):

| Bản | Đã làm gì | Vì sao gỡ |
|---|---|---|
| `[TENMAU]` | Đếm phe theo khoảng cách | Đã chứng minh màu phe đúng |
| `[VETRUNG]` | Đếm NPC bị vẽ trùng trong một khung | Kết quả 0,00 |
| `[NHIPVE]` | Ghi vị trí vẽ từng khung | Đã lấy đủ số liệu, sửa xong `[NHIP d]` |
| `[CHU]` `VienChu` | Ép viền đen khi alpha 0 | Không phải nguyên nhân; đặt 1 là y hệt cũ |
| `[CHUP]` `ChupThem` | Chụp liên tiếp nhiều khung | Không dùng tới |
| `[SANG]` `AnhSang`/`AnhSangNen` | Tắt hoặc chỉnh nền bản đồ ánh sáng | Tắt hẳn vẫn bị |
| `[CHUBUOC]` | Neo vị trí lớp phủ theo bước | Thử 4, 6, 8 đều còn. **Không thể ăn**: neo trong toạ độ thế giới, mà khi người chơi di chuyển thì cả khung nhìn trượt theo nên trên màn hình chữ vẫn trượt |
| `[TRINH]` | Đo khoảng cách hai lần trình khung | Đã đo xong, nhịp sạch |

**Một phát hiện đáng giữ dù đã gỡ khoá:** hàm dựng bản đồ ánh sáng (`KIpoTree::RenderLightMap`, bản đang biên
dịch ở `KIpoTree.cpp:938`, hai bản trên đều bị chú thích) đặt nền `m_dwAmbient = 0xff101010` = 16 trên thang 64,
trong khi Represent3 coi `0x404040` là trung tính. Mỗi NPC và mỗi đạn là một nguồn sáng bán kính 320, và bản đồ
này được dựng lại **mỗi khung vẽ**. Nếu sau này cần cắt CPU lúc đông người thì đây là chỗ tắt được.

---

## 5. LUẬT RÚT RA TRONG PHIÊN NÀY

1. **Manh mối của chủ phải dùng để LOẠI hướng trước khi mở bất kỳ tác tử nào.** Chủ đã nói "59 Hz bình thường,
   144 Hz mới bị" ngay từ đầu, tôi vẫn đi tra dữ liệu phe — thứ không thể đổi theo tần số màn hình.
2. **Khi chủ nói "A thì bị, B thì không", phải hỏi lại ngay: hết hẳn hay chỉ mờ đi?** Hai câu trả lời dẫn tới hai
   hướng hoàn toàn khác. Trong phiên này tôi mất nhiều vòng vì hiểu nhầm "hết" trong khi thực tế là "mờ đi".
3. **Không chạy nhiều tác tử cho một lỗi nhìn thấy được.** Một câu hỏi chính xác cho chủ, hoặc một phép thử
   bật/tắt bằng khoá config (một lần khởi động), rẻ và đúng hơn mọi fan-out.
4. **Sửa xong thì gỡ ngay các bản sai và các bộ đo đã dùng xong**, cả trong mã lẫn trong `config.ini`.
5. **Bộ đếm thời gian trong hàm vẽ phải chốt theo tick logic**, không theo số khung vẽ.
6. **Đừng kết luận "thiếu tệp" chỉ vì client báo nạp hỏng** — phải tra cả tệp rời lẫn chỉ mục pak.
