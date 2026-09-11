# CLIENT VLTK 2.0 VẼ CẢNH (HOA SƠN, MẠC CAO QUẬT, MINH NGUYỆT TRẤN…) NHƯ THẾ NÀO

**10/09/2026 — CHỈ MỔ NHỊ PHÂN + ĐỌC DỮ LIỆU PAK. KHÔNG SỬA MỘT DÒNG MÃ NÀO. KHÔNG BUILD. KHÔNG SWAP.**

Chủ hỏi: *bản 2.0 vẽ cảnh ở Hoa Sơn / Mạc Cao Quật / Minh Nguyệt Trấn ra sao.*

---

## 1. Mẫu đã mổ

| Nguồn | Ghi chú |
|---|---|
| `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\` | client 2.0 (Level Up), `[Client] Represent=3` |
| `gamecl.exe` 1.313.376 B | UPX; ảnh bung + vá bộ lọc call: `gamecl_fixed.bin` (12,6 MB), VA = 0x401000 + offset |
| 13 pak trong `data\` | `map.pak`, `resource.pak`, `res1/res2.pak`, `slistcl.pak`… |
| Cây dự án `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\data` | 41 pak, để đối chiếu |
| Nguồn dự án `D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\Scene\` | `KScenePlaceC.*`, `KScenePlaceRegionC.*` |

Công cụ (đã đưa vào `ReverseTools\pak_vltk\vltk2\`): `pak_tim.py` (tra tên trong pak theo băm),
`canh_v20.py` (rút `.wor`, đếm ô vùng), `so_canh.py` (so dữ liệu cảnh 2 client), `scroll_v20.py`
(rút toàn bộ `_Scroll.ini`), `anh_nen.py` (kiểm ảnh nền jpg/spr hai bên).

---

## 2. Trả lời ngắn

1. **Lõi cảnh của 2.0 GIỐNG HỆT dự án** — cùng mã nguồn gốc: cùng `MapList.ini`, cùng `.wor`, cùng ô vùng
   `\maps\<khu>\<map>\v_yyy\xxx_Region_C.dat`, cùng ảnh đất dựng sẵn tên `_*PlaceGround*_#~%d~#_` 512×512,
   cùng `KScenePlaceC::Preprocess` + cây phân lớp. Dữ liệu Hoa Sơn ở hai client **trùng từng byte**.
2. **Cái làm Hoa Sơn / Mạc Cao Quật / Minh Nguyệt Trấn "đẹp" trong 2.0 là LỚP ẢNH CẢNH (画面层)**:
   ảnh nền JPG lớn vẽ phía sau cảnh, chạy **thị sai** (parallax), cộng các **lớp mây SPR trôi** phía trước
   và phía sau. Cấu hình nằm ở `\Maps\ScrollSetting\<map_id>_Scroll.ini`.
3. **Dự án CÓ ĐỦ DỮ LIỆU nhưng KHÔNG CÓ MÃ CHẠY**: 26 tệp `_Scroll.ini` + 11 ảnh nền jpg + 9 `cloud*.spr`
   nằm sẵn trong pak của ta (nhiều tệp trùng byte với 2.0), nhưng client ta **không đọc** chúng:
   `KScenePlaceC::LoadGround()` (hàm đọc lớp nền) **không được gọi ở đâu cả**, nên `m_bBackGroundImages`
   luôn = FALSE và `PaintBackGround()` luôn trả TRUE (không vẽ gì). Không có chữ `ScrollSetting` nào trong
   toàn bộ `Sources\`.
4. Khác biệt phụ đo được: 2.0 giữ **15** ảnh đất dựng sẵn, dự án giữ **49**; ảnh nền Mạc Cao Quật của 2.0
   **rộng hơn** bản của ta (1161×1500 so với 774×1500).

---

## 3. Dữ liệu cảnh: hai client giống hệt

`\settings\maplist.ini` của 2.0: 1.035 mục map, 327 đường dẫn khác nhau. Ba map chủ hỏi:

| Map | id trong MapList | đường dẫn |
|---|---|---|
| Hoa Sơn 华山 | 2 (và 212, 333, 864, 865 cho các bản khác) | `\maps\西北南区\华山` |
| Mạc Cao Quật 莫高窟 | 340, 917, 918, 1029, 1030 | `\maps\西北北区\莫高窟` |
| Minh Nguyệt Trấn 明月镇 | 520–526 | `\maps\特殊用地\明月镇` |

Hoa Sơn, đo bằng `so_canh.py`:

| | 2.0 | dự án |
|---|---|---|
| `.wor` | có, `rect=140,106,165,129` | có, **nội dung trùng từng byte** |
| ô vùng `Region_C.dat` | 532 ô, 3,3 MB | 532 ô, 3,3 MB |
| mẫu `v_106\141_Region_C.dat` | 2.752 B, cùng 16 byte đầu | 2.752 B, cùng 16 byte đầu |

`rect` trong `.wor` **không phải toạ độ điểm ảnh** mà là **khoảng chỉ số ô vùng** (Hoa Sơn: cột 140–165,
hàng 106–129). Ô vùng của 2.0 vẫn là 512×1024 điểm ảnh như ta.

Trong `gamecl.exe` có nguyên các chuỗi cũ: `%s\v_%03d\%03d_%s`, `%sRegion_C.dat`, `%sGround.dat`,
`%sBuildinObj.Dat`, `%sOBSTACLE.DAT`, `KScenePlaceRegionC::LoadGroundLayer`, `_*PlaceGround*_#~%d~#_`,
và RTTI `?AVTreeObjSet@?BI@??Preprocess@KScenePlaceC@@AAEXXZ@`. Cùng một dòng mã với ta.

