# BÀN GIAO — PHI PHONG THI CÔNG ĐỢT 31/08 CHIỀU (đợt 1+2+5+6+7)

**Nối tiếp** `BANGIAO_PHIPHONG_TIEPTUC_3108.md` (tóm tắt) và `BANGIAO_PHIPHONG_DANHSACHVIEC_3108.md` (danh sách việc).
Phiên này **thi công** toàn bộ các đợt KHÔNG cần chủ game quyết: **Đợt 1 (Lua) · Đợt 2 (ITEM-01) · Đợt 5 (client) · Đợt 6 (chọn lỗ khảm) · Đợt 7 (đồng bộ m_nPfPack)** — sau đó chạy **đợt phản biện 7 tác tử độc lập** (2,45 triệu token, 0 tác tử lỗi) trên chính bản vá và sửa tiếp 4 lỗi thật nó tìm ra.

Git: 3 commit `48df7378` → `3c5f326b` → `05f44239` (D:\GAMEDEVNEW). 2 tệp Lua nằm ngoài git (cây E:), có backup `.truoc_chonlo_3108` cạnh tệp.

---

## 1. CHỜ SWAP — BA NHỊ PHÂN, PHẢI CÙNG LÚC

| Tệp | md5 (12 đầu) | Thay cho |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | `0024C5E945C6` | bản `9ED3CB2A` (vá ô chết REFOAN) — **đã gộp**, không mất gì |
| `bin\client\CoreClient.dll.moi` | `0DB83C635C1E` | — |
| `bin\client\Game.exe.moi` | `9374C3DFFD0D` | — |

🔴 **VÌ SAO PHẢI CÙNG LÚC** (tác tử protocol đã chứng minh đến từng dòng): gói mới `s2c_syncpfpack` (=215) gửi cho MỌI vật phẩm phi phong ngay lúc đồng bộ hành trang. Client CŨ tra `g_nProtocolSize[150]` = 0 → rơi vào nhánh biến-độ-dài → con trỏ đọc gói nhảy bậy theo 2 byte thấp của dwID → **desync cả luồng mạng phía sau** (mất item/HP sync, hiếm khi crash). Quy trình chuẩn (tắt server → `ChayGameServer.bat` / thoát game → `ChoiGame.bat`) tự đảm bảo điều này vì restart server ngắt mọi client; chỉ CẤM kịch bản "swap server mà client cứ chạy tiếp binary cũ".

Script Lua (2 tệp `mantleupgrade_*.lua`) đã nằm trên đĩa, nạp lúc boot — **cùng sống với DLL ở lần khởi động tới**, đúng luật 0.4 (script có rào `if GetGiveItemSlot then` nên nạp trước DLL cũng không nổ).

---

## 2. ĐÃ LÀM — THEO SỐ MỤC CỦA DANH SÁCH VIỆC

### Đợt 2 — CoreServer (lỗ hổng kinh tế)
- **2.1 ITEM-01** ✅ thêm `"\\script\\global\\mantlesystem\\"` vào `szJx2[]` (`KItemList.cpp:1963`). Tác tử phản biện xác nhận: 4885/4887 giờ trừ đúng 1, **4883 không bị trừ hai lần** (return 1 mọi nhánh + 2 lớp chặn `nRet != 1` và `SearchID`), không vật phẩm nào khác đổi hành vi (grep magicscript = đúng 3 record).
- **2.2** ✅ `KJx2WarInfra_ClearGiveSession` (xoá cả 2 map) gọi từ `RecoveryBoxCmd` (kèm vô hiệu `m_dwGiveBoxId`) + `KPlayer::Release` (khe tái sử dụng).

