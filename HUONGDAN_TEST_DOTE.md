# HƯỚNG DẪN TEST TỪNG TÍNH NĂNG ĐỢT E (E1→E8) — 14/08/2026

> Test theo ĐÚNG THỨ TỰ dưới đây (mỗi bước phụ thuộc bước trước). Chuỗi commit: E1 `fcdb7f4` · E2 `bfe0817`/`f85a579` · E3 `3b5a92b` · E4 `60af727` · E5 `683cfaf` · E6 `948d7b4` · E7 `8f1cf7e`.
> Công cụ chính: **NPC test** gắn script `\script\test\citywar_e3.lua` (14 nút: xem thành / appoint / kết trận / ép pha 18-19-20-0h / phát lệnh / xem điểm / test kho E2).

## BƯỚC 0 — CHUẨN BỊ (bắt buộc đọc)

1. **Restart GameServer + S3Relay** như thường lệ. Binaries mới đã nằm sẵn `bin\server\Coreserver.dll` (relay + GameServer.exe + client Game.exe KHÔNG đổi — đợt E không đụng protocol).
2. Client: chỉ cần `MapList.ini` mới (E1 đã chép cả PATCHFULL) — client cũ vào bình thường, chỉ cần file này để đi map 221-223.
3. ⚠ **CHƯA thả `timerserver_CUTOVER.lua`** ở giai đoạn test tay. Chỉ thả khi muốn chạy LỊCH THẬT (mục E6-b). Mọi pha đều ép được bằng NPC test, không cần đồng hồ.
4. Gắn NPC test: sửa 1 dòng NPC "Hỗ Trợ Test" trong `startgame.lua` (đổi đường script sang `\\script\\test\\citywar_e3.lua`) HOẶC thêm 1 dòng cạnh đó:
   `AddNpcEx1({1596},1,nil,53,1619*32,3170*32,"","\\script\\test\\citywar_e3.lua","Test Cong Thanh",6)`
5. Chuẩn bị 2 tài khoản + **2 bang** (mỗi bang có bang chủ online). Nhân vật test nên là THÀNH VIÊN BANG ≥5 NGÀY nếu muốn vào trận bằng đường "chính quy" (xem E4-trap); không thì dùng lệnh bài.

## E1 — NỀN DỮ LIỆU + NGẮT HỆ CŨ (phiên trước, `fcdb7f4`)

| # | Test | Kết quả đúng |
|---|---|---|
| 1.1 | Boot xong tìm file `ScriptError.log` MỚI SINH (bin\server + các thư mục script) | Không có lỗi mới; console có `[====== Lua Script StartGame!!! ======]` |
| 1.2 | GM goto lần lượt map **221, 222, 223, 37, 176** | Vào được cả 5; 221 là Kiếm Môn Quan bản vn; 37/176 CÓ THỂ trống NPC (đã báo từ E1 — không phải lỗi) |
| 1.3 | PUBG chạy trọn vòng đời (báo danh → đếm ngược → kết thúc) | Không lặp vô hạn (timer đã dời 12/13→14/15) |
| 1.4 | NPC báo danh CTC CŨ ở Ba Lăng Huyện (NPC 373 balanghuyen) | Mục báo danh công thành cũ **biến mất/không tác dụng** |
| 1.5 | Rương template 625 khắp server | Hiện tên "Rương chứa đồ" (trước rỗng — side-effect đã ghi bàn giao) |
| 1.6 | Bia đá Tương Dương (map 78) | Vẫn thoại được; **thuế cũ ngừng cập nhật theo login** (đường playerlogin đã ngắt) — thuế giờ do hệ MỚI quản (xem E7) |

## E2 — KHO C++ DÙNG CHUNG (phiên trước, `bfe0817`/`f85a579`)