---

## 4. Lớp ảnh cảnh (画面层) — thứ 2.0 có mà ta không bật

### 4.1 Tệp cấu hình
Chuỗi trong `gamecl.exe` tại VA `0x7B0BE6`: **`\Maps\ScrollSetting\%d_Scroll.ini`** (`%d` = id map trong MapList).
Hàm đọc ở `~0x6748FC` đọc lần lượt các khoá: `AreaLeft/Top/Right/Bottom`, `PaintRectLeft/Top/Right/Bottom`,
`SceneCenterPointX/Y`, `PicCenterPointX/Y`, `Rating`, `ViewAreaLeft/Top/Right/Bottom`, `Speed`, `Count`;
mỗi lớp cấp một khối 72 byte, chỉ nhận `Type` = 1, 2, 3; danh sách lớp treo ở `this+0x6A850` và `this+0x6A864`
(hai danh sách `KLinkStructEx<tagSCROLL_INFO,KScenePlaceC>` — RTTI có trong ảnh).

### 4.2 Ba loại lớp (chú thích gốc trong ini)
| Type | Nghĩa gốc | Cách chạy |
|---|---|---|
| 1 | 背景 — ảnh nền | JPG tĩnh, đặt theo `PicCenterPoint`/`SceneCenterPoint`, **thị sai**: “cảnh dịch `Rating` điểm thì nền dịch 1 điểm” |
| 2 | 前景 — tiền cảnh | nhiều SPR (`Image0..Image8`), `Count` bản, trôi với `Speed`, hướng `Angle` (0 = thẳng lên, thuận kim đồng hồ, thang 64) |
| 3 | 背景覆盖层 — lớp phủ nền | như loại 2 nhưng nằm sau vật thể (mây xa) |

`AreaLeft/Top/Right/Bottom` = **vùng kích hoạt tính theo chỉ số ô vùng**: chỉ khi ô vùng tiêu điểm nằm trong
khoảng đó thì lớp mới vẽ. `PaintRect*` = khung màn hình được phép vẽ (1024×768 hoặc 800×600).

### 4.3 Thống kê 37 tệp `_Scroll.ini` của 2.0
94 lớp: **62 loại 1** (ảnh nền, đều có `Rating` + `Image`), **16 loại 2**, **16 loại 3** (đều có
`Speed` + `Angle` + `Count` + `Image0..Image8`). 11 ảnh nền JPG khác nhau, 9 tệp mây `cloud1..9.spr`.

### 4.4 Ba map chủ hỏi

| Map | id | số lớp | ảnh nền | Rating |
|---|---|---|---|---|
| Hoa Sơn | 2 | nhiều lớp nền + tiền cảnh (5.136 B) | `\游戏资源\background\背景图.jpg` 1640×2176 | 8 |
| Mạc Cao Quật | 340 / 1029 / 1030 | 2 lớp nền | `mogaoku.jpg` 1161×1500 và `yueyaquan.jpg` (Nguyệt Nha Tuyền) 1164×1014 | 16 |
| Minh Nguyệt Trấn | 520–526 | 2 lớp nền | `背景图.jpg` | 16 |

Ví dụ Thanh Thành Sơn (id 21, 8.457 B) có 15 lớp: 4 nền + các cặp mây loại 2 (`Speed=2`) và loại 3
(`Speed=1`) cùng `Angle=48`, mỗi lớp 15–20 bản mây lấy từ `cloud1..9.spr`.

---

## 5. Phía dự án: dữ liệu đủ, mã chết