### Đợt 1 — Lua thuần (cây E:)
- **1.1** ✅ 3 điểm đếm `+ GetItemStackCount` → `+ 1` (head:310/435/631) — hết đếm N², hết lỗ hổng tỉ lệ ×N, hết chặn đường đột phá. `head:720` (rào chồng) giữ nguyên đúng chỉ định.
- **1.2** ✅ 2 chuỗi mô tả rút còn 201/177 byte (< trần 255).
- **1.3** ✅ đường tự xếp chỉ chọn lỗ trống **đã tăng sao**, phân biệt 2 thông báo ("lỗ còn lại đều 0 sao" ≠ "hết lỗ trống"). *Lệch spec có chủ ý:* bỏ "vòng 2 xếp vào lỗ 0 sao kèm cảnh báo" vì tự mâu thuẫn với hàng rào 6.3-(2) — hành vi cuối nhất quán: không bao giờ trượt im lặng.
- **1.4** ✅ `ProcessInlayStarStone` chỉ trừ đá khi `InlayStarStone(...) == 1`, đếm `nTruot`, trượt thì báo rõ + `g_ReturnInlayResult(0)`.
- **1.5** ✅ `szLockState` (chưa từng gán — lỗi gốc Kingsoft) → `nBindState` + `%d`.
- **1.6** ✅ shim `SetUiGiveItemMsg` lọc trùng theo người chơi + tiền tố `[Phi Phong]`; cache được xoá khi mở phiên mới (`onMantleSystem`).
- **1.7** ✅ nhánh mất phiên của `doMantleMosaicStoneBox` **tự dựng lại phiên** (tốt hơn spec); nhánh `not PlayerIndex` giữ return (không có ngữ cảnh người chơi để Talk).
- **1.8** ✅ `PF_MoLaiHopNangCap()` gọi lại đúng handler menu theo `nType`; 3 nhánh lỗi của `doMantleUpGrade` (szErrorMsg / thiếu tiền / `DoMantleUpGrade()==0`) đều mở lại hộp; `DoMantleUpGrade` trả 0 **chỉ khi giao dịch CHƯA chạy** (đã lập bảng return từng nhánh Process* — không có đường nào đã trừ tiền/đồ mà trả 0). Nhánh mất phiên báo rõ bằng Talk.
- **1.9** ✅ phần không cần duyệt: bỏ lời hứa "mở server N ngày" (npc:49 — `nMinOpenDays` không ai cưỡng chế ở cả hai cây); bỏ chỉ dẫn tới "Nâng cấp Vô Cực" (head:79 — menu đang tắt; **nếu sau này mở Vô Cực thì thêm lại**). Phần [Q] (npc:12) chưa đụng.

### Đợt 5 — Client (Game.exe)
- **5.1** ✅ `KUiMantleInlay::CloseWindow` + `KUiAffairItem::CloseWindow`: `OnCancel()` TRƯỚC khi huỷ — Huỷ/X/ESC trả đồ về túi, hết gọi qua con trỏ NULL.
- **5.2** ✅ tiêu đề panel dùng chuỗi script (30 byte, vừa khít Value[32] — tác tử đã đo cả chuỗi copy 3 tầng).
- **5.3** ✅ thẻ `AddStarstoneBtn` giữ trạng thái CHECKED.
- **5.4** ✅ một nửa: nhãn 60px kẹp trong lòng `m_Pad` — hết tràn viền trái, hết đè GuideList. Giữ nhãn "Phi Phong" + `BringToTop` (khác spec-b; các ô bị đè giờ chỉ là trang trí).
- **5.6** ✅ ẩn món hàng dưới (`Region.v != 0`) — hết "bóng ma"/gộp ô.
- **5.7** ✅ **ĐÃ CHỐT BẰNG ẢNH THẬT**: trích `\spr\Ui3\星辰石镶嵌.spr` (424×233, trong `updatejx14.pak`) → 5 hốc đá là **VÒNG NGOÀI obj_0..4** (trùng khít 5 hoa văn mũi nhọn của ngôi sao chúc xuống: 2 trên, 2 hông, 1 đuôi dưới); obj_10..12 cũ nổi trên nền TRỐNG. Ảnh + khung đối chiếu: scratchpad phiên này (`nen_kham.png`, `nen_kham_khung.png`).

### Đợt 6 — Chọn lỗ khảm (khoảng cách chính với Linux)
- **6.1** ✅ đủ 6 sửa đổi + đúng 4 ràng buộc cứng (tác tử đã đối chiếu: thứ tự duyệt/khai triển chồng/giá trị trả về **giữ nguyên từng byte ngữ nghĩa**, 29 điểm gọi cũ không đổi hành vi). Mở rộng: `vSlots` ghi `-1` cho món hàng dưới (`nY != 0`).
- **6.2** ✅ shim đọc `GetGiveItemSlot`, rào `bCoSlot` (DLL cũ → tự xếp), nhánh riêng "Phi Phong hãy đặt vào ô giữa" / "Ô giữa dành cho Phi Phong"; món hàng dưới bị **bỏ qua hẳn** (không cướp chỗ Phi Phong, không chặn luồng bằng món vô hình — sửa theo phản biện).
- **6.3** ✅ 3 hàng rào (lỗ chưa mở / lỗ 0 sao / lỗ đã có đá) — hàng rào 2 dùng đúng câu có sẵn của bản Linux. Hàng rào 3 là **lệch CÓ Ý** (Linux cho ghi đè khác loại và viên cũ mất vĩnh viễn) — chờ chủ game quyết (mục 4).
- **6.4** ✅ nhãn "Lỗ khảm 1..5" (`L\347 kh\266m`, đúng luật TCVN3) + `PF_UI_INPUT = {0,1,2,3,4,13}` + sửa chú thích sai trong `.h`.