| # | Test | Kết quả đúng |
|---|---|---|
| 2.1 | NPC test → **"Test kho C++ E2"** | `GlbValue(1999)=12345`, `OB pop=777`, `Ladder10999 top1=<tên GM>/111`, `League508 <Phượng Tường> lid>0` (lid>0 chứng minh boot đã tạo league 4/508/509) |
| 2.2 | Restart GS → bấm lại "Test kho C++ E2" | Ladder10999 vẫn còn tên (persist `settings\jx2ladder.txt`); lid 508 vẫn >0 (persist `jx2league.txt`) |
| 2.3 | Console boot | Dòng `[citywar] boot: league 4/508/509 + GlbMission 8 OK, NPC infocenter=<số>` — số phải **>0** |

## E3 — STATE 7 THÀNH (`3b5a92b`)

| # | Test | Kết quả đúng |
|---|---|---|
| 3.1 | NPC test → **"Xem 7 thanh"** | Đủ 7 tên TCVN3 (Phượng Tường→Lâm An), `chu=[] thaithu=[]`, `war=0 signup=0` |
| 3.2 | Đứng TRONG 1 thành (vd goto 78 Tương Dương) → "Xem thanh dang dung" | Ra đúng số thành + câu "Thành … hiện vô chủ." (đứng Ba Lăng Huyện phải ra "không phải thành") |
| 3.3 | (Có bang) đứng trong thành → **"AppointViceroy: bang TA lay thanh"** | Bảng 7 thành hiện `chu=[tên bang] thaithu=[tên bang chủ]` |
| 3.4 | **Restart GS** → "Xem 7 thanh" | Chủ/Thái Thú **còn nguyên** (mirror `settings\jx2citywar.txt`); mở file đó xem có dòng `C/O/M` |
| 3.5 | Client hỏi chủ thành (nếu UI nào dùng): đứng map thành có chủ | Trả về tên bang đệm 2 space; đứng map thường trả rỗng (đổi hành vi — trước hardcode Tương Dương) |
| 3.6 | Hồi quy nhánh bang hội SỐNG DẬY: bảo trì bang có tổng đàn cấp 5 + menu cột totem lãnh địa | Không lỗi ScriptError (giờ `GetCityOwner/GetCityAreaName` là hàm thật, nhánh `tong_setting/city_totempole` chạy trọn) |

## E4 — ENGINE TRẬN (`60af727`) — test gộp trong trận E6-a bên dưới, riêng lẻ:

| # | Test | Kết quả đúng |
|---|---|---|
| 4.1 | Title: sau 3.3, nhân vật bang chủ gõ NPC nào gắn `titlefuncs.lua` (nếu chưa gắn: bỏ qua — Title kiểm ở 6.7) | `Title_GetTitleTab` có id `152+cityid` (Thái Thú …) |
| 4.2 | GetJoinTongTime: nhân vật vào bang <5 ngày đi cổng trận (trap 221) đường "chính quy" | Bị đuổi ra ("Thân phận nghĩa sĩ chưa phù hợp") — dùng LỆNH BÀI hoặc nhân vật bang lâu năm |
| 4.3 | GiveBox: NPC **Sứ Giả Công Thành** (Ba Lăng Huyện, cạnh NPC Hỗ Trợ Test) → "Giao nộp khiêu chiến lệnh" | Mở Ô GIAO VẬT PHẨM; kéo item KHÁC loại vào → bị từ chối lịch sự; kéo Khiêu chiến lệnh → cộng đúng SỐ LƯỢNG (stack 10 = 10 điểm), +50000 exp/lệnh |

## E5 — SCRIPT GỐC + NỐI DÂY (`683cfaf`)

