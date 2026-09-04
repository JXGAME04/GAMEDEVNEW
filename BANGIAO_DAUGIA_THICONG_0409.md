# THI CÔNG ĐẤU GIÁ — 04/09/2026 (chủ chốt: tiền **xu + lượng**, làm lần lượt **A → B → C**)

Thiết kế và bằng chứng mổ client: `BANGIAO_DAUGIA_CHIENLENH_0409.md`.
Mức A = ký gửi cá nhân · B = đấu giá toàn server · C = đấu giá bang hội.

---

## Đợt A1 (xong 04/09 ~01:00) — NỀN MÁY CHỦ

Chỉ máy chủ, **chưa nối vào luồng chơi** nên không đổi gì với người chơi. Mục đích: dựng đúng chỗ khó nhất trước.

### 1. Bảng MySQL `auction_item` (tự tạo lần dùng đầu, cùng DB với bảng `mail`)

| Cột | Ý nghĩa |
|---|---|
| `atype` | 1 bang hội · 2 thế giới · 3 cá nhân (ký gửi) |
| `activity` | tên phiên (cá nhân để trống) |
| `kind` | 1 kiểu Anh (giá tăng) · 2 kiểu Hà Lan (giá giảm) |
| `seller` / `seller_tong` | người bán (byte TCVN3) / bang của họ |
| `item_name` / `item_desc` / `item_rec` / `item_cells` | tên · "g,d,p,l,s,k" · **bản ghi hex giữ nguyên món đồ** · số ô hành trang |
| `currency` | **1 Ngân lượng · 2 Xu** (chủ chốt 04/09) |
| `base_price` `cur_price` `guaranteed_price` `buy_price` `deposit` | các mốc giá + phí ký gửi |
| `buyer` | người mua / người trả giá cao nhất |
| `start_time` `end_time` `next_drop_time` `drop_left` | mốc thời gian + số lần giảm giá còn lại |
| `state` | 0 đang bán · 1 đã bán chờ giao · 2 hết hạn/lưu phách · 3 xong |

### 2. Giữ nguyên món đồ — phần khó nhất, đã giải xong

Hộp thư tạo lại đồ bằng `AddItem` nên **trang bị bị đổi thuộc tính ngẫu nhiên**. Đấu giá không được phép thế: người mua phải nhận
đúng món người bán ký gửi.

`KAuctionServer.cpp` lưu đúng bộ tham số mà đường lưu/nạp cơ sở dữ liệu của nhân vật dùng (`KPlayerDBFuns.cpp`), rồi tạo lại bằng
**chính các hàm sinh đồ mà đường nạp đó gọi**: `Gen_ExistEquipment` / `GetGoldItemByIndex` / `Gen_Medicine` / `Gen_Quest` /
`Gen_MagicScript` / `Gen_Fusion` / `Gen_StarStone` / `Gen_TownPortal`. Nhờ vậy món đồ quay lại y hệt: cùng seed, may mắn, độ bền,
khoá, hạn dùng, dung luyện, phi phong, ngoại trang phát sáng, giá bày bán.

🔴 Ràng buộc bảo trì: nếu `KPlayerDBFuns.cpp` đổi cách nạp thì **phải sửa theo ở `KAuctionServer.cpp`** (đã ghi chú ở đầu tệp).
Quặng và nguyên liệu (`item_mine`, `item_materials`) bản gốc cũng không tạo lại được ⇒ **từ chối ký gửi**.

### 3. 12 hàm Lua mới (máy chủ)

`AUC_ItemToRec(nItemIdx)` → hex, tên, mô tả, số ô · `AUC_RecName(hex)` · `AUC_RecCells(hex)` · `AUC_GiveRec(hex)` → tạo lại đúng
món cho người chơi hiện tại (hết chỗ thì **không** vứt xuống đất, trả 0 để script giữ lại trong kho) ·
`AUC_Ready` `AUC_PutOn` `AUC_List` `AUC_Get` `AUC_Buy` (nguyên tử, chỉ một người mua được) `AUC_SetState` `AUC_Sweep` `AUC_CountSeller`.

### 4. Binary

`bin\server\CoreServer.dll.moi` = **e8d65505** (18.323.968). Gồm: nền đấu giá + phát lương bang hội qua thư + vá BroadCast
(wauto-ca 61e9c0c7) + S13 (wauto-75) + Tống Kim (wauto-c0). Đã kiểm nhị phân đủ các dấu `auction_item` `bangluong` `BroadCastTam`
`BC-TUVUNG` `S13-DENY-GIUCHAY` `TKDich`. Build sạch cả hai cấu hình máy chủ x64 và client Win32.

---

## Việc còn lại

| Đợt | Nội dung | Ghi chú |
|---|---|---|
| **A2** | `auction_manager.lua` (ký gửi, mua, hết hạn trả về) + `auction_def.lua` (tham số) + giao/trả qua **thư** + NPC tạm để thử | chỉ script, không cần build |
| **A3** | Cửa sổ đấu giá thật: 13 lớp C++ `KUiAuction*` + 21 hàm Lua + thả 13 ini và `uiauction_house.lua` (đã rút từ client cũ) | cần build client — **chờ phiên khác xong đợt chữa sập** |
| **B** | Phiên đấu giá toàn server theo giờ (kiểu Anh + Hà Lan), thông báo toàn server | |
| **C** | Phiên bang hội + danh sách thành viên, nối với phát lương bang hội | |

Tham số đề xuất (theo bản gốc 2.0, chủ chỉnh được): thuế 5%, phí ký gửi 10%, 20 món/trang, một người ký gửi tối đa 5 món,
ký gửi cá nhân 24 giờ giảm giá 1 lần, kiểu Anh 30 phút mỗi lần trả giá gia hạn 60 giây.
