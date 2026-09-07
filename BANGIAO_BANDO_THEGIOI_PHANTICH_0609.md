# HỆ BẢN ĐỒ THẾ GIỚI — MỔ NHỊ PHÂN BẢN 2.0 & ĐỐI CHIẾU DỰ ÁN JX1

**06/09/2026 — CHỈ MỔ NHỊ PHÂN + ĐỌC NGUỒN. KHÔNG SỬA MỘT DÒNG NÀO. KHÔNG BUILD. KHÔNG SWAP.**

Yêu cầu của chủ game:
1. Viết lại hệ bản đồ như bản 2.0, tiếng Việt.
2. Kích vào địa điểm nào thì nhảy qua địa điểm đó.
3. Xem bản 2.0 có giới hạn gì khi kích vào bản đồ tự nhảy map không.
4. **Mổ nhị phân và phân tích trước, chưa làm.**

---

## 0. TRẢ LỜI NGẮN (3 câu quan trọng nhất)

1. **Bản 2.0 KHÔNG có chức năng kích bản đồ để nhảy map.** Cửa sổ bản đồ thế giới của 2.0 chỉ
   để XEM: bấm chuột trái/phải hoặc phím bất kỳ là **đóng cửa sổ**; bấm đúng vào tên 7 thành thì
   hiện **bảng chú thích** (bang chiếm, thuế) rồi thôi. Vì vậy **không có "giới hạn của 2.0" để
   chép** — luật giới hạn phải do ta tự đặt.
2. **Cái 2.0 có mà dự án KHÔNG có là "Tìm đường" (WayFinding)** — nút trên **tiểu bản đồ**
   (không phải bản đồ thế giới): bấm một điểm thì nhân vật **tự chạy** theo lộ trình, cấu trúc
   lộ trình có mang **mã bản đồ** nên chạy được xuyên map. Đây là **đi bộ**, không phải dịch chuyển.
3. ✅ **Bản đồ tiếng Việt: 2.0 CÓ SẴN — và lấy về dùng được ngay.** 2.0 giữ **bản sinh đôi tiếng Việt**
   của đúng 2 tệp ảnh mà dự án đang dùng, chỉ khác **hậu tố `_vn`**:
   `剑侠大地图2_vn.spr` (bản đồ thế giới) và `打怪向导山洞图_vn.spr` (bản đồ sơn động), nằm trong
   `update.pak`. Dự án đang trỏ vào bản **không có `_vn`** (chữ Hán) — **đó chính là lý do chủ thấy
   chữ Hán ở client mình còn 2.0 thì tiếng Việt.** Cùng cỡ 752×576, cùng khuôn dạng ⇒ **thay thẳng được.**
   Ngoài ra còn **2 bản tiếng Việt kiểu khác** (đẹp hơn) ở đường dẫn chưa dò ra, đã rút được theo mã băm.

**Tin tốt:** dự án đã có sẵn gần hết vật liệu cho việc "kích để đi" — kể cả **bảng nối map 493 mục**
và hàm dò trúng đích. Chỉ thiếu đúng phần hành động (xem mục 4.3: `DoDirectMap` là **hàm rỗng**).

---

## 1. MẪU ĐÃ MỔ