### Đợt 7 — Đồng bộ m_nPfPack (client hết mù sao/lỗ/đá)
- **7.1** ✅ phương án (b): gói riêng `ITEM_SYNC_PFPACK` (21 byte, pack(1), `Headers\KMantleProtocol.h` — guard riêng, include từ CẢ HAI KProtocol.h theo khuôn KDiceProtocol). `s2c_syncpfpack = 215` chèn cuối dãy; bảng size nửa client thêm đúng ô 150; handler client khuôn y hệt `s2cSyncMagic`; **chỉ gửi cho vật phẩm có pfpack ≠ 0** → vật phẩm thường không tốn thêm byte nào. Cả đường login (đồng bộ cả túi) lẫn mọi `SyncItem` runtime đều mang gói kèm. Lưới giao thức + thứ tự handler đã qua 2 công cụ kiểm (151 ô, phần tử cuối = 215; nền 69 chỗ sai s2c là nợ cũ, bản vá thêm 0).

### Sửa theo đợt phản biện (31/08c — commit `05f44239`)
1. 🔴 **`UiCommandScript` case 1 tiêu thụ `m_dwGiveBoxId` TRƯỚC callback.** Bản gốc gán 0 SAU callback → mọi cơ chế "mở lại hộp" (kể cả vá 31/08 sáng) bị đè mất id mới → **nút OK lần hai chết im lặng**. Đây là lỗi CHẶN HẲN mà nghiệm thu chắc chắn sẽ vấp nếu không sửa. (Sửa cả cho OpenGiveBox JX1 — mở lại hộp trong callback giờ mới sống.)
2. **Magic static cho `s_GiveSessions`/`s_GivePending`**: `KPlayer` ctor → `Release` → `ClearGiveSession` chạy ngay trong static init của DLL; static toàn cục chỉ sống nhờ thứ tự file trong vcxproj (KJx2WarInfra đứng trước KPlayer) — mìn nổ chậm, đã gỡ.
3. **Món hàng dưới trong chế độ chọn lỗ bị bỏ qua hẳn** (xem 6.2) — trước đó phi phong sót có thể thành ĐÍCH khảm tàng hình.
4. **`nBindState = nBindState or 0`** trong `ProcessSecBreakThrough` — `DynamicExecute` không thể trả giá trị (mọi đường `return 0` giá trị Lua) và `equip_trader.lua`/`calc_new_bindstate` không tồn tại; thiếu rào là ScriptError sau khi giao dịch đã chạy (đường này hiện là mã chết vì menu Vô Cực tắt).

---

## 3. NGHIỆM THU SAU SWAP (GM làm theo thứ tự)

1. **ITEM-01**: phát 1 viên PT 4887 (Tinh Thần Khoáng), bấm → nhận 1 Tinh Thần Thạch **và viên khoáng biến mất**. Lặp với 4885 (trừ thêm 1 Tinh Hoả Than). Phát PT 4883 + 10 mảnh → ghép xong trừ đúng 10, **không trừ kép**.
2. **Chọn lỗ** (`test_phiphong_admin.lua` tạo phi phong 3 lỗ: 1–2 cấp 10, lỗ 3 cấp 0):
   - đặt đá vào **ô số 3** → báo "0 sao, tăng sao hãy khảm", đá còn;
   - **ô số 4** → báo "lỗ chưa mở", đá còn;
   - **ô số 1** → khảm đúng lỗ 1 (rê chuột lên phi phong thấy dòng lỗ — Đợt 7 làm tooltip sống);
   - lại **ô số 1** → báo "đã có Tinh Thần Thạch";
   - đặt Phi Phong vào ô đá / đá vào ô giữa → 2 thông báo riêng.
3. **Nút OK lần hai**: bỏ thiếu đồ → OK → báo lỗi + hộp mở lại → sửa đồ → **OK lần hai phải chạy** (đây là chỗ lỗi 31/08c-1 chặn trước đây).
4. **Huỷ/X/ESC panel khảm** → đồ về túi ngay. ⚠️ làm cả ca "túi chỉ còn 1 ô trống" — xem mục 4 rủi ro RecoveryBox.
5. **Hiển thị**: phi phong 5 sao khảm 2 viên → tên có tiền tố "5 sao", tooltip đủ dòng lỗ + điểm chúc phúc; nhãn "Lỗ khảm 1..5" nằm đúng 5 hoa văn mũi nhọn, thẻ trên cùng bấm không tắt, tiêu đề panel là chuỗi script.
6. **Hồi quy 5 luồng give-box khác** (dùng chung `GiveBoxCollect`): đột phá Phi Phong · `equip_system.lua:167` · `seasonnpc.lua:415` · `lib\composeex.lua:185` · `missions\yandibaozang\yandiduihuan.lua:108` (nhạy nhất).
7. **Tăng sao**: 1 chồng 10 Tinh Ngọc → tỉ lệ phải là `10 × nRatePreStar` (không phải ×100); đột phá thường: chồng 2 Thiên Tinh Ngọc → báo "đã đặt 2".