| # | Test | Kết quả đúng |
|---|---|---|
| 5.1 | Boot log KHÔNG có lỗi Lua từ `missions\citywar_*` (2514 script nạp) | Sạch |
| 5.2 | NPC test → **"PHAT 10 Khieu chien lenh"** | Nhận 10 item "Khiêu chiến lệnh" (6,1,1508) — icon/tên hiển thị đúng trên client |
| 5.3 | Xa phu Thành Đô (npcchucnang\xaphu) | Menu có thêm mục **"Đi Chiến trường công thành"** (9 mục); bấm khi CHƯA có trận → im lặng đóng |
| 5.4 | NPC Sứ Giả Công Thành: "Ta muốn xem tình hình báo danh công thành chiến" | Thoại chạy, bảng top-10 lệnh (PushString/AppendString hoạt động) |
| 5.5 | Nộp lệnh khi CHƯA tới pha báo danh | Bị từ chối "không ở giai đoạn báo danh" (cờ league task=0) |

## E6 — VÒNG ĐỜI TRẬN (`948d7b4`) — (a) ÉP PHA BẰNG TAY

> Kịch bản chuẩn 2 bang: bang A là CHỦ thành X (làm bước 3.3 tại thành X), bang B sẽ khiêu chiến.

1. **ÉP 18h** (NPC test, đứng trong thành X): bảng hiện `signup=1`; có tin AddLocalNews "Báo danh công thành chiến … bắt đầu".
2. Nhân vật bang B: **PHÁT lệnh** (nút test) → tới **Sứ Giả Công Thành** nộp (⚠ nộp được cả ngoài 18h-19h THẬT khi đã ép cờ? — flow gốc còn kiểm giờ thật 18-19h ở `sure_signupcitywar:666`; nếu bị chặn giờ, test nộp vào đúng 18h-19h thật hoặc báo tôi nới tạm). Sau nộp: NPC test → **"Xem diem lenh bang TA"** → thành X tăng đúng số lệnh + kho 538 trừ tương ứng.
3. **ÉP 19h**: bang B thành khiêu chiến giả (bảng: thành X có `chu=[A]`, GetCityWarBothSides hiện khi xem summary "Bang [B] sẽ khiêu chiến"); có tin "tranh đua lệnh bài thành công". Nộp 0 lệnh → "thái bình". Thành VÔ CHỦ → bang B được **bổ nhiệm Thái Thú NGAY**.
4. **ÉP 20h**: `war=1` (state ĐANG ĐÁNH). Trong ≤5 phút timer 18 poll → **mission 7 mở trên map 221**: 3 bia đá + 3 cổng thành (tên bang thủ gắn trước tên bia) + 8 dược y + địa đạo + 2 rương; tin toàn server "công thành chiến chính thức bắt đầu".
5. Vào trận: xa phu → "Đi Chiến trường công thành" → chọn phe → map 222 (thủ) / 223 (công) → dẫm trap vào 221. Kiểm: cùng bang thủ/công vào tự do; người ngoài cần lệnh bài đúng loại (354-367).
6. Đánh: phá 1 cổng → tin "một cổng thành của phe thủ đã bị tấn công" + **rào chắn sau cổng MỞ** (đi qua được — fix CHẶN-1); giết người → điểm PK/liên trảm chạy (playerdeath — fix CHẶN-6); phe công phá đủ 3 bia → `GameWin` công thắng; hết 90' bia còn → thủ thắng. (Test nhanh: dùng 2 nút "Ket tran CONG/THU thang" thay vì đánh thật.)
7. Kết trận CÔNG thắng: chủ thành đổi sang bang B; **Title Thái Thú chuyển** từ bang chủ A sang bang chủ B (đổi cả sau restart — boot re-grant); field 48 OccupyCityDay đổi (bang B = TONG_GetDay, bang A = 0); top-10 nhận 3M exp + 5 người ngẫu nhiên nhận Lễ hộp; `war=0`, khiêu chiến rỗng; **trận KHÔNG tự mở lại** (đợi 5-10 phút xác nhận poll im).
8. **ÉP 0h**: league 508/509 dọn member (Xem diem lenh về 0 — kho 538 GIỮ nguyên), trạng thái sót về thường.
9. Restart GIỮA trận (war=1) → boot log "ha state DANG DANH cua thanh …" + `war=0` (chống trận ma).

