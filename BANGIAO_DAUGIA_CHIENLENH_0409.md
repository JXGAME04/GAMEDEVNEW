# MỔ CLIENT VLTK 2.0 — ĐẤU GIÁ & CHIẾN LỆNH (04/09/2026)

Chủ yêu cầu: **chỉ phân tích và xác định phương hướng, chưa làm.** Tài liệu này là kết quả mổ nhị phân + rút script.

Cây mổ: `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0` (client JX2 bản VNG 2021+) và
`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky` (client JX1 cũ — cùng dòng với client dự án).
Vật liệu đã rút để dùng lại: `ReverseTools\pak_vltk\vltk2\` (công cụ) + thư mục tạm `vltk2_dg\` (script + ini đã giải nén).

---

## 1. Kết luận nhanh

| Tính năng | Có trong 2.0? | Có trong client JX1 cũ? | Có trong dự án ta? | Mức khó port |
|---|---|---|---|---|
| **Đấu giá** (Auction House) | **CÓ, đầy đủ** — 13 lớp C++, 21 hàm Lua, 19 giao thức, script 27 KB, 13 ini | **CÓ y hệt** (cả ini + script + sprite) | KHÔNG (0 dấu vết) | **Trung bình** — nền vận chuyển đã xong |
| **Chiến lệnh** (Season Ticket 2026) | **CÓ** — nhưng **thuần Lua**, 4 giao thức, script 32 KB | KHÔNG (chỉ có khung UI Lua) | KHÔNG | **Khó hơn** — cần cả khung UI động |

Điểm mấu chốt: **kênh vận chuyển ScriptProtocol/ObjBuffer mà cả hai tính năng dùng thì ta ĐÃ LÀM XONG ở hệ thư (đợt 1 + đợt 6)**,
kể cả chỗ khó nhất là truyền **bảng** qua `DynamicExecute` (vá D6). Đấu giá dùng đúng kênh đó và dùng luôn hệ thư để trả tiền/đồ.

---

## 2. ĐẤU GIÁ — bằng chứng

### 2.1 Nhị phân
`gamecl.exe` (giải UPX NRV2E, ảnh 34,5 MB) và `game_y.exe` (JX1 cũ, không nén) đều có:

- **13 lớp UI (RTTI)**: `KUiAuctionManager` (cửa sổ chính) · `KUiAuctionBase` · `KUiAuctionTong` / `KUiAuctionWorld` / `KUiAuctionPersonal` (3 tab)
  · `KUiAuctionItemList` · `KUiAuctionItemBase` · `KUiAuctionItemHeaderEnglish` / `KUiAuctionItemHeaderDutch` (2 kiểu đấu)
  · `KUiAuctionActivityList` / `KUiAuctionActivityHeader` (danh sách phiên) · `KUiAuctionMemberList` / `KUiAuctionMemberHeader` / `KUiAuctionMemberListInfo`
  (danh sách thành viên bang + lương). Client JX1 cũ có thêm `KUIAuctionHouseIcon`.
- **21 hàm C++ xuất cho Lua**: `OpenAuctionWindow CloseAuctionWindow SwitchAuctionWindow AuctionUiIsOpen NewAuctionEventArrival
  AuctionAddActivity AuctionDelActivity AuctionClearActivityList AuctionOnActivitySelect AuctionAddEnglishItem AuctionAddDutchItem
  AuctionSetEnglishItem AuctionSetDutchItem AuctionEndItem AuctionClearItemList AuctionAddActivityMember AuctionClearMemberList
  AuctionSetSalaryAndCount AuctionSetCurrentPageTxt AuctionClearAll`.
- **13 tệp ini**: `auction_manager / auction_icon / auction_page_{personal,tong,world} / auction_item_{english,dutch,personal,personal_myself}_header
  / auction_activity_header / auction_member_header / auction_memberlist_info / auction_item_icon`.

### 2.2 Script (đã rút được nguyên bản)
| Tệp | Nguồn | Cỡ |
|---|---|---|
| `\script\ui\uiauction_house.lua` | `slistcache.pak` (JX1 cũ) | 27.537 B, 591 dòng, 40 hàm |
| `\script\auction_house\auction_def.lua` | `slistcache.pak` | 6.036 B — **toàn bộ tham số + luật** |
| 13 ini `\ui\ui3\auction\*.ini` | `slistcache.pak` + `1024.pak` | đủ, đã có bản Việt hoá |
| `\script\script_protocol\protocol_def_c.lua` | cả 2 client | có sẵn 11 mục AUCTION |

**Chưa có**: `\script\auction_house\auction_manager.lua` (bộ máy phía MÁY CHỦ) — không nằm trong client nào, phải tự viết
(giống hệt tình huống `MailManager.lua` của hệ thư, ta đã viết lại thành công).

### 2.3 Mô hình chơi (đọc từ `auction_def.lua`)

**3 phạm vi**: Bang hội (1) · Thế giới (2) · Cá nhân (3). **2 kiểu đấu**: Anh (English, giá tăng) · Hà Lan (Dutch, giá giảm dần).

| Tham số | Giá trị gốc | Ý nghĩa |
|---|---|---|
| `nAuctionTaxRate` | 5 | thuế giao dịch 5% |
| `nBindingSilverTaskKey` / `nBindingGoldTaskKey` | 4361 / 4362 | 2 loại tiền (bạc khoá / kim khoá) — ô nhiệm vụ |
| `nMaxItemPerPage` | 20 | mỗi trang 20 món |
| `nMaxItemPlayerCanJoin` | 2 | một người tham gia tối đa 2 món cùng lúc |
| `nEnglishRemainingTime` | 30 phút | thời lượng đấu kiểu Anh |
| `nRefreshRemingTimeWhenOffer` | 60 giây | mỗi lần trả giá thì gia hạn 60 s |
| `nDutchInitRate` | 1,5 | giá mở = 150% giá bảo đảm |
| `nDutchFloatTimes` / `nDutchFloatRange` / `nDutchFloatInterval` | 8 / 10% / 5 phút | giảm 8 lần, mỗi lần 10%, cách 5 phút |
| `nPersonalFloatTimes` / `nPersonalFloatInterval` | 1 / 24 giờ | đấu giá cá nhân = ký gửi 24 giờ, giảm giá 1 lần |
| `nPersonalPutOnCost` | 10 (%) | phí đặt bán = 10% giá (dịp khuyến mãi 5%) |
| `nMaxBindingGoldCost` | 20000 | trần kim khoá cho một món |
| `nDailyPutOnCountTaskKey` | 3654 | ô đếm số món đặt bán mỗi ngày |

Sinh mã món đấu giá bằng `OB_SaveShareData/OB_LoadShareData` khoá `SDKEY_AUCTION_ITEMID_INCREASER` — **cơ chế này dự án ta ĐÃ CÓ** (`KJx2SharedStore`).

**Trả kết quả bằng THƯ**: `AUCTION_DEF:SendMail(...)` với người gửi `"Chưởng Quầy Khu Đấu Giá"` — đúng như tên người gửi đã thấy khi mổ hệ thư.
Nghĩa là **đấu giá bắt buộc phải có hệ thư trước**, và ta vừa làm xong hệ thư.

### 2.4 Hợp đồng giao thức (19 tên, chỉ số 127–146 trong `protocol.lua` 2.0)

**Client → Máy chủ (8)**: `REQUEST_OFFERENGLISHPRICE` · `REQUEST_OFFERDUTCHPRICE` · `REQUEST_REFUND` (hoàn tiền) ·
`REQUEST_ACTIVITYLIST` · `REQUEST_ACTIVITYCONTENT` · `REQUEST_ITEMCONTENT` · `REQUEST_MEMBERLIST` · `REQUEST_GETBACKITEM` (lấy lại đồ).

**Máy chủ → Client (11)**: `OFFERPRICERET` · `REFUNDRET` · `ACTIVITYLIST` · `ACTIVITYINFO` · `ITEMINFO` · `MEMBERLIST` ·
`NEWACTIVITY` · `ENDACTIVITY` · `NEWITEM` · `ENDITEM` · `TPRICECHANGE`.

**4 trong số đó truyền BẢNG (`OBJTYPE_TABLE`)** → phải có bản vá D6 của ta (chuyển tay cầm ObjBuffer sang state đích rồi mới bóc), nếu không sẽ nhận `nil` y như lỗi hộp thư hôm qua.

### 2.5 Thế hệ CŨ hơn: đấu giá theo THÀNH
Trong `update01.pak` (cả hai client) còn một tệp ini cấu hình của **bản đấu giá đời cũ do C++ máy chủ chạy**:
`MinBasePrice=1000000`, `AddPricePerTime=10000`, `MaxPrearrangeRounds=5`, `FeePerRound=10000`, `ItemsPerRound=4`, `TaxRate=12`,
`CountDown=30`, `MaxAuctionPerCity=32`, `MaxAuctionPerPlayer=3`, `MaxBidderPerItem=10`, `ResultKeepTime=3600`, `AuctionTime=5`.
Chuỗi `MaxAuctionPerCity` và `AuctionTime` cũng nằm trong `game_y.exe`. Đây là **hai hệ khác nhau**; ta nên đi theo hệ mới (script) chứ không phải hệ này.

### 2.6 Ta đã có gì / thiếu gì

| Hạng mục | Trạng thái ở dự án |
|---|---|
| Kênh ScriptProtocol + ObjBuffer 2 chiều | **XONG** (hệ thư đợt 1) |
| Truyền tham số bảng sang state đích | **XONG** (D6) |
| Hệ thư để trả tiền/đồ | **XONG** (đợt 1–9) |
| `OB_SaveShareData/LoadShareData` (sinh id) | **CÓ SẴN** |
| Lưu trữ MySQL + mẫu `KMailServer.cpp` | **CÓ SẴN** |
| 13 ini giao diện | **CÓ** (rút từ client JX1 cũ) |
| Sprite | **19/21 đã có sẵn trong pak dự án**, gồm cả thư mục `拍卖行` và 2 con dấu Việt hoá "đã giao dịch" / "đã lưu phách". Thiếu 2 tệp phụ (`小按钮二字`, `信件选择框11`) — vô hại |
| Script client `uiauction_house.lua` | **CÓ** (27 KB, sửa vài chỗ như đã làm với `uimail.lua`) |
| 13 lớp C++ UI + 21 hàm xuất Lua | **PHẢI VIẾT** (~2.500–3.500 dòng, khuôn mẫu = `UiMail.cpp` đã làm) |
| Bộ máy máy chủ `auction_manager.lua` + bảng MySQL | **PHẢI VIẾT** (khuôn mẫu = `mailmanager.lua` + `KMailServer.cpp`) |
| NPC mở cửa sổ + biểu tượng | Phải thêm (như Tín Sứ / biểu tượng thư) |

---

## 3. CHIẾN LỆNH — bằng chứng

Tên trong mã nguồn không phải "battle pass" mà là **`SEASON_TICKET`** (mục `--------------------2026战令---------------------------` trong `protocol.lua` của 2.0).
Vì vậy tìm theo chữ "BattlePass/ZhanLing" ban đầu ra 0 kết quả.

### 3.1 Có gì
- **4 giao thức**: S→C `SEASON_TICKET_SHOW_ICON`, `SEASON_TICKET_UPDATE_WND`; C→S `SEASON_TICKET_GET_AWARD`, `SEASON_TICKET_GET_SCORE`.
- **Script client**: `\script\ui\season_ticket_2026.lua` (32.688 B, 818 dòng, 18 hàm) + `\script\ui\season_ticket_2026_icon.lua` (1.217 B).
- **Định nghĩa dùng chung**: `\script\global\season_ticket_2026\def.lua` (22.682 B) — bảng mốc thưởng, danh sách nhiệm vụ, mùa giải.
- **KHÔNG có lớp C++ nào** (`gamecl.exe` 0 dấu vết `SeasonTicket`). Toàn bộ giao diện dựng bằng Lua.

### 3.2 Mô hình chơi (đọc từ script)
Đúng khuôn chiến lệnh hiện đại: danh sách **nhiệm vụ** → cộng **điểm** (`GetScore(nMissionId)`) → lên **cấp/mốc** (`CalcPlayerHaveFinishLv`) →
mỗi mốc có **2 nhánh thưởng**: miễn phí và VIP (`BtnBuyVipCard`, `bIsVip`, `ImgLowAwardBG`), có **thanh tiến độ**, **hạn mùa giải**
(`TxtRemainTime`, `TransExpiredTime`, `CalcRemainTimeTxt`), nhận thưởng bằng `OnGetAward(nElemId, bIsVip)`.

### 3.3 Rào cản thật sự
2.0 dựng cửa sổ này bằng **khung giao diện Lua động**:
`UiManage:OpenWindow("UI_SEASON_TICKET_2026")` với `\script\ui\manage.lua` + `\script\ui\uicontrols\controls.lua`
(kéo theo `prefabcontrols.lua`, `wndcontainer.lua`, `wndtextcontainer.lua`, `wndtextcheckbox.lua`, `wnditembox.lua` + `\UI\prefabcontrols.ini`).

| Client | `manage.lua` | `controls.lua` | hàm C++ cần (`LoadUiGroup`, `Wnd_Show/Hide/BringToTop`, `ObjBox_HoldObject`, `UiSoundPlay`…) |
|---|---|---|---|
| 2.0 | có | có | có |
| JX1 cũ | **có** | **có** | (chưa kiểm sâu) |
| **Dự án ta** | không | không | **0/8 hàm có trong Game.exe** |

⇒ Muốn "giống 100% 2.0" thì phải port cả khung giao diện Lua đó (thêm ~8 nhóm hàm C++ + thư viện Lua + ini) — việc này **to hơn chính chiến lệnh**,
nhưng làm một lần thì mọi tính năng mới sau này (2.0 dùng nó cho hàng loạt cửa sổ đời mới) đều dựng bằng Lua, không phải build lại client.

---

## 4. PHƯƠNG HƯỚNG ĐỀ XUẤT

### 4.1 Đấu giá — 3 mức, làm được ngay

| Mức | Nội dung | Ước lượng | Ghi chú |
|---|---|---|---|
| **A. Ký gửi cá nhân** | chỉ tab "Cá nhân": người chơi đặt bán, hết 24 giờ giảm giá 1 lần, ai mua trước được; tiền/đồ trả qua **thư** | ~60% khối lượng hệ thư | Đúng cái chợ mà máy chủ tư nhân hay cần nhất; ít rủi ro nhất |
| **B. + Thế giới** | thêm phiên đấu giá toàn server theo giờ (kiểu Anh + Hà Lan), có thông báo toàn server | +40% | Cần bộ hẹn giờ phiên trên máy chủ |
| **C. + Bang hội** | phiên riêng của bang, kèm **danh sách thành viên + lương** (`AuctionSetSalaryAndCount`) | +25% | **Nối thẳng với phần phát lương bang hội vừa làm ở đợt 9** |

Thứ tự thi công đề xuất (giống hệ thư): (1) bảng MySQL `auction_item` + `auction_activity` và `KAuctionServer.cpp`;
(2) `auction_manager.lua` máy chủ; (3) `UiAuction*.cpp` client + 21 hàm Lua; (4) thả 13 ini + script 2.0; (5) NPC + biểu tượng.

### 4.2 Chiến lệnh — 2 lựa chọn

| Cách | Việc | Ưu | Nhược |
|---|---|---|---|
| **1. Cửa sổ C++ riêng** (như hệ thư) | viết 1 lớp `KUiSeasonTicket` + ~10 hàm Lua, dùng lại logic script 2.0 | Nhanh hơn, ăn chắc, không đụng khung UI | Không "100% giống 2.0"; mỗi tính năng mới sau lại phải build client |
| **2. Port khung UI Lua động** | thêm `LoadUiGroup`/`Wnd_*`/`ObjBox_HoldObject`… vào Game.exe + thư viện `uicontrols` + `prefabcontrols.ini` | Sau này mọi cửa sổ mới viết bằng Lua, không cần build; port thẳng script 2.0 | To hơn nhiều; rủi ro chạm hệ UI đang chạy |

Nếu chủ muốn chiến lệnh trong thời gian ngắn thì chọn **cách 1**. Nếu tính đường dài (còn định port nhiều tính năng 2.0 nữa) thì **cách 2** đáng đầu tư,
và nên làm **sau** đấu giá để đấu giá không phải chờ.

### 4.3 Ba câu cần chủ chốt trước khi thi công
1. **Đấu giá bán bằng tiền gì?** 2.0 dùng 2 loại tiền khoá (ô 4361 bạc khoá / 4362 kim khoá). Dự án ta đang dùng ô 251 cho xu.
   Chọn: Ngân lượng, xu (251), hay thêm loại tiền mới?
2. **Làm mức nào trước?** A (ký gửi cá nhân), B (thế giới), hay cả C (bang hội, nối lương bang)?
3. **Chiến lệnh theo cách 1 hay cách 2?**

---

## 5. Vật liệu đã rút (dùng lại khi thi công)

Thư mục tạm phiên này: `…\scratchpad\vltk2_dg\`
- `gamecl.img` — ảnh gamecl.exe đã giải nén UPX (34,5 MB)
- `auction_jx1cu\` — 13 ini + `uiauction_house.lua` + `auction_def.lua` + `protocol_def_c.lua` (client JX1 cũ)
- `v20\` — `protocol.lua`, `protocol_def_c.lua`, `season_ticket_2026.lua`, `season_ticket_2026_icon.lua`, `st_def.lua`,
  `manage.lua`, `controls.lua`, `auction_settings.ini` (bản 2.0)
- `scanpak.py`, `scanscript.py` — công cụ quét pak theo từ khoá (GBK + TCVN3 + ASCII)

Khi chốt hướng, các tệp này nên chuyển vào `ReverseTools\pak_vltk\vltk2\auction\` và `…\season_ticket\` để lưu lâu dài.