## 4. RỦI RO ĐÃ BIẾT + CHỜ CHỦ GAME QUYẾT

**Rủi ro chấp nhận (ghi rõ theo yêu cầu spec 5.1):**
- `KPlayer::RecoveryBox` khi túi hết chỗ sẽ nhét món vào TAY, lệnh thu hồi kế tiếp **ném món đang cầm xuống đất** (`KPlayer.cpp:6663-6708`). Huỷ panel khảm giờ thu hồi cả hộp một lượt nên ca "túi gần đầy" có thể rơi đồ — rủi ro sẵn có của hộp `UiAffairItem`, nay panel khảm cũng chạm tới. Muốn chặn triệt để phải theo kỷ luật `DT_ThuHoiBox` (mỗi lần 1 món + kiểm chỗ) — việc treo.
- "Xem trang bị người khác" không mang pfpack → không hiện sao/lỗ của đồ người khác (chỉ đồ của mình). Khoảng trống, không phải regression.
- 5.5 [T] treo: `GDCNI_END_AFFAIR_BOX` vẫn đóng panel khảm vô điều kiện (hành vi cũ, không nặng hơn).
- Guard gửi pfpack bỏ ô [3] đơn lẻ + trạng thái "pack về 0" không đẩy xuống client — chỉ là foot-gun tương lai, chưa có đường chạy tới.

**Câu hỏi chờ quyết (chặn các việc Đợt 3/4/8 — xem mục F danh sách việc):**
1. Hàng rào 3 (cấm ghi đè lỗ đã có đá) — giữ (an toàn) hay bỏ cho giống Linux 100% (ghi đè = mất vĩnh viễn viên cũ)?
2. Đường phát Phi Phong + Tinh Thần Thạch cho người chơi thường (3.2) và nguồn rơi nguyên liệu (3.3) — hiện = SỐ KHÔNG. ⚠️ ITEM-01 đã vá nên mở nguồn giờ an toàn.
3. Tiệm nguyên liệu (3.1): thêm hàng buysell + có giữ khoá đồ mua không.
4. 19 dòng `goldequip.txt` đổi số (3.4 — ưu tiên `:5379` cột 62 và `:5931-5933`) — giữ Linux hay trả JX1 cũ.
5. `mask.txt` thay trọn bảng (3.5 — **rủi ro lớn nhất**): quét CSDL mặt nạ cũ trước khi cho người chơi vào.
6. Gắn Hoàng Kim Ấn vào `thoren.lua` (4.2) + thử GM lệch 1 cấp Ấn trước khi vá (4.1).
7. Cấp sao có cộng thuộc tính không (8.1); có mở nhánh Vô Cực không (8.2 — nếu mở, thêm lại câu chỉ dẫn ở head:79).

## 4b. BỔ SUNG 31/08d (sau khi chủ game swap 15:24 và test)

Chủ game báo *"bỏ vào ép thì báo thiếu nguyên liệu"* trên bản MỚI. Mổ ra 2 gốc, đã sửa (thuần Lua, không cần build):

1. 🔴 **Hàng rào "lỗ 0 sao" (6.3-2) là lỗi thiết kế của đợt sáng**: phi phong GM phát ra (hoặc mua/nhặt) **mới tinh thì MỌI lỗ đều 0 sao** (cấp lỗ chỉ được đặt lúc đột phá, `head.lua:501`) ⇒ cấm lỗ 0 sao = **cấm khảm hoàn toàn bậc đầu chuỗi**. Danh sách việc mục 1.3 đã cảnh báo đúng điều này. Sửa theo bản Linux: **cho khảm NẰM NGỦ** (chưa cộng gì), thành công thì cảnh báo vàng "%d viên nằm ở lỗ 0 sao, sẽ phát huy sau khi đột phá kích hoạt lỗ"; đường tự xếp có 2 vòng (ưu tiên lỗ đã kích hoạt). Hàng rào 1 (lỗ chưa mở) và 3 (lỗ đã có đá) giữ nguyên.
2. **Hộp đột phá đòi ĐÚNG 1 viên Thiên Tinh Ngọc** (Linux cũng vậy) nhưng lệnh bài phát nguyên chồng 200 ⇒ thả cả chồng là bị từ chối; kèm 2 cặp tên dễ nhầm (Thiên Tinh Ngọc 4882 ≠ Mảnh Thiên Tinh Ngọc 4883; Tinh Thần Thạch nguyên liệu 4888 ≠ đá khảm G9 có tiền tố).