| Tệp | Ghi chú |
|---|---|
| `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\gamecl.exe` | 1.313.376 B, nén **UPX NRV2E**. Ảnh đã bung + đã vá bộ lọc `call`: `gamecl_fixed.bin` (phiên `17c6a10c`). **VA = 0x401000 + offset** |
| `...\data\slistcl.pak` | 666 mục — chứa `MapList.ini`, `MapTraffic.ini`, các ini tiểu bản đồ |
| `...\data\res2.pak` | chứa ảnh bản đồ lớn |
| `E:\...\TESTLOFFF_ONLINE\bin\client\` | client JX1 chạy thật (46 pak, 189.913 uid) |
| `D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiWorldMap.cpp` (348 dòng) | bản đồ thế giới dự án |
| `D:\GAMEDEVNEW\Sources\Core\Src\Scene\ScenePlaceMapC.cpp` | bộ nạp/vẽ ký hiệu + `OnDirectMap`/`DoDirectMap` |

Công cụ để lại ở scratchpad phiên `46ee1ca2`: `g20.py` (giải mã lệnh gamecl đã bung),
`scanpat.py` (dò mẫu byte + tìm đầu hàm), `pakfind.py` / `findmapini2.py` / `cmpmapimg.py`
(tra pak theo băm — **băm chạy trên BYTE GBK, không phải ký tự Unicode**), `jx1pak.py`, `mapimg.py`.

---

## 2. BẢN 2.0 — BẢN ĐỒ THẾ GIỚI (bằng chứng nhị phân)

### 2.1 Lớp cửa sổ

Bảng ảo tại `0x795264`; ô số 4 = **WndProc = `0x4D37F0`** (đối chiếu: lớp tiểu bản đồ có bảng ảo
`0x7946AC`, cũng ô số 4 = WndProc `0x4C9E80` — cùng bố cục).

- `Initialize` ≈ `0x4D3398`: nạp `<đường dẫn giao diện>\小地图_世界地图版.ini`, đọc mục `"WorldMap"`
  (chuỗi tại `0x795344`), 8 loại ký hiệu `Country/City/Capital/Tong/Battlefield/Field/Cave/Others`,
  `PureTextBtn`, và 7 mục thành `fengxiang/bianjing/yangzhou/linan/xiangyang/chengdu/dali` +
  `BtnStartPos`.
- `UpdateData = 0x4D3B70`: nạp `\Settings\MapList.ini` (chuỗi `0x795390`), đọc `%d_MapPos`
  (`0x795384`) và `%d_MapType` (`0x795378`) của map đang đứng rồi đặt ký hiệu "ngươi ở đây".

> **Kết luận cấu trúc:** lớp bản đồ thế giới của 2.0 và của dự án **giống hệt nhau về khoá ini,
> loại ký hiệu, nguồn dữ liệu.** 2.0 chỉ khác ở TÊN TỆP ini và ở việc bấm tên thành hiện chú thích.

### 2.2 Bấm chuột làm gì — giải mã `0x4D37F0`

```
4D3804  mov  eax,[esp+0x110]      ; uMsg
4D380B  cmp  eax,0x202            ; WM_LBUTTONUP
4D381D  je   0x4D395E             ;   -> ĐÓNG CỬA SỔ
4D3823  cmp  eax,0x100            ; WM_KEYDOWN
4D3828  je   0x4D395E             ;   -> ĐÓNG CỬA SỔ
4D382E  cmp  eax,0x200            ; WM_MOUSEMOVE
4D3833  jne  0x4D3856
4D3835  mov  ecx,0x8329E8         ;   -> ẩn bảng chú thích
4D383A  call 0x44E560
4D3844  cmp  eax,0x205            ; WM_RBUTTONUP
4D3849  je   0x4D395E             ;   -> ĐÓNG CỬA SỔ
4D384F  cmp  eax,0x565            ; WND_N_BUTTON_CLICK
4D3854  je   0x4D386C             ;   -> xử lý nút tên thành
4D3856  ...  call 0x46D3B0        ; mặc định: WndProc lớp cha
```

Nhánh `0x4D386C` (bấm nút tên thành):

```
4D386F  lea  eax,[edi+0x5C4]      ; nút thành thứ 0
4D3875  cmp  ebp,eax              ; ebp = cửa sổ con vừa bấm
4D387C  add  eax,0x4F8            ; bước 0x4F8 mỗi nút
4D3881  cmp  esi,7                ; đúng 7 nút thành
4D388E  call 0x4D36D0             ; dựng chuỗi  "%s%s:%s"
4D389E  call 0x4D3740             ; dựng chuỗi  "%s:%s" , ",%s:%d,%s:%d"
4D38DA  call 0x44E5C0  (ecx=0x8329E8)   ; nạp bảng chú thích
4D3904  call 0x44E650  ... màu 0x00FFFF33
4D3933  call 0x44E750  ... màu 0x00FFFF33
4D394B  call 0x44E7E0  ( [0x837740],[0x837744] = vị trí con trỏ )
4D3950  mov  dword [0x83775C],1   ; bật hiện bảng chú thích
4D395A  xor  eax,eax              ; TRẢ 0 -> cửa sổ KHÔNG đóng
```

Đường đóng cửa sổ `0x4D3200`:

```
4D3200  mov eax,[0x8BF430]        ; con trỏ đơn thể (singleton)
4D320A  call 0x466640             ; nhả độc quyền
4D3214  or  dword [eax+4],0x2000000   ; cờ huỷ
4D321E  mov dword [0x8BF430],0
4D3228  call 0x4CA4D0             ; = MapToggleStatus (trả tiểu bản đồ về)
4D3232  jmp 0x44E560              ; ẩn bảng chú thích
```

➡️ **Không có một lời gọi nào tới dịch chuyển / đổi map / gửi gói lên máy chủ.**
Đây đúng bằng `KUiWorldmap::CloseWindow()` của dự án (cũng gọi `MapToggleStatus()`).

### 2.3 Vậy "giới hạn khi kích bản đồ tự nhảy map" của 2.0 là gì?

**Không có, vì tính năng đó không tồn tại trong 2.0.** Câu trả lời trung thực cho câu hỏi 3.
Nếu làm, ta **tự thiết kế luật** — đề xuất ở mục 6.

---

## 3. BẢN 2.0 — "TÌM ĐƯỜNG" (WayFinding) TRÊN TIỂU BẢN ĐỒ

Đây mới là thứ 2.0 có mà dự án không có, và là thứ gần nhất với "kích để đi".

### 3.1 Giao diện

Trong ini tiểu bản đồ của 2.0 (mục `slistcl.pak` uid `09AA3EC8`):

```ini
[ScenePos]     Tip=Nhấn chuột tìm đường
[MapPosInput]  Type=2  MaxLen=32          ; ô gõ toạ độ
[WayFinding]   Left=73 Top=129 19x19      ; NÚT tìm đường
               Tip=Nhấn chuột tìm đường
               Image=\spr\UI3\世界面\NewUI\寻路vn.spr
