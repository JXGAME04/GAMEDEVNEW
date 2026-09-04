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

---

## Đợt A2 + A3 + B + C (04/09 ~01:00–02:30) — TRỌN BỘ, chờ chủ test một lần

Chủ 04/09: "làm xong toàn bộ rồi test 1 lần". Đã làm cả ba mức trên cùng một cửa sổ (3 tab Bang hội / Thế giới / Cá nhân).

### 1. Máy chủ (script; C++ chỉ thêm 2 hàm so với A1)
| Tệp | Vai trò |
|---|---|
| `script\auction_house\auction_def.lua` | tham số: thuế 5 %, phí ký gửi 10 %, 20 món/trang, tối đa 5 món/người, ký gửi 24 h, kiểu Anh 30 phút (+60 s mỗi lần trả), Hà Lan 8 lần × 10 % mỗi 5 phút; tiền 1 = Ngân lượng, 2 = Xu (ô 251) |
| `script\auction_house\auction_manager.lua` | bộ máy: 8 yêu cầu từ client, phát 12 sự kiện, chốt giao dịch `AUC_Settle`, hết hạn `AUC_Expire`, quét `AUC_Tick`. Giao/trả **qua thư** (`aucitem:<id>` → `AUC_GiveRec`, giữ nguyên thuộc tính). Tiền bán về người bán qua thư trừ thuế; phiên bang → **quỹ bang** (`TONG_ApplyAddMoney`); phiên thế giới (GM) → tiền vào hệ thống |
| `script\auction_house\auction_npc.lua` | menu NPC **Dịch Quán → "Đấu giá"**: ký gửi (chọn tiền → nhập giá `OpenGetNumber` → đặt món vào hộp `GiveItemUI` → trừ phí → vào kho); Bang chủ đưa món vào phiên bang (Anh/Hà Lan × Ngân lượng/Xu); "Mở cửa sổ đấu giá". **Lệnh bài admin** → "Đấu giá: mở phiên thế giới (GM)" |
| `script\auction_house\auctionpoll.lua` | timer 30 s (`AddTimer`, GlbValue 9002): giảm giá Hà Lan, kết thúc kiểu Anh, hết hạn ký gửi → thư trả người bán, báo kết thúc phiên |
| `protocol.lua` (+20 tên), `protocol_def_gs.lua` (+8), `mail\mailmanager.lua` (đính kèm `aucitem:`) | |
| `KAuctionServer.cpp` | thêm `AUC_SetPrice` (giảm giá) + `AUC_Bid` (trả giá nguyên tử: chỉ khi `cur_price < giá mới`) |