**`test_phiphong_admin.lua` viết lại toàn bộ** (backup `.truoc_codau_3108`): nhãn/thông báo **tiếng Việt có dấu** (TCVN3, tránh chữ HOA có dấu), thêm **3 bộ thử phát đúng từng luồng**:
- *Nhận bộ thử tăng sao*: 1 Tuyệt Thế + 50 Tinh Ngọc + tiền.
- *Nhận bộ thử đột phá*: 1 Tuyệt Thế 10 sao **đầy chúc phúc** (chắc chắn thành công) + **đúng 1** Thiên Tinh Ngọc + tiền.
- *Nhận bộ thử khảm đá*: 1 Ngự Phong 5 lỗ (4 lỗ kích hoạt, lỗ cuối 0 sao như sau đột phá thật) + 5 viên đá khác loại.
Menu gốc đo 447/512 byte (trần Say dùng chung tiêu đề + nhãn); mọi SayEx phụ đều tự đo lúc sinh.

**Kích hoạt không cần restart** (script nạp theo state lúc boot, thoại Thợ Rèn chạy trong state của `thoren.lua`):
```
?gm RLS \script\global\npcchucnang\thoren.lua
?gm RLS \script\item\lenhbaiadmin.lua
```
(`RLS` gỡ + nạp lại MỘT tệp và chạy lại thân chunk → `Include` kéo bản mới của `mantleupgrade_npc/head.lua` và `test_phiphong_admin.lua` từ đĩa. Khởi động lại server cũng được.)

⚠️ Bẫy tự vấp khi làm 31/08d, ghi lại: driver sửa-lặp-được có 2 phép thay mà `old` là **chuỗi con của `new`** → chạy lại 2 lần là khối bị nhân 3 (đã dọn, kiểm bằng count=1). Phép thay kiểu "old → old + thêm" **không idempotent** — phải kiểm marker trước.

## 4c. BỔ SUNG 31/08e — "đá nhận về vô hình" + tooltip chuẩn (commit `d545facb`)

Chủ báo: *"nhận đá khảm mà không hiện hình và thông tin đá"* + gửi ảnh bản chuẩn (tooltip phi phong có danh sách đá từng lỗ + khối **thuộc tính ẩn màu tím**).

1. 🔴 **Gốc đá vô hình**: `KItem::operator=(KBASICPROP_STARSTONE&)` (port 29/08, `KItem.cpp:896`) **thiếu trọn khối `#ifndef _SERVER`** mà mọi operator= khác đều có — không chép `szImageName`/`szIntro`, không khởi tạo `m_Image` ⇒ client vẽ ô rỗng, chuột phải trống. Server không dùng các trường này nên nằm im từ 29/08 (chưa ai từng nhìn viên đá G9 thật phía client). Đã bổ sung + vệ sinh trường stale của khe tái sử dụng (`AddItemSet2` KHÔNG zero `m_CommonAttrib`): `szScript`, `nPickExecute`, `nIsSell=1`, `nIsTrade=1`.
2. **Tooltip phi phong so ảnh chuẩn**: khung đã đúng sẵn từ 29/08 (chúc phúc `HBlue`, mỗi lỗ `%d sao <tên đá>` màu Green đúng khoá `G_STR_COLOR_XING` của slistcache, thuộc tính đá mỗi viên một dòng qua `g_MagicDesc` — câu chữ tự khớp vì `magicattrib_ge.txt` là dữ liệu Linux). Chỉ lệch **màu khối thuộc tính đá**: Green → đổi **tím `<color=200,120,255>`** (khối "thuộc tính ẩn" trong ảnh) + thêm dấu cách trước `<color>` đóng (luật TEncodeText nuốt `<`). Lý do chủ chưa từng thấy khối này: client mù pfpack (tới Đợt 7) + khảm bị chặn 0-sao (31/08d) + đá vô hình (mục 1).
3. `.moi` mới: `CoreClient.dll.moi` **751F86EB207D** (mang cả hai fix hiển thị — chỉ cần thoát game → `ChoiGame.bat`) + `CoreServer.dll.moi` **AFAF8E402E06** (đồng bộ nguồn, chờ lần khởi động server sau; không đổi giao thức nên chạy lệch tạm thời an toàn).

## 4d. BỔ SUNG 31/08f — "khảm xong không hiện thuộc tính" = lỗ 0 sao (thuần Lua)

Ảnh chủ gửi cho thấy 5 dòng đá đều "**0 sao** …" — đá nằm trong lỗ 0 sao (nằm ngủ) nên **đúng cơ chế** là không cộng/không hiện thuộc tính (giá trị = `nValue[cấp lỗ]`, thiết kế Linux; ảnh chuẩn các dòng đều "10 sao"). Gốc: **lệnh bài phát phi phong THÔ** (AddGoldEquipByRow/AddPlatinaItem không đặt cấp lỗ), trong khi kinh tế thật luôn ra món có lỗ kích hoạt (đột phá thường: lỗ 1..n−1 = 10; đột phá lần 2: đủ hết = 10).