[SwitchBtn] [CaveMapBtn] [WorldMapBtn] [BtnFlag] [MapRect] [Line] ...
```

Nút `WayFinding` là **con thứ `+0x1444`** của lớp tiểu bản đồ (khởi tạo tại `0x4C4CD6`).

### 3.2 Bấm nút "Tìm đường" — `0x4CA20B`

```
4CA20B  lea ecx,[esi+0x37F4]     ; nhãn ScenePos
4CA211  cmp ebx,ecx              ; con vừa bấm
4CA215  lea edx,[esi+0x1444]     ; nút WayFinding
4CA21B  cmp ebx,edx
4CA223  ...  đọc chuỗi ở [0x846EC8] (ô gõ toạ độ)
4CA27E  call 0x51CD00  (op 0x86, 1, 3, 8, ...)
```

### 3.3 Bấm trong khung bản đồ — `0x4C9EE2` → `0x4C60B0`

```
4C9EE2  call GetTickCount ; chống bấm dồn 0x1F4 = 500 ms
4C9F3F  call 0x4C8F00     ; cổng: nếu đã chọn sẵn 1 đích thì dùng luôn, thoát
4C9F4C  ...               ; đổi toạ độ con trỏ -> toạ độ bản đồ (chia 16)
4C9FC9  call 0x54A6A0     ; kiểm vật cản -> vướng thì bỏ
4C9FD6..4CA000            ; phải nằm trong khung MapRect
4CA006  call 0x4C60B0     ; << HÀNH ĐỘNG
```

`0x4C60B0` dựng **hai cấu trúc 3 dword `{mã map, x, y}`**:
`0x846F08` = điểm đi, `0x846F14` = điểm đến; rồi gọi bộ điều phối `0x51CD00` **thao tác 0x86**
hai lần (xoá, rồi tìm đường). Thành công thì ghi đích vào `[0x810CE4]/[0x810CE8]` để nhân vật tự chạy.

Cổng `0x4C8F00` đọc đối tượng đang chọn `[this+0x5F5C]` lấy **`+0x14` = mã map**, `+0x0C/+0x10` = toạ độ
→ nạp vào đúng hai cấu trúc trên.

> 🔴 **Điểm mấu chốt: cấu trúc lộ trình có mang MÃ MAP** ⇒ tìm đường của 2.0 **đi xuyên map được**,
> nhưng là **đi bộ theo lộ trình**, không phải dịch chuyển tức thời.

---

## 4. DỰ ÁN JX1 ĐANG CÓ GÌ (nhiều hơn tưởng)

### 4.1 Ba cửa sổ bản đồ — khớp đúng 2 ảnh chụp của chủ

| Ảnh | Cửa sổ | Nguồn ảnh |
|---|---|---|
| Ảnh 1 (剑网山洞迷宫分布图) | `KUiMapCave` — bản đồ sơn động | `MapList.ini` khoá `CaveMapImage` = `\Spr\Ui3\小地图\打怪向导山洞图.spr` |
| Ảnh 2 (山河社稷图) | `KUiWorldmap` — bản đồ thế giới | `MapList.ini` khoá `WorldMapImage` = `\Spr\Ui3\小地图\剑侠大地图2.spr` |

Chuyển qua lại bằng `MapSetMode()` / `MapToggleStatus()` trong `UiMiniMap.cpp:599-664`.
Nút mở nằm ở tiểu bản đồ: `WorldMapBtn`, `CaveMapBtn` (`UiMiniMap.cpp:207-210`).

### 4.2 Dữ liệu đã có sẵn — đây là phần quý nhất

**`settings\MapList.ini`** (client, 6.518 dòng):

| Mục | Số lượng |
|---|---|
| `_name=` (**tiếng Việt sẵn**) | **1.006** |
| `_MapPos=` (toạ độ trên ảnh bản đồ) | **224** |
| `_MapType=` | 204 |

Phân bố `_MapType`: Battlefield 80 · Cave 48 · Tong 35 · Field 23 · Country 10 · City 5 · Capital 2 · Others 1.
(⚠️ 224 − 204 = **20 map có toạ độ nhưng thiếu `_MapType`** ⇒ hiện không vẽ được ký hiệu.)

**`settings\MapTraffic.ini`** (242.816 B) — **bảng nối map**:

```ini
[1]                        ; mục = MÃ MAP
1_Type=0                   ; 0 = cửa nối map (MapDirect)
1_Point=1484,601           ;     điểm cửa trong map này
1_TargetPoint=89,448       ;     toạ độ rơi xuống ở map đích
1_Index=7                  ;     MÃ MAP ĐÍCH
4_Type=1  4_Content=Lễ Quan            ; 1 = nhãn chữ (đã tiếng Việt)
5_Type=2  5_Content=...YellowPoint.spr ; 2 = ký hiệu ảnh
```

| Bảng nối | Dự án JX1 | Bản 2.0 |
|---|---|---|
| Mục `Type=0` (cửa nối map) | **493** | 498 |
| Số map có ký hiệu | **389** | 406 |
| Nhãn chữ `Type=1` | 1.480 | 1.748 |

➡️ **Đồ thị nối map của dự án gần như đầy đủ ngang 2.0.** Đây chính là dữ liệu cần cho cả
"tìm đường xuyên map" lẫn "kiểm tra đường đi có thông không".

Engine đã đọc sẵn: `KScenePlaceMapC::LoadSymbol` (`ScenePlaceMapC.cpp:1028`), cấu trúc
`KMapTraffic { Content[80]; Kind; Point; TPoint; MapIndex; }` (`GameDataDef.h:165`).

### 4.3 🔴 CHỖ THIẾU DUY NHẤT: `DoDirectMap` LÀ HÀM RỖNG

`Sources\Core\Src\Scene\ScenePlaceMapC.cpp:1381`

```cpp
void KScenePlaceMapC::DoDirectMap(int nX, int nY)
{
    for (int j = 0; j < defMAX_NUM_SYMBOL; j++)
    {
        if (m_SymbolMap[j].Kind != MapDirect) continue;
        ... tính nNpcX/nNpcY ...
        if (trúng vùng ký hiệu)
        {
            break;          // <<< TÌM RA RỒI... KHÔNG LÀM GÌ CẢ
        }
    }
}
```

Trong khi đó **hàm dò trúng đích thì chạy đúng**: `OnDirectMap()` (`ScenePlaceMapC.cpp:1364`)
trả `TRUE` khi con trỏ trúng một cửa nối map.

Và **người gọi cũng đang bị chú thích**: `UiMiniMap.cpp:263-292`

```cpp
/*case WM_LBUTTONUP://toa do
    if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
        g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DO_DIRECT_MAP, nCursorX, nCursorY);
    else { ... DirectFindPos(...); AutoMove(); }
*/
```

Đường dây đã nối sẵn tới tận `CoreShell.cpp:24210-24216`
(`GSMOI_IS_SCENE_DIRECT_MAP` → `OnDirectMap`, `GSMOI_IS_SCENE_DO_DIRECT_MAP` → `DoDirectMap`).

> **Nghĩa là:** dữ liệu ✔ · dò trúng đích ✔ · đường dây lệnh ✔ · **hành động ✘** · **người gọi ✘**.

### 4.4 Những thứ dự án có mà nên tái dùng

- **Đi bộ tự động trong map**: `DirectFindPos()` (`CoreShell.h:1245`). *(`AutoMove()` đang bị chú
  thích ở `CoreShell.h:1249`.)* **Không có** tìm đường xuyên map (grep `WayFinding` = **0 kết quả**).
- **Cửa sổ nhập toạ độ**: `KUiFindPos` (`UiFindPos.cpp`) — đã có, mở từ nhãn `ScenePos`
  của tiểu bản đồ; bắn `GSMOI_SCENE_MAP_TG_COORD` (cắm cờ đích).
- **Dịch chuyển thật, đang chạy — Xa Phu** `script\global\station.lua` (620 dòng):
  - `STATION_ARRAY` = **16 bến**, mỗi chặng có **giá tiền** (`GetPrice2Station`).
  - Dịch chuyển = `NewWorld(map, x, y)` (`ScriptFuns.cpp:15227` → `LuaEnterNewWorld`).
  - **Giới hạn thật của game**: *"Thật xin lỗi ta chỉ có thể đưa ngươi tới nơi ngươi đã đi qua"*
    (`station.lua:153`) và *"không có tiền thì không thể đi đâu được"* (`station.lua:174`).
  - Ngoài ra có **3 điểm ghi nhớ** `GetWayPoint(1..3)` (`station.lua:45-47`).
- **Kênh client↔máy chủ chạy bằng script**: `Core\Src\KScriptProtocol.{h,cpp}` (đã dựng cho hệ thư
  03/09) — có `LuaSendScriptDataToServer` (client→server) và `SP_SendToPlayer` (server→client).
  ➡️ **Đây là đường vận chuyển sẵn có cho yêu cầu "xin đi tới map N", không phải thêm gói mới.**

---

## 5. ✅ BẢN ĐỒ TIẾNG VIỆT — 2.0 CÓ SẴN, LẤY VỀ DÙNG ĐƯỢC NGAY

> **ĐÍNH CHÍNH.** Lượt rút đầu tôi kết luận "2.0 cũng chữ Hán" là **SAI** — chủ game báo lại là vừa
> vào client 2.0 và thấy tiếng Việt. Kiểm lại thì đúng: tôi đã rút nhầm bản chữ Hán vì tra theo
> đường dẫn ghi trong `MapList.ini`, mà **bản tiếng Việt nằm ở tên tệp KHÁC (thêm `_vn`)**.

### 5.1 Quy ước của VNG: bản sinh đôi `_vn`

Quét toàn bộ 13 pak của 2.0 tìm mọi ảnh SPR ≥ 600×400 ⇒ ra **8 ảnh cỡ 752×576** (đúng cỡ bản đồ),
trong đó **6 ảnh nằm trong `update.pak`** mà lượt tra theo đường dẫn ban đầu không đụng tới.
Dò băm ngược ra quy ước đặt tên:

| Đường dẫn | Mã băm | Nằm ở | Nội dung |
|---|---|---|---|
| `\Spr\Ui3\小地图\剑侠大地图2.spr` | `C7C05D3B` | res2.pak · **và dự án JX1** | thế giới, **chữ Hán** |
| `\Spr\Ui3\小地图\剑侠大地图2_vn.spr` | `43841ADC` | **update.pak** | thế giới, **TIẾNG VIỆT** ✅ |
| `\Spr\Ui3\小地图\打怪向导山洞图.spr` | `9E2787EB` | res2.pak · **và dự án JX1** | sơn động, **chữ Hán** |
| `\Spr\Ui3\小地图\打怪向导山洞图_vn.spr` | `88D08A38` | **update.pak** | sơn động, **TIẾNG VIỆT** ✅ |

🔴 **ĐÂY LÀ GỐC CỦA VIỆC "client mình chữ Hán, 2.0 tiếng Việt":**
`settings\MapList.ini` của dự án ghi

```ini
WorldMapImage=\Spr\Ui3\小地图\剑侠大地图2.spr        ; <- bản chữ Hán
CaveMapImage=\Spr\Ui3\小地图\打怪向导山洞图.spr      ; <- bản chữ Hán
```

tức là trỏ đúng vào **bản không có `_vn`**. Bản `_vn` **cùng cỡ 752×576, cùng khuôn dạng SPR,
cùng 437.424 byte** ⇒ **thay thẳng, không phải sửa một dòng mã nào.**

### 5.2 Bốn ảnh tiếng Việt đã rút được

| Mã băm | Tiêu đề trong ảnh | Ghi chú |
|---|---|---|
| `43841ADC` | **BẢN ĐỒ SƠN HÀ XÃ TẮC** | thế giới, chữ trắng; **có Vũ Hồn, Hoành Sơn phái, Chiến trường Tống Kim** |
| `A7A36384` | **Bản Đồ Sơn Hà Xã Tắc** | thế giới, tiêu đề vàng, la bàn Bắc/Tây/Đông/Nam tiếng Việt |
| `88D08A38` | **PHÂN BỐ CÁC MÊ CUNG SƠN ĐỘNG** | sơn động; chú giải dịch đủ (Nam Tống/Kim Quốc/Đại Lý/Chư hầu/Thành thị/Đồng hoang/Chiến trường/Sơn động/Môn phái/Đường đi/Thôn trấn) |
| `371420E4` | **Bản Đồ Phân Bố Mê Cung Sơn Động** | sơn động, **đẹp và rõ nhất**, đủ tuyến đường + chấm đỏ nối động |

`43841ADC` và `88D08A38` là **cặp `_vn` chính thức** (khớp đúng tên mà `MapList.ini` đang trỏ).
`A7A36384` và `371420E4` là **hai bản kiểu khác**, đẹp hơn, chưa dò ra đường dẫn nhưng **đã rút được
theo mã băm** ⇒ vẫn dùng được. **Chủ chọn bản nào cũng được.**

### 5.3 🔴 BẪY GIẢI MÃ SPR (ghi lại để lần sau khỏi mất công)

`ReverseTools\bandonghanh\spr2png.py` đọc bảng màu ở **offset 28**; nhóm ảnh bản đồ này để bảng màu
ở **offset 32** ⇒ giải ra **nhiễu hạt xanh lơ**, suýt kết luận nhầm là "ảnh hỏng / chữ Hán".
Bộ giải đã sửa: `scratchpad\spr_v2.py` (tự thử 28 và 32, chọn bản mượt hơn). Chỉ số điểm ảnh và
tiêu đề khung vẫn ở `base = 28 + 256*3 + 8 = 804` cho **cả hai** biến thể.

### 5.4 Còn thiếu gì để bản đồ "tiếng Việt trọn vẹn"

Ảnh `_vn` đã dịch **tên địa danh vẽ trong ảnh**. Vẫn nên làm thêm (không bắt buộc):

- **Vẽ nhãn lúc chạy** từ `MapList.ini` (`_name=` tiếng Việt + `_MapPos=`, **224 điểm sẵn có**) —
  để tên map do server đặt luôn khớp, và **dùng lại đúng dữ liệu đó để dò chuột cho việc bấm-để-đi**
  ⇒ một công đôi việc.
- Ảnh `_vn` của 2.0 **chưa có** các map riêng của dự án (Vũ Hồn/Tiêu Dao có trong `43841ADC`,
  nhưng map mới thêm sau này thì không) ⇒ nhãn vẽ lúc chạy bù được chỗ này.

---

## 6. ĐỀ XUẤT — CẦN CHỦ GAME CHỐT TRƯỚC KHI LÀM

### 6.1 Ba mức cho "kích vào địa điểm thì đi tới đó"

| Mức | Cách chạy | Ưu | Nhược |
|---|---|---|---|
| **A. Dịch chuyển thẳng** | bấm → `NewWorld(map,x,y)` ngay | dễ nhất, đúng chữ "nhảy qua địa điểm đó" | **phá cân bằng**: Xa Phu, Thổ Địa Phù, 3 điểm ghi nhớ, tiền đi đường thành vô nghĩa |
| **B. Kiểu Xa Phu** (đề nghị) | bấm → máy chủ kiểm (đã đi qua? đủ tiền?) → trừ tiền → `NewWorld` | dùng lại **đúng luật đang có trong game**, không phá cân bằng | chỉ tới được các thành/bến, không tới mọi điểm |
| **C. Kiểu 2.0 (tìm đường)** | bấm → tìm lộ trình qua 493 cửa nối → **tự chạy bộ** | giống bản 2.0 nhất, không phá cân bằng chút nào | việc lớn nhất: phải viết bộ tìm đường xuyên map + đi bộ tự động |

**Thứ tự đề nghị:** làm **nhãn tiếng Việt + dò chuột + hiện chú thích** trước (không đụng cân bằng,
thấy kết quả ngay) → rồi **B** → rồi **C** nếu chủ muốn giống hệt 2.0.

### 6.2 Giới hạn đề nghị (vì 2.0 không có sẵn để chép)

- **Theo loại map**: chỉ cho `City` / `Capital` (và `Country` nếu chủ muốn). **Cấm** `Battlefield`
  (80 map!), `Tong` (35 map phái), `Others`.
- **Cấm khi đang ở map sự kiện**: Tống Kim, Công Thành Chiến, Phong Lăng Độ, Vượt Ải, Viêm Đế —
  đúng như bài học Vận Tiêu 06/09 *(map sự kiện chặn mọi tự-di-chuyển)*.
- **Cấm khi**: đang giao chiến / bị đánh dấu PK, đang áp tiêu, đang trong tổ đội phó bản.
- **Điều kiện**: phải **đã từng đi qua** nơi đó (dùng lại `GetStation`/`GetWayPoint` của Xa Phu),
  **trừ tiền** theo `GetPrice2Station`, có **thời gian chờ** giữa 2 lần.
- 🔴 **Bắt buộc kiểm ở MÁY CHỦ, không kiểm ở client** — client chỉ gửi "xin đi tới map N";
  máy chủ tự quyết. Nếu tin client thì thành lỗ hổng dịch chuyển tuỳ ý.

### 6.3 Đường thi công dự kiến (chưa làm)

1. **Client** — `UiWorldMap.cpp`: đọc `MapList.ini` một lần, dựng bảng 224 mục
   `{mã map, x, y, tên Việt, loại}`; vẽ nhãn tiếng Việt; `WM_MOUSEMOVE` dò trúng → đổi con trỏ +
   hiện chú thích; `WM_LBUTTONUP` trúng địa điểm → **gửi yêu cầu**, không đóng cửa sổ
   (đúng như 2.0 trả 0 để giữ cửa sổ).
   ⚠️ Hiện `WndProc` đang **đóng cửa sổ với mọi cú bấm** (`UiWorldMap.cpp:200-206`) — phải tách nhánh.
2. **Vận chuyển** — dùng `KScriptProtocol` sẵn có (`LuaSendScriptDataToServer`), **không thêm gói mới**
   ⇒ không đụng Gate 2 (cấm đổi giao thức).
3. **Máy chủ** — một hàm Lua kiểm đủ luật ở 6.2 rồi gọi `NewWorld(map,x,y)`; dùng lại
   `station.lua` để không viết lại luật tiền/đã-đi-qua.
4. **Chữ Việt**: mọi chuỗi theo **RULE 0** — TCVN3 thô, sửa bằng `vn_edit.py`; nhớ **1 dấu cách
   trước `<color…>`**; TCVN3 **không có nguyên âm hoa có dấu**.

---

## 7. 🐛 LỖI PHÁT HIỆN DỌC ĐƯỜNG (chưa sửa, chờ chủ)

**`KUiMapCave::Initialize()`** — `Sources\S3Client\Ui\UiCase\UiMapCave.cpp:76-78`

```cpp
g_UiBase.GetCurSchemePath(szBuffer, sizeof(szBuffer));
sprintf(szBuffer, "\\%s", SCHEME_INI_WORLD);   // GHI ĐÈ, đáng lẽ phải NỐI THÊM
KIniFile Ini;
if (Ini.Load(szBuffer))                        // nạp "\UiMapCave.ini"
```

`GetCurSchemePath` vừa lấy xong đường dẫn giao diện thì bị `sprintf` **ghi đè** ⇒ nạp `\UiMapCave.ini`.

**Đã kiểm chứng: `\UiMapCave.ini` KHÔNG tồn tại** — không có trên đĩa, không có trong cả **46 pak**
của client (189.913 uid). Tệp thật là `\Ui\Ui3\UiMapCave.ini` (có trên đĩa **và** trong
`sprgame.pak`, `sprvlngaothe.pak`, `vlngaothe1.pak`).

**Hậu quả**: cửa sổ bản đồ sơn động **không bao giờ nhận được** `Left/Top/Width/Height` (23,16,754,578)
và ký hiệu `[Sign]` (mũi tên "ngươi ở đây") **không bao giờ được khởi tạo ⇒ không bao giờ hiện**.
Ảnh vẫn hiện vì `UpdateData()` tự đặt ảnh từ `MapList.ini`.

So sánh: `KUiWorldmap::Initialize()` (`UiWorldMap.cpp:135-138`) làm **đúng** bằng `strcat`.

Sửa 1 dòng là xong, nhưng **đây là thay đổi hành vi hiển thị ⇒ chờ chủ đồng ý** (Gate 5).

---

## 8. BẢNG TÓM TẮT ĐỐI CHIẾU

| Hạng mục | Bản 2.0 | Dự án JX1 | Ghi chú |
|---|---|---|---|
| Lớp cửa sổ bản đồ thế giới | có | có | **cấu trúc giống hệt** |
| Ảnh bản đồ 752×576 | có (chữ Hán) | có (chữ Hán) | thay được cho nhau |
| 8 ký hiệu loại map (.spr) | có | có | **giống hệt** |
| `MapList.ini` toạ độ + tên | 243 vị trí / 1.054 tên | 224 vị trí / **1.006 tên Việt** | |
| `MapTraffic.ini` cửa nối map | 498 / 406 map | **493 / 389 map** | gần như ngang nhau |
| Bấm tên thành → chú thích | **có** | không (chỉ hiện nhãn bang chiếm) | |
| Bấm bản đồ thế giới → nhảy map | **KHÔNG** | không | *không bản nào có* |
| Nút "Tìm đường" trên tiểu bản đồ | **có** | **không** | grep `WayFinding` = 0 |
| Bấm tiểu bản đồ → tự chạy | **có** (xuyên map) | bị chú thích | `UiMiniMap.cpp:263-292` |
| `DoDirectMap` (đi qua cửa nối) | có | **hàm rỗng** | `ScenePlaceMapC.cpp:1381` |
| Dịch chuyển thành thị | (Xa Phu) | **có, đang chạy** | `station.lua`, 16 bến, có giá |
| Kênh script client↔server | có | **có** | `KScriptProtocol` (làm cho hệ thư) |
| Bản đồ tiếng Việt | ✅ **CÓ** (`*_vn.spr`) | ✗ (đang trỏ bản Hán) | **thay ảnh là xong, không sửa mã** |

---

## 9. CÂU HỎI CHỜ CHỦ CHỐT

1. ✅ **Bản đồ tiếng Việt — làm được NGAY, chỉ chờ chủ chọn ảnh** (xem 4 ảnh đã gửi kèm):
   - thế giới: `43841ADC` (cặp `_vn` chính thức, có Vũ Hồn) hay `A7A36384` (tiêu đề vàng, la bàn Việt)?
   - sơn động: `88D08A38` (cặp `_vn` chính thức) hay `371420E4` (đẹp và rõ nhất)?
   - Cách áp: bơm 2 ảnh vào pak client rồi **đổi 2 dòng `WorldMapImage=` / `CaveMapImage=`
     trong `settings\MapList.ini`** sang tên `_vn` — **không sửa mã, không build, không swap DLL.**
2. **Chọn mức nào cho "kích để đi"**: A (dịch chuyển thẳng) / **B (kiểu Xa Phu — đề nghị)** /
   C (tìm đường tự chạy như 2.0)?
3. **Có vẽ thêm nhãn tiếng Việt lúc chạy** từ `MapList.ini` không (224 điểm)? — nên có, vì
   dùng lại đúng dữ liệu đó để dò chuột cho việc bấm-để-đi.
4. **Giới hạn**: đồng ý bộ luật ở mục 6.2 chứ? Có thu tiền không, bao nhiêu, có thời gian chờ không?
   **Bấm được những đâu**: chỉ 5 thành + 2 kinh đô, hay cả 224 địa điểm có toạ độ?
5. **Lỗi mục 7** (`UiMapCave.ini` không bao giờ nạp được): sửa luôn hay để riêng?

---

---

## 10. ✅ THI CÔNG ĐỢT 1 (06/09 tối) — ĐÃ ĐỔI SANG BẢN ĐỒ 2.0, GIỮ NGUYÊN BANG HỘI CHIẾM LĨNH

Chủ chốt: *"hãy đổi qua bản đồ 2.0 và giữ nguyên bang hội chiếm lĩnh của dự án"*.
**Chỉ đổi tài nguyên + 2 dòng ini. KHÔNG sửa mã, KHÔNG build, KHÔNG swap DLL.**

### 10.1 Đã làm gì (cây chạy thật `E:\...\TESTLOFFF_ONLINE\bin\client\`)

| # | Việc | Chi tiết |
|---|---|---|
| 1 | Đặt 2 ảnh `_vn` của 2.0 lên **đĩa** | `Spr\Ui3\小地图\剑侠大地图2_vn.spr` (thế giới, uid `43841ADC`, md5 `c426ce363051`) và `打怪向导山洞图_vn.spr` (sơn động, uid `88D08A38`, md5 `dfaa2191f693`), mỗi tệp 437.424 B |
| 2 | Sửa `settings\MapList.ini` | dòng 3–4: `CaveMapImage=` / `WorldMapImage=` thêm `_vn` trước `.spr`. Sao lưu: `MapList.ini.truoc_bando20_0609`. High-byte 23.423 → 23.423, FFFD 0, CRLF nguyên |
| 3 | Gương git | `clientdata_jx2\settings\MapList.ini` (đồng bộ lại từ cây sống — gương cũ đã lệch từ 14/08, kéo theo cả sửa VHTD 02/09) + `clientdata_jx2\Spr\Ui3\<小地图>\*.spr` + `clientdata_jx2\Spr\README.md` |
| 4 | Công cụ | `ReverseTools\pak_vltk\vltk2\bando_vn\`: `apply_bando.py` (chạy lại được), `spr_v2.py`, `bigspr.py`, `findpaths.py`, `cp_check.py`, `preview_final.py`, 4 ảnh VN `.spr` + `.png` |

**Vì sao đặt trên đĩa mà client thấy:** `KPakFile::Open` chế độ 0 = **đọc ĐĨA trước, pak sau**
(`Engine\Src\KPakFile.cpp:241`; `m_nPakFileMode` mặc định 0, không ai gọi `g_SetPakFileMode`);
`KFile::Open` đi qua `g_GetFullPath` nên `\Spr\...` được hiểu từ gốc client (`KFile.cpp:55`).
Ảnh SPR nạp qua `KImageRes` → `KPakFile` ⇒ cùng cơ chế với `\Ui\Ui3\UiWorldMap.ini` đang đè pak.

**Tên thư mục GBK trên đĩa:** client là ứng dụng ANSI (cp1252) ⇒ Windows lưu tên = byte GBK đọc theo
cp1252 (`小地图` → `Ð¡µØÍ¼`). Đã **chứng minh** bằng 4 thư mục có sẵn (`NPC对话条`→`NPC¶Ô»°Ìõ`…, 4/4 tồn tại)
trước khi tạo. Không byte nào rơi vào ô trống của cp1252.

**Engine đọc được bản `_vn`:** `SPRHEAD` = **32 byte** (`KSprite.h:15-27`, có 6 WORD dự trữ) ⇒ bảng màu ở
offset 32 — đúng như bộ giải đã sửa; 28 byte tiêu đề + khung tại 804 của bản `_vn` **giống byte-một**
bản chữ Hán đang chạy ⇒ vẽ y hệt.

### 10.2 Bang hội chiếm lĩnh — GIỮ NGUYÊN, đã kiểm hình học

7 nhãn `KWndPureTextBtn` vẫn do mã vẽ (`UiWorldMap.cpp` `RefreshCityLabels`), **không đụng
`Ui\Ui3\UiWorldMap.ini`**. Đo từ ảnh chụp thật của chủ: nhãn **căn giữa** nút 102 px
(Phượng Tường: x 92–345 màn hình = tâm 198 ± 127), chữ thấp hơn `BtnStartPos.y` ≈ 5 px.
Vẽ thử đúng hình học đó lên ảnh `_vn`: nhãn chỉ **chạm mép 0–4 px** tên thành tiếng Việt (Biện Kinh,
Tương Dương, Thành Đô) — **giống hệt mức chạm trên bản chữ Hán hiện tại** ⇒ không nhích.
Ảnh xem trước: `scratchpad\giao_vn\XEMTRUOC_world_VN.png` (đã gửi chủ). Muốn nhích nhãn nào thì sửa
`BtnStartPos` của thành đó trong `Ui\Ui3\UiWorldMap.ini` (1 dòng, không build).

### 10.3 Để thấy kết quả · Lùi

- **Không cần khởi động lại**: `UpdateData()` nạp lại `MapList.ini` mỗi lần mở cửa sổ; đường dẫn `_vn`
  chưa từng nằm trong cache ảnh. Bấm nút *bản đồ thế giới* / *bản đồ sơn động* trên tiểu bản đồ là thấy.
  Nếu vẫn thấy chữ Hán ⇒ khởi động lại client.
- **Lùi trong 10 giây**: chép `MapList.ini.truoc_bando20_0609` đè lại `MapList.ini` (bản chữ Hán vẫn
  nằm trong pak, không mất gì). Hoặc chạy lại `apply_bando.py` để áp lại.
- **Chưa làm** (chờ chủ): kích-để-đi (mục 6), nhãn vẽ lúc chạy (5.4), lỗi `UiMapCave.ini` (mục 7 —
  cửa sổ sơn động vẫn hiện ảnh mới bình thường vì `UpdateData` tự đặt ảnh).

*Đợt 1 xong: đã đổi ảnh, chưa build, chưa đặt `.moi`. Còn chờ chủ chốt câu 2–5 ở mục 9.*