### 2. Client
| Tệp | Vai trò |
|---|---|
| `Core\Src\KAuctionUiDef.h` | hợp đồng Core ↔ Game.exe: `AUCUI_CMD_*` (19), `AUCUI_OP_*` (14), `KAucUiItem` / `KAucUiActivity` / `KAucUiMember` / `KAucUiReq` |
| `Core\Src\KAuctionClient.{h,cpp}` | 21 hàm Lua giống 2.0 + `SetAuctionIconVisible`, `AuctionSetMoney`, `GetLocalTime`, `PopBlackTips`; `AuctionUi_OnRequest` → `UIAuctionHouse:xxx` qua `SP_RunClientLua` |
| `S3Client\Ui\UiCase\UiAuction.{h,cpp}` | `KUiAuctionManager` (3 tab + đóng) · `KUiAuctionPage` (danh sách phiên cuộn, 3 hàng vật phẩm 118 px cuộn, trang trước/sau, số Ngân lượng/Xu, nút xem thành viên) · `KUiAuctionItemRow` (hai bộ widget Anh/Hà Lan, icon vật phẩm thật qua `GDI_ITEM_CHAT`, nút + / Báo giá / Huỷ / Ta muốn đấu giá / Lấy lại, đếm ngược mỗi giây) · `KUiAuctionMemberWnd` · `KUiAuctionIcon` (ngay dưới biểu tượng thư, nhấp nháy khi có món mới) |
| `CoreShell.h/.cpp`, `ScriptFuns.cpp`, `GameSpaceChangedNotify.cpp`, 2 vcxproj | `GDCNI_AUCTION_UI` / `GOI_AUCTION_UI` (cuối enum), đăng ký 25 hàm, 3 móc GAME_START / EXIT / cmd |
| `script\ui\uiauction_house.lua`, `script\auction_house\auction_def.lua`, `protocol_def_c.lua` (+12), `protocol.lua` | script client viết lại từ bản 2.0 (vật phẩm = 6 số + số lượng + tên + tên tiền; bỏ hộp xác nhận) |
| `ui\Ui3\auction\*.ini` (13) | rút từ client JX1 cũ; `Image=` thêm `\` đầu; biểu tượng Left=765 Top=322; nút "Báo giá" đổi sang sprite 4 chữ (sprite 2 chữ không có trong pak dự án) |

🔴 Bẫy đã gặp: `wnds.h` không có include guard → `UiAuction.h` include từng header elem như `UiMail.h`; `g_pCoreShell` phải `extern` tự khai (như `UiPartnerCommon.h`); `KWndWindow::Breathe` là private → không gọi hàm lớp cha.

### 3. Phối hợp binary (04/09 01:30–02:30)
- Máy chủ: bản đang chạy 72348ac7 là **broadcast-0309** của wauto-ca (main chưa có) ⇒ gộp `origin/broadcast-0309` c6ca1f55 rồi build: **`CoreServer.dll.moi` = 3bda2f1a** (18.327.040), đủ dấu `auction_item` `AUC_Bid` `bangluong` `BroadCastTam` `BC-TUVUNG` `S13-DENY-GIUCHAY` `TKDich`. Cần `heaven.dll` a793834b đang chạy (wauto-ca đã thêm `:capnhat heaven.dll` vào bat).
- Client: bản đang chạy 16c0d5ca là **net-0309** (acchinh + 3 vá kiểm biên, main chưa có) ⇒ gộp `origin/net-0309` b0a05751 rồi Rebuild CoreClient + Game.exe; kiểm `BIEN-XAU` trước khi đặt; đặt **cả hai cùng lúc** (đổi enum CoreShell.h).
- `origin/main` chỉ nhận các commit đấu giá (cherry-pick, badbd14f), không kéo nhánh của người khác.

### 4. Cách test một lần (chủ)
1. `ChayGameServer.bat` (nạp CoreServer 3bda2f1a) rồi `ChoiGame.bat` (nạp CoreClient + Game.exe .moi).
2. Vào game: có **biểu tượng đấu giá** ngay dưới biểu tượng thư (góc phải). Bấm → cửa sổ 3 tab; tab Cá nhân có phiên "Ký gửi".
3. Ký gửi: Dịch Quán → Đấu giá → "Ký gửi vật phẩm (Ngân lượng)" → nhập giá → đặt 1 món (không khoá) vào hộp → OK. Nhận thông báo mã số; trừ 10 % phí; món biến mất khỏi túi; tab Cá nhân hiện món (người khác thấy "Ta muốn đấu giá", chính mình thấy "Lấy lại").
4. Mua bằng nhân vật khác: bấm "Ta muốn đấu giá" → trừ tiền → hộp thư tự mở với thư "Đấu giá thành công" đính kèm đúng món (thuộc tính y nguyên); người bán nhận thư "Tiền bán ký gửi" (giá − 5 %).
5. Lấy lại: người bán bấm "Lấy lại" → thư "Rút vật phẩm ký gửi".
6. Phiên bang: Bang chủ ở Dịch Quán → "Bang chủ: đưa vật phẩm vào phiên đấu giá bang" → Kiểu Anh / Ngân lượng → giá khởi điểm → đặt món. Thành viên mở tab Bang hội → phiên tên bang → bấm + rồi "Báo giá"; người bị vượt giá nhận thư hoàn tiền; sau 30 phút (gia hạn 60 s mỗi lần trả) món về người thắng qua thư, tiền vào quỹ bang. "Xem danh sách thành viên tham gia" mở bảng thành viên.
7. Phiên thế giới: lệnh bài admin → "Đấu giá: mở phiên thế giới (GM)" → Kiểu Hà Lan → giá → đặt món: giá mở 150 %, giảm 10 % mỗi 5 phút, ai bấm mua trước được.
8. Nhật ký: `logs\hethong.log` dòng `[DAUGIA]` (đặt bán / bán / hết hạn / lỗi) và `[MAIL]`; client `jx_mail.log` dòng `[AUC]`; lỗi script `ScriptError.log`.


---

## 9. Đợt A13-A14 + B1-B4 (04/09 03:42) — ba việc chủ báo + nhóm MẤT ĐỒ

Binary chờ swap (ba tệp PHẢI cùng lúc; bản này gộp cả `[VATCAN]` `g_nPbNpcChan = 1` của phiên khác, biến đó
biên vào CẢ HAI phía nên lệch là dồn dần rồi dăn một phát):

| Tệp | md5 | Cỡ |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | `cb9712ce12cae7396fbe1c876f3bd9b6` | 18.330.624 |
| `bin\client\CoreClient.dll.moi` | `fdca41ad50cfcdc83ef2b7aeb6ac4cd6` | 2.540.544 |
| `bin\client\Game.exe.moi` | `09f0a0f0d09751c3d25d3748643b6a34` | 1.436.672 |

Commit main: `9037da0a`.

### 9a. "bấm vào Ta muốn đấu giá không hiện gì cả" (A13)
`KWndButton` chỉ báo `WND_N_BUTTON_CLICK` cho **cha trực tiếp**. Nút nằm trong một hàng vật phẩm nên thông báo
dừng lại ở lớp hàng — lớp đó không có `WndProc` — và không bao giờ tới trang để gửi lên máy chủ. Nút *Ký gửi* /
*Trang trước* / *Trang kế* là con trực tiếp của trang nên vẫn chạy, đúng như chủ thấy.
Nay hai lớp hàng kế thừa `KUiAuctionScrollWnd` (lớp sẵn có việc chuyển tiếp lên cha).

### 9b. "các chữ chồng lên nhau" (A14)
Khung một hàng rộng **429**. Bản gốc để nhãn `Width=120` nhưng hai cột chỉ cách nhau **86**: `86+120 = 206` đè lên
giá trị ở `172`, và `258+120 = 378` đè lên giá trị ở `344`; cột phải còn tràn khỏi khung (`344+120 = 464`).
Lưới mới: **nhãn 86** (86..172 và 258..344), **giá trị 84** (172..256 và 344..428). Nhãn cũng rút gọn cho vừa:
*Mua ngay · Giá sàn · Loại tiền · Hạ giá sau · Giá kế tiếp · Hết hạn sau* (Hà Lan) và
*Khởi điểm · Bước giá · Loại tiền · Cao nhất · Giá của ta · Còn lại* (kiểu Anh).

### 9c. "3 item trong bản thì bị chèn ra ngoài bản" (A14)
Khung danh sách cao **321**, mỗi hàng cao **118** → ba hàng là **354**, thừa 33 px ra ngoài. Khung không cao thêm
được (ngay dưới nó là nút chuyển trang ở `Top=374`). Nay **2 hàng/trang** và mỗi trang đúng **2 món**
(`AUCUI_ROW_COUNT` = `nMaxItemPerPage` = 2) để số món mỗi trang khớp số hàng thấy được.

### 9d. "giá mua ngay 200 xu mà giá cơ bản cũng 200 xu vậy sao đấu giá?" (A14) — LUẬT ĐẤU GIÁ
Đúng: ký gửi cá nhân đang đặt `base = cur = sàn = ` đúng giá người bán nhập nên không còn gì để đấu.
Bản gốc 2.0 để `nPersonalFloatTimes = 1`, tức **ký gửi cũng là đấu giá kiểu Hà Lan, một nhịp**. Nay làm đúng vậy:

| | Ký gửi cá nhân | Phiên thế giới / bang hội (Hà Lan) | Phiên kiểu Anh |
|---|---|---|---|
| Giá mở bán | **150 %** giá người bán nhập | 150 % giá bảo đảm | = giá khởi điểm |
| Hạ giá | **một nhịp**, sau 12 h hạ **thẳng** về giá người bán | −10 % mỗi 5 phút, 8 nhịp | không hạ, chỉ tăng |
| Sàn | giá người bán nhập | giá bảo đảm | — |
| Hết hạn | 24 h → trả món qua thư | hết nhịp → trả món | 30 phút, mỗi lần trả gia hạn 60 s |
| Cách mua | bấm *Ta muốn đấu giá* = mua ngay theo giá hiện tại | như trên | bấm `+` rồi *Báo giá* |

Nghĩa là: **ai muốn chắc thì mua ngay giá cao, ai chịu chờ thì mua giá gốc** — đó là chỗ "đấu" của kiểu Hà Lan.
Ô *Mua ngay* = giá hiện tại, ô *Giá sàn* = giá người bán muốn, ô *Giá kế tiếp* báo đúng giá vòng quét sẽ hạ tới.
Kèm một lỗi gốc: **vòng quét trước chỉ hạ giá phiên thế giới + bang hội**, nên dù có đặt mốc hạ thì ký gửi cũng
không bao giờ hạ. Đã thêm khối quét riêng cho ký gửi (để riêng vì vòng kia còn lo báo *kết thúc phiên*, mà tab
Cá nhân là chợ thường trực, không có phiên nào để kết thúc).

### 9e. Nhóm MẤT ĐỒ (đội soát 54 tác tử)
- **B1 (C++)** `sCanRebuild` chặn ngay ở đường ký gửi những loại `sRecToItem` không tạo lại được (quặng, nguyên
  liệu, trang bị hỏng) — trước đây món bị xoá khỏi túi rồi lúc trả lại mới lộ ra là không dựng lại được.
  `AUC_GiveRec` kiểm kết quả `AddKIL`. Thêm `AUC_CanGiveRec` (thử đặt thật, đòi **khối liền** WxH) và
  `AUC_Rollback` (trả dòng về đang bán **và xoá người mua**).
- **B2 (Lua)** cấm ký gửi món có hạn dùng (hạn là mốc tuyệt đối, món chết trong kho); xoá món khỏi túi **trước**
  khi ghi kho, ghi hỏng thì trả lại ngay; **mua ngay chỉ áp cho dòng Hà Lan** — trước đây có thể cướp trắng dòng
  kiểu Anh và người trả giá cao nhất mất trắng tiền đã trừ; gửi thư **trước** rồi mới đổi trạng thái.
- **B3** timer quét đặt ngay trong `auction_manager.lua`: `Include` = `lua_dofile` vào chính state gọi, để tệp
  riêng thì `AUC_Tick` chạy ở state khác state giữ `AUC_Viewers` nên **mọi thông báo tự động không tới ai**.
  Người xem lưu thêm tên + bang để lọc đúng bang.
- **B4 (hộp thư)** `CalcFreeItemCellCount` đếm **ô rời rạc** còn đặt đồ đòi **khối liền**, mà thư đã đánh dấu
  ĐÃ NHẬN (nguyên tử) **trước** khi trao → mất món vĩnh viễn. Nay thử đặt thật trước; vẫn hỏng thì gửi lại món
  bằng một thư mới thay vì chỉ ghi log.

### 9f. Cách kiểm nhanh sau khi swap
1. Ký gửi một món giá 200 xu → tab Cá nhân phải hiện **Mua ngay 300**, **Giá sàn 200**, **Giá kế tiếp 200**.
2. Chữ trong hàng không đè nhau, không tràn khỏi khung; một trang đúng 2 món, món thứ ba ở trang sau.
3. Nhân vật khác bấm *Ta muốn đấu giá* → trừ 300 xu, nhận thư đính kèm đúng món.
4. Dọn túi cho chật (chỉ còn ô rời) rồi nhận thư → phải báo "không còn khoảng trống liền", thư **vẫn chưa nhận**.