Sửa `test_phiphong_admin.lua`: helper `PP_KichHoatLo(nIdx, nBachKim)` — mọi nút phát phi phong giờ kích hoạt lỗ **đúng trạng thái tự nhiên** (hoàng kim chừa lỗ cuối 0 sao, bạch kim đủ hết); thêm mục **"Kích hoạt lỗ khảm mười sao"** trong bảng ép món đang mặc (nâng cấp lỗ, GIỮ nguyên đá — đá ngủ thức dậy, phải thoát/vào lại theo PP_NHAC). Kích hoạt bằng `?gm RLS \script\item\lenhbaiadmin.lua`.

## 4e. KIỂM TOÁN 31/08g — thuộc tính đá có CỘNG THẬT vào nhân vật không?

Đối chiếu trọn 34 viên (`starstone.txt` cột 14 → `magicattrib_ge.txt` cột 5 = mã loại → bảng đăng ký `ProcessFunc[]` trong `KNpcAttribModify.cpp`, dispatcher `ModifyAttrib` **bỏ qua im lặng** loại không đăng ký):

- ✅ **26/34 viên cộng thật**: đường áp `PF_ModifyStoneAttrib` chạy trong `ApplyMagicAttribToNPC` lúc MẶC (`KItem.cpp:327`) và gỡ đối xứng lúc cởi (`:415`); giá trị = `nValue[cấp lỗ]`. Giá trị cấp-10 khớp TỪNG SỐ với ảnh tooltip bản chuẩn (Anh Dũng +50, Cường Công +228, hóa giải +4…) ⇒ dữ liệu = dữ liệu chuẩn.
- 🔴 **8/34 viên hiện CỘNG "CÂM"** (tooltip hiện dòng tím nhưng không có tác dụng): mã loại có TÊN trong enum `KMagicAttrib.h` nhưng **chưa từng có handler** (tên chỉ xuất hiện đúng 1 chỗ = dòng khai báo; grep toàn Sources không lọc):

| P | Viên | type | enum |
|---|---|---|---|
| 5 | Cương Ngạnh | 219 | `magic_anti_do_stun_p` |
| 7 | Chí Mật | 220 | `magic_anti_stuntimereduce_p` |
| 11 | Đoạn Liệt | 271 | `magic_addblockrate` |
| 15 | Thiểm Diệu | 248 | `magic_anti_lightingres_yan_p` |
| 29 | Thuần Tịnh | 263 | `magic_normal_reserve8` |
| 30 | Lỗ Mãng | 266 | `magic_rangedamagereturnmana_p` |
| 31 | Cuồng Nhiệt | 265 | `magic_meleedamagereturnmana_p` |
| 33 | Cơ Mẫn | 267 | `magic_reduceskillcd1` |

Đây là **nợ engine JX1 có sẵn** (8 loại này chưa hệ nào dùng trước Phi Phong). Muốn giống 100% Linux phải **port 8 handler từ `jx_linux_y`** (mổ nhị phân lấy ngữ nghĩa từng loại) — việc C++ kế tiếp, ĐANG CHỜ: (a) chủ gật, (b) phiên `wauto-f1` build xong (đang giữ lượt build Core, tránh bẫy trộn .obj). Công cụ kiểm: scratchpad `t88_kiem_handler_da.py` (in đủ 34 viên).

**Kiểm TẦNG 2 (31/08g-b, `t90_kiem_field_co_ai_doc.py`)** — có handler chưa đủ, field handler ghi vào phải có nơi ĐỌC trong đường chạy, nếu không vẫn câm. Tự động: handler → field `m_Current*` → đếm nơi đọc trong toàn `Core\Src` (bỏ dòng khai báo/gán/memset). Kết quả: **26/26 viên có handler đều có nơi đọc thật** (ví dụ `m_CurrentSkillEnhancePercent` đọc ở `KNpc.cpp:4453`, `m_CurrentStunTimeReducePercent` ở `KNpc.cpp:4360/4363`, khối kháng hệ ở `CoreShell.cpp:1271-1322`) ⇒ **đúng 8 viên câm, không hơn**.
⚠️ Bẫy của chính bộ đo: bản đầu báo oan 6 viên "handler không ghi field" vì regex `)\s*{` vấp **comment nằm giữa `)` và `{`** (`void KNpcAttribModify::BlockRate(...)   //#hoa giai`). Phải bỏ comment trước khi cắt thân hàm — cùng họ với luật 0.2 (grep có lọc thì kết luận sai).

**Dữ liệu 8 viên đã đối chiếu Linux** (`t89_gia_tri_8_vien.py`): giá trị 10 cấp của JX1 **khớp bản patch CUỐI `Patch\settings\item\004`** từng số. Riêng P=11 Đoạn Liệt khác các bản 000-003 (`1,2,2,3,3,4,4,5,5,6` vs `1,1,1,2,2,2,3,3,4,4`) — **JX1 đúng, vì 004 là bản mới nhất**; đừng "sửa" theo 000.

