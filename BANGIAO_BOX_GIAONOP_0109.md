# BÀN GIAO — BOX GIAO NỘP DÙNG CHUNG (01/09) — commit `ff09b7ce`

Chủ báo 2 lỗi; phản biện bắt thêm 1 lỗi. Cả 3 đã vá, build sạch 3 nhị phân, phản biện 3 tác tử 3/3 AN TOÀN.

## 1. Lỗi A [BOXMAU] — ô mô tả hiện nguyên văn `<color=red>` / `<color>`

- **Gốc**: `KUiAffairItem::OpenWindow` (UiAffairItem.cpp:47) đổ chuỗi Lua **thô** vào `KWndMessageListBox::AddOneMessage` (WndMessageListBox.cpp:121 chỉ memcpy). Nơi duy nhất dịch tag màu → byte điều khiển là `TEncodeText` (Engine\Src\Text.cpp:458+); bộ vẽ không bao giờ phân tích `<`. Đối chứng đúng: UiCompoundItem.cpp:641 có gọi TEncodeText nên màu lên.
- **Vá**: hàm mới `UiAffair_EncodeDesc` (UiAffairItem.cpp, khai báo UiAffairItem.h) = nắn thẻ sau chữ Việt (khuôn DTG_FixTagAfterVn UiTaskGuide.cpp:774) + nắn **byte Việt lẻ cuối chuỗi** (TEncodeText drop — Text.cpp:476) + TEncodeText. Gọi tại 3 box: UiAffairItem / UiMantleInlay / UiMantleWash. **Chỉ client.**
- Sau mã hoá chuỗi có thể chứa byte 0 (RGB) → phải dùng độ dài trả về, **cấm strlen**.

## 2. Lỗi B [BOXSOT] — item trôi giữa các tính năng dùng chung box

- **Gốc**: KHÔNG tồn tại "danh sách đồ của box". Đồ thả vào box nằm **thẳng trong m_ItemList** với `nPlace = pos_affairitem` (bền vững qua logout). Cả 4 đường mở box (LuaGiveItemUI :832 / OpenGiveBox ScriptFuns:3039 / InlayBox :2469 / WashBox :2492) không dọn; `GiveBoxCollect` (:877) quét **mọi** món pos_affairitem không lọc phiên → đồ sót Dã Tẩu bị tính vào box Phi Phong.
- **Vá** (chỉ server, không đổi giao thức): `KJx2WarInfra_ClearAffairBox` gọi tại 4 điểm mở box, dọn đồ sót về túi. **3 tầng an toàn không mất đồ**:
  1. `CheckCanPlaceInEquipment` **trước** rồi mới `Remove`; túi đầy → **để nguyên** trong box;
  2. `AddKIL` fail (=0) → trả về **đúng ô cũ**;
  3. **CẤM** `KPlayer::RecoveryBox` (túi đầy → pos_hand → món đang cầm bị ném xuống đất, KPlayer.cpp:6680; cảnh báo sẵn CoreShell.cpp:3722).
- **Cờ tái nhập `sInGiveCb`** (đặt quanh ExecuteScript của UiCommandScript case 1, KProtocolProcess.cpp): `PF_InlayMoLai`/`PF_MoLaiWashBox`/`PF_MoLaiHopNangCap` CỐ Ý mở lại box khi đồ còn trong khay — lúc đó không được dọn. Thiếu cờ này = giật Phi Phong khỏi khay giữa chu trình khảm/tẩy. Reset cờ ở khe player tái sử dụng (KPlayer.cpp:300).
- **Đổi hành vi có chủ đích**: mở lại box từ **menu** (qua c2s_playerselui, không qua case 1) giờ sẽ dọn khay — người chơi đặt lại đồ. Thiết kế cũ [VA 31/08b] "mở lại panel hiện lại y trước" chỉ còn đúng trong callback.

## 3. Lỗi C [WASHFIX] — nút "Giữ nguyên"/"Áp dụng" box tẩy luyện chạy nhầm

- **Gốc**: client gửi `GOI_ADD_UI_CMD_SCRIPT nType=1` kèm `szFunc="doWashKeep"/"doWashApply"` (UiMantleWash.cpp:242/249), nhưng case 1 xưa nay **vứt szFunc**, luôn chạy `m_szTaskExcuteFun` (= `doWashRoll` do PF_MoLaiWashBox đặt) → bấm "Giữ nguyên" = **tẩy lại + trừ nguyên liệu**.
- **Vá**: whitelist đúng 2 tên `doWashKeep`/`doWashApply` trong case 1 (không chạy szFunc tuỳ ý — chặn client hack gọi hàm bất kỳ).

## 4. Nghiệm thu (sau restart với bộ .moi mới)

1. Mở box "Phi Phong tăng sao": mô tả **có màu đỏ** đoạn cảnh báo, không còn chữ `<color`.
2. Bỏ nguyên liệu vào box Dã Tẩu (ghép Sát Thủ Giản) → đóng → mở box Phi Phong tăng sao: **khay trống**, nguyên liệu về túi.
3. Box khảm nạm mở lên: khay trống nếu trước đó có đồ sót; giữa chu trình khảm (khảm xong mở lại) Phi Phong **vẫn nằm trong khay**.
4. Tẩy luyện: bấm "Tẩy tiếp" tốn nguyên liệu như cũ; bấm **"Giữ nguyên"** → đóng không mất thêm nguyên liệu, dòng ẩn giữ cũ; **"Áp dụng"** → nhận dòng mới.
5. Túi chật kín: đồ sót **không biến mất** — vẫn nằm trong khay, không rơi xuống đất.

## 5. Điểm tồn (nhẹ, chưa sửa — cân nhắc đợt sau)

- Khay dọn tối đa 64 món/lần (khay 6×4=24 ô nên thực tế đủ).
- `UiAffairItem.cpp:50` strcpy szAction1[64] vào szFunc1[32]; ScriptFuns.cpp:3072-3074 strcpy không chặn độ dài (tồn tại từ trước, chưa đụng).
- Chuỗi 2 cột dòng ẩn của UiMantleWash (`LayDongAn`) chưa qua encode — hiện không chứa tag nên chưa cần.

## 6. Bối cảnh phối hợp đa phiên (01/09 sáng)

- `KPlayerBot.cpp` +155/−43 **chưa commit** = vá bot/TK chờ restart của phiên bot — **cố ý build kèm** (mọi bản CoreServer từ sáng đều chứa; build thiếu nó = regress bot). Không stash, không commit hộ.
- Bộ .moi 08:55 (phiên pet) chứa snapshot **dở dang** của box (4/7 tệp) — đã bị bộ mới đè.