| Thứ | 2.0 | dự án |
|---|---|---|
| `_Scroll.ini` | 37 tệp | **26 tệp** (thiếu 512, 515, 841–848, 968; có thêm 917, 918) |
| ảnh nền JPG | 11 | **11** (9 tệp trùng byte) |
| `cloud1..9.spr` | 9 | **9, trùng byte cả 9** |
| mã đọc `\Maps\ScrollSetting\` | có | **không có** (0 kết quả trong toàn bộ `Sources\`) |
| `KScenePlaceC::LoadGround(KIniFile*)` | — | có định nghĩa (dòng 2058) nhưng **không nơi nào gọi** |
| `KScenePlaceC::PaintBackGround()` | — | có (dòng 2110), luôn trả TRUE vì `m_bBackGroundImages` = FALSE |

Hàm `LoadGround` của ta chỉ đọc 6 khoá (`AreaLeft/Top/Right/Bottom`, `Image`, `PicCenterPointX/Y`),
thiếu `PaintRect*`, `SceneCenterPoint*`, `Rating`, `ViewArea*`, `Speed`, `Angle`, `Count`, `Image0..8`
và không phân biệt `Type` — tức là bản rút gọn, chỉ vẽ được ảnh nền tĩnh, chưa có thị sai và chưa có mây.

Ảnh nền hai bên khác nhau ở đúng 2 tệp:

| Ảnh | 2.0 | dự án |
|---|---|---|
| `mogaoku.jpg` | 235 KB, **1161×1500** | 127 KB, 774×1500 |
| `yueyaquan.jpg` | 95 KB, **1164×1014** | 46 KB, 776×1014 |

---

## 6. Khác biệt phụ trong cách vẽ đất

Đo trong `gamecl.exe` (hàm khởi tạo cảnh `0x6753DE`): 2.0 tạo ảnh đất dựng sẵn **512×512, kiểu 2
(`ISI_T_DRAWINGRC`)**, vòng lặp dừng khi đủ **15** ảnh (`cmp [esi+0x328B4], 0xF`); bố cục cấu trúc xác nhận:
mảng ảnh ở `+0x31EA0`, mỗi ảnh 0xAC byte, 15 × 0xAC = 0xA14, đúng bằng khoảng cách tới bộ đếm `+0x328B4`.
Bảng vị trí vùng của 2.0 có 49 mục (`0x81BE40..0x81BFC8`, bước 8) = `SPWP_MAX_NUM_REGIONS` 49 như ta.

| Hằng số | JX1 gốc (ảnh chụp `9bc7936a`) | dự án hiện nay | 2.0 |
|---|---|---|---|
| `SPWP_NUM_REGIONS_IN_PROCESS_AREA` | 9 | **49** | **15** |
| `SPWP_PROCESS_RANGE` | 3 | 6 | — |
| `SPWP_TRIGGER_RANGE` | 2 | 1 | — |
| `SPWP_TRIGGER_LOADING_RANGE` | 4 | 1 | — |

Nghĩa là dự án giữ vùng xử lý rộng hơn 2.0 (49 ô so với 15) — tốn thêm ảnh đất 512×512 và làm
`Preprocess` phải dựng lại cây từ 49 ô mỗi lần đổi ô vùng. Đây là thay đổi của phía ta, không phải của 2.0.

---

## 7. Nếu sau này muốn làm (CHƯA LÀM, chờ chủ quyết)

Việc cần: gọi được lớp ảnh cảnh trong client ta.
1. Nạp: đọc `\Maps\ScrollSetting\<id>_Scroll.ini` lúc mở map (chỗ đã đọc `.wor`), dựng 2 danh sách lớp
   (nền / tiền cảnh) theo `Type`, giữ nguyên tên khoá gốc để dùng lại y nguyên 26 tệp đang có.
2. Vẽ: loại 1 vẽ trước nền đất, dịch theo `(tiêu điểm − tâm cảnh)/Rating`; loại 3 vẽ sau nền đất trước vật
   thể; loại 2 vẽ sau cùng; mỗi lớp chỉ vẽ khi ô vùng tiêu điểm nằm trong `Area*` và cắt theo `PaintRect*`.
3. Mây: `Count` bản mỗi lớp, vị trí trôi theo `Speed` và `Angle` (thang 64 hướng), quấn vòng trong `ViewArea*`.
4. Ảnh: JPG đi qua đường nạp ảnh sẵn có của Represent3 (đã có bộ đếm `jpeg` trong `[REP3-NAP]`), mây là SPR
   bình thường. Nếu làm, nên xin 2 tệp `mogaoku.jpg` / `yueyaquan.jpg` bản rộng của 2.0 cho khớp bố cục.

Rủi ro cần đo trước: mỗi ảnh nền 1640×2176 mở ra ≈ 7 MB VRAM ở 2 byte/điểm; mây `cloud3.spr` 183 KB;
với bộ gộp lệnh vẽ hiện tại thì thêm 2–4 lệnh Draw mỗi khung, không đáng kể.

---

## 8. Tra lại bằng lệnh

```
cd D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2
python canh_v20.py wor            # rút .wor + đếm map có .wor
python so_canh.py hoason          # so dữ liệu cảnh Hoa Sơn hai client
python scroll_v20.py 1100         # rút mọi _Scroll.ini của hai client
python anh_nen.py                 # kiểm ảnh nền hai client
```
Ảnh bung của `gamecl.exe` và `gdis.py` nằm trong scratchpad phiên `4001d2df…\v20canh\`.