## 4f. BỔ SUNG 31/08h — làm liên tục + thả cả chồng (thuần Lua) & 2 câu kiểm tra

**Lỗi 1 — "mỗi lần xong phải tắt box mở lại".** Gốc: đường **THÀNH CÔNG** của `doMantleUpGrade` xoá phiên (`tbAllPlayerProcess[PlayerIndex] = nil`) mà **không mở lại hộp** — vá 31/08b chỉ mở lại ở các nhánh LỖI. Sửa: thành công thì dựng phiên MỚI cùng `nType` rồi gọi `PF_MoLaiHopNangCap()` (handler tự đặt lại `nNeedMoney`/`nNeedItem`/`szTitle`). Tăng sao đặc biệt tiện: phi phong vẫn nằm trong hộp nên bỏ thêm Tinh Ngọc là tăng tiếp ngay.

**Lỗi 2 — "thả cả chồng thì không tự trừ, phải tách từng viên".**
- *Đột phá* (thường + lần 2): điều kiện `nStarCount ~= nNeedItem` → **`<`** (chỉ đòi ĐỦ). Thả cả chồng 200 viên vẫn chạy, chỉ trừ đúng 1 (hoặc 15/20/25/30/35).
- *Tăng sao*: thêm trần `nToiDa = ceil(100 / nRatePreStar)` — chỉ dùng số viên **đủ đạt 100%**, phần dư giữ nguyên trong hộp (trước đây ăn sạch cả chồng, vừa phí vừa buộc tách tay). Bảng thông tin báo rõ *"Đặt vào %d viên, chỉ dùng %d viên là đủ, phần dư giữ nguyên."*
- Cơ chế: thêm `self.nDungSo`; cả 4 vòng trừ nguyên liệu chuyển sang trừ **tối đa `nDungSo` đơn vị** (`local nConTru`, 4 chỗ).
- ⚠️ **Lệch Linux có chủ ý** (Linux đòi đúng bằng và ăn cả chồng) — theo yêu cầu trực tiếp của chủ game, ghi nhận tại đây.

**Câu hỏi 3 — thuộc tính phi phong/ấn/mặt nạ có giống 100% Linux chưa? → CÓ, khác = 0** (`t94_so_thuoctinh_pp_an_matna.py`, so từng ô với `Patch\settings\item\004`):

| Bảng | Mặt nạ (DT 11) | Phi phong (12) | Ấn (13) | Trang sức (14) |
|---|---|---|---|---|
| `goldequip.txt` | 231 giống / **0 khác** | 31 / **0** | 166 / **0** | 126 / **0** |
| `platinaequip.txt` | 207 / **0** | 39 / **0** | 153 / **0** | 127 / **0** |

Thêm: **0 dòng của Linux bị thiếu ở JX1**; 1.460 dòng chỉ có ở JX1 đều nằm **sau** vùng Linux (từ dòng 5939) nên không xô lệch mã vật phẩm.

🔴 **ĐÍNH CHÍNH mục A7/3.4 (một trong 7 câu [Q] treo)**: mục đó đề nghị "trả lại cột trần điểm chúc phúc `22` cho Phi phong Ngự Phong vì đợt port đã xoá". Đo thực tế ba bên: **JX1 = rỗng, Linux 004 = rỗng, JX1 trước port = 22** ⇒ đợt port đã đổi **để khớp Linux**, và trả lại 22 mới là **lệch khỏi mốc Linux**. Hợp lý về lối chơi: Ngự Phong là bậc cuối nhánh hoàng kim, đi đường Vô Cực (tỉ lệ 100%) nên không dùng điểm chúc phúc. **Kết luận: giữ nguyên, không sửa.**

**Câu hỏi 4 — bản Linux có hiện thuộc tính của viên đá trong bảng mô tả không? → KHÔNG.** Cột mô tả (`szIntro`) của `starstone.txt` **trùng nhau từng byte giữa hai cây** và mọi viên dùng **chung một câu chung chung** ("Bảo thạch có năng lực đặc biệt, có thể khảm lên Phi Phong…"), không ghi thuộc tính cụ thể. Người chơi bản Linux phải tra ngoài hoặc khảm thử. ⇒ Muốn tiện hơn thì phải **thêm mới** (in thuộc tính theo cấp vào tooltip viên đá) — là **cải tiến vượt Linux**, chờ chủ game duyệt; làm được trong cùng lần build với đợt 8 handler.

## 4g. 🔴 PHÁT HIỆN LỚN 31/08j — CHUỖI 13 BẬC PHI PHONG ĐÃ CÓ SẴN TRONG BẢNG JX1