## E6 — (b) CHẠY LỊCH THẬT (cutover)

1. Chép `serverscript_jx2\citywar_e\timerserver_CUTOVER.lua` → `bin\server\script\timerserver.lua` (GS ĐANG chạy bản mới; hiệu lực sau ~1 phút, không cần restart).
2. Lịch: PT T4/T5 · TĐ T2/T3 · ĐL T3/T4 · BK T6/T7 · TD T5/T6 · DC CN/T2 · LA T7/CN (báo danh 18h, chốt 19h, đánh 20h-21h30, dọn 0h). Hôm nay thứ mấy thì canh thành tương ứng.
3. Kiểm 18h00-18h05 có tin báo danh; 19h00-19h05 có kết quả bốc thăm; 20h00-20h05 war=1.
4. ⚠ Muốn GỠ đồng hồ: chép ngược bản timerserver.lua cũ (git có) — cũng hiệu lực tức thì.

## E7 — THUẾ THÁI THÚ (`8f1cf7e`)

| # | Test | Kết quả đúng |
|---|---|---|
| 7.1 | Nhân vật KHÔNG phải Thái Thú thoại bia Quản Thành (Tương Dương) | Chỉ xem thông tin thành (không có menu thuế) |
| 7.2 | Thái Thú (bang chủ bang chiếm thành) thoại bia NGOÀI 22h-23h → chọn mức thuế | Câu gốc "Trước mắt không cho phép thiết định thuế suất…" |
| 7.3 | Trong 22h-23h: đặt 15% | "Đã thiết đặt thuế suất mới cho thành."; mua bán trên map thành trừ thuế 15% (cả 2 instance nếu map nạp đôi) |
| 7.4 | Đặt lần 2 cùng ngày | "Bạn đã đặt thuế suất của ngày hôm nay!!" |
| 7.5 | Restart → thuế còn nguyên (mirror dòng `C`/`D`) | ✓ |

## HỒI QUY BẮT BUỘC (song song)

- Bang hội đợt 12: bảo trì 06h05, phường sản xuất, kick, chiêu mộ, liên minh, phát tiền — như cũ.
- Tống Kim + songjin shop: **top-1 nhận thưởng KHÔNG còn được cấp title** (đã ngắt chủ đích F9 — báo chủ game biết); điểm tích lũy 751 KHÔNG bị trận công thành ghi đè (đã né sang 2895).
- Kiếm Môn Quan (mission 5) + Lôi Đài BH (mission 6) chạy như cũ (citywar dùng 7/8/9).

## ĐIỂM "ĐỂ Ý" KHI TEST (đã ghi nhận, không phải lỗi mới)

1. Rìa rào chắn (cổng chưa vỡ): client KHÔNG thấy rào → có thể giật kéo-về khi đi sát (VỪA-3).
2. Xe công thành là NPC thường (chưa AI); nếu XE/NPC ra đòn kết liễu CỔNG thì script cổng không kích (JX1 chỉ kích khi NGƯỜI kết liễu) — người chơi nên tự phá phát cuối (VỪA-5).
3. Lệnh bài công/thủ mua ở Sứ Giả **không tự hết hạn 5 ngày** (GetItemLife=0 — có lợi người chơi, ghi nhận).
4. Điều kiện bang cấp ≥18 / ≥37 người: nếu bang test NHỎ vẫn báo danh được thì gate này thuộc phần gốc chưa cưỡng chế phía GS — báo lại để bổ sung.
5. `SetPKFlag(0)` lúc rời trận tắt luôn chế độ PK tự bật của người chơi (THẤP-3).
6. Nộp lệnh flow gốc kiểm GIỜ THẬT 18-19h (độc lập với cờ ép) — test nộp ngoài khung giờ bị chặn là ĐÚNG gốc.