Chủ game cho từ khóa từ hộp thoại NPC trong client VLTK thật ("Tăng cấp Phi Phong", "Lăng Tuyệt", "Đại Bách Luyện Thành Cương"). Trích bảng `goldequip` của VLTK ra khỏi pak (`slistcache_B78E5D57.txt`, 62 cột — khớp JX1 từng dòng ở 5374-5377) và đối chiếu:

| Dòng | Tên JX1 | Tên VLTK | Cấp (c11) | **Số lỗ khảm (c60)** | Trần chúc phúc (c61) |
|---|---|---|---|---|---|
| 5374 | Tuyệt Thế | = | 2 | 1 | 4 |
| 5375 | Phá Quân | = | 3 | 2 | 6 |
| 5376 | Ngạo Tuyết | = | 4 | 3 | 8 |
| 5377 | Kinh Lôi | = | 5 | 4 | 14 |
| 5378 | Ngự Phong | = | 6 | 5 | **JX1 RỖNG / VLTK 22** |
| 5939 | Phệ Quang | = | 7 | 6 | 32 |
| 5940 | Khấp Thần | = | 8 | 7 | 44 |
| 5959 | Huyền Kim | = | 9 | 8 | 44 |
| 5960 | Vô Cực | = | 10 | 9 | 44 |
| 5961 | Kình Thiên | = | 11 | 10 | 44 |
| 6733 | Lăng Tuyệt | = | 10 / **12** | 11 | 44 |
| 6734 | **Tử Vân** | **Long Ngâm** | 10 / **13** | 12 | 44 |
| 6735 | Sồ Phượng | = | 10 / **13** | 13 | 44 |

⇒ **JX1 ĐÃ CÓ đủ dữ liệu 13 bậc, tới 13 lỗ khảm** — trần chúc phúc 44 khớp đúng ảnh chủ gửi ("Đột phá điểm chúc phúc 0/44"). Script `mantleupgrade_head.lua` mới chỉ nối tới `5378 → 3485` nên 7 bậc cuối **chưa ai chạm tới**.

🔴 **ĐÍNH CHÍNH cho chính mục 4f của tôi**: tôi đã kết luận "cột trần chúc phúc của Ngự Phong rỗng ở cả JX1 lẫn Linux nên giữ nguyên". **Sai** — tôi mới so với `D:\ServerLinux` (JX2 cũ). **Client VLTK có 22**, và bản JX1 trước port cũng 22. Với hệ Phi Phong thì **VLTK mới là mốc đúng** (đây là bản game chủ đang vận hành), nên mục A7 ban đầu **đúng**: phải trả lại 22. Bài học: hai nguồn tham khảo không đồng hạng cho mọi hệ — hệ nào chủ chỉ theo VLTK thì VLTK thắng.

**Lệch dữ liệu cần chốt với chủ**: (a) dòng 6734 JX1 tên *Tử Vân* ≠ VLTK *Long Ngâm*; (b) cột cấp của 3 dòng 6733-6735 JX1 đều ghi 10, VLTK ghi 12/13/13; (c) trần chúc phúc 5378.

**Sức chứa cho 13 lỗ** (tự tính, chờ đợt khảo sát xác minh): cần 13×6 = 78 bit mã đá + 13 bit cấp lỗ (nén 0/10 — đã đo cấp lỗ chỉ nhận 2 giá trị) + ~21 bit phần đầu = **112 bit ≤ 128 bit** hiện có ⇒ **VỪA**, còn ~16 bit cho thời gian đột phá (hạ độ chính xác xuống ngày). Nhưng phải **đổi bố cục bit** ⇒ phá tương thích với phi phong người chơi đang giữ ⇒ **cần bảng chuyển đổi một lần + chủ game duyệt**. CSDL không còn ô dự phòng nào (`iiduphong1..9` đã dùng hết) nên đây là đường duy nhất không đụng schema.

## 5. GHI CHÚ KỸ THUẬT
- Toàn bộ chuỗi Việt mới đi qua `vn_edit.vn()` (TCVN3), FFFD = 0, syncheck Lua 4.0 thật qua cả 2 tệp; kiểm lưới giao thức + thứ tự handler bằng `ReverseTools\viemde\kiem_luoi_giaothuc.py` / `kiem_thutu_handler.py`.
- Build ôm theo `KPlayerBot.cpp` chưa commit của phiên song song (sửa 12:06, TRƯỚC bản 9ED3CB2A build 12:44 — cùng trạng thái, không thêm rủi ro).
- Bộ vá tự động + ảnh chốt vòng ô: scratchpad `daec676c...\scratchpad\` (`t80_thicong_phiphong.py`, `t82_chot_vong_o.py`, `t83_sua_theo_phanbien.py`, `nen_kham_khung.png`).
- Kết quả phản biện đầy đủ (7 tác tử, từng mục đã kiểm/nghi vấn): `daec676c...\tasks\wp2g4s43v.output`.
