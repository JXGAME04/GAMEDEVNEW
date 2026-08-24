# ĐẶC TẢ: AUTO TỰ ĐỘNG THAM GIA TỐNG KIM CHO NGƯỜI CHƠI THẬT (WAuto)

> Viết 24/08/2026 theo yêu cầu chủ game: *"làm Auto tự động tham gia tống kim — áp dụng cho
> người chơi thật nên cần phải thay đổi 1 số cái khác so với bot"*.
> Mọi khẳng định về server trong tài liệu này đã **kiểm chứng trực tiếp trên script sống**
> `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script` ngày 23-24/08 (có ghi file:dòng).
>
> **PHẠM VI: 100% phía CLIENT (WAuto). KHÔNG sửa bất kỳ thứ gì phía server.**
> Đường đi của người chơi thật đã hoạt động đầy đủ trên server; nếu trong lúc thi công thấy
> "hình như cần server hỗ trợ" thì DỪNG LẠI HỎI CHỦ GAME, không tự sửa server.

---

## 0. ĐỌC GÌ TRƯỚC — VÀ ĐỪNG NHẦM VỚI CÁI GÌ

| Đọc | Để làm gì |
|---|---|
| `D:\GAMEDEVNEW\PHANTICH_BOT_TONGKIM.md` | Nền tảng luật Tống Kim phía server (lịch, mission, trap, toạ độ). **Nhưng toàn bộ phần "bot phải tự làm bằng C++" trong đó KHÔNG áp dụng cho auto này** — xem mục 3. |
| `D:\GAMEDEVNEW\AUTO_LIENDAU_SPEC.md` | Auto anh em gần nhất (cũng canh lịch → báo danh → đánh). Cách xử lý lịch + tạm dừng auto khác lấy theo khuôn này. |
| `D:\GAMEDEVNEW\BANGIAO_AUTO_DATAU_WAUTO.md` (mục 13) | Cơ chế WAuto đã có sẵn: bấm thoại NPC, chọn dòng thoại, dùng item trong túi, phù di chuyển — auto Tống Kim TÁI DÙNG các cơ chế này, không viết mới. |
| Ký ức `wauto-canonical-tree`, `wauto-luu-cau-hinh-apdata`, `gio-server-mui-gio-wauto`, `feedback-khong-tu-y-doi-cau-truc-auto` | Bốn bẫy WAuto kinh điển — xem mục 6. |

🔴 **ĐỪNG NHẦM #1**: hệ **BOT** Tống Kim (server-side, `KPlayerBot.cpp`, `PB_SetTongKim`, các hàm `pb_Tk*`)
là một dự án KHÁC đã xong. Auto này KHÔNG đụng một dòng nào vào đó. Nếu grep thấy `pb_TkRaTrai`,
`bot_tongkim.lua`… thì đó là đồ của bot — đọc tham khảo được, sửa thì KHÔNG.

🔴 **ĐỪNG NHẦM #2**: nhiều cơ chế bot phải "chế" bằng C++ là vì bot **không có client**. Người chơi
thật CÓ client nên các cơ chế gốc của game **tự chạy**. Chép logic của bot sang WAuto là sai
phương pháp và thừa việc — bảng so sánh ở mục 3 liệt kê đủ.

---

## 1. SỰ THẬT SERVER ĐÃ KIỂM CHỨNG (23-24/08/2026)

### 1.1 Lịch và pha của một trận

- Khung giờ (`lib_tktc.lua:70-82`, bảng `TAB_TIME_TONG_KIM`): **13:23 · 17:50 · 20:50 · 22:50, hằng ngày** — theo **giờ HĐH máy chủ**, KHÔNG phải giờ máy người chơi (xem bẫy múi giờ mục 6.3).
- Cửa sổ báo danh `TIME_BD_TK` = **1 phút** (chú thích trong file ghi "10 phút" là SAI — `lib_tktc.lua:56`). Trận đánh 45 phút (`TIME_KT_TK`).
- "Chính thức bắt đầu" = mission timer 1 hết (`timerserver.lua:699`, `task01.lua:44` loan báo *"Tống Kim đại chiến chính thức bắt đầu!"*).
- Thể thức theo giờ (`lib_tktc.lua:452-459`): giờ 22 → Cửu Sát, còn lại → Bảo Vệ Nguyên Soái. **NHƯNG hiện `RandPThucTongKim` đang hardcode Cửu Sát cả hai nhánh ⇒ mọi trận đều KHÔNG có quái, thuần PvP.** Trận nào cũng có vài trăm bot tham gia sẵn nên luôn có địch để đánh.

### 1.2 Điều kiện tham gia (8 cửa chặn của `mobinhtk.lua`, đúng thứ tự)

1. Trận đang mở (`IsMission`); 2. tên không bị chặn cứng; 3. **đã vào môn phái**; 4. **camp ≠ 4**;
5. phe chưa đủ `PLAYER_MS_LIMIT` = **2000**; 6. **chênh lệch quân số 2 phe < `MAX_PLAYER_CL` = 20**
(đã nới từ 1 hôm 21/08); 7. **cấp ≥ 80** (`LEVEL_ENOUGH_TK`); 8. chưa báo danh phe khác cùng trận.
- Phí 2 vạn hiện **KHÔNG bị thu** (mọi `Pay` đã chú thích) — nhưng auto vẫn nên bảo đảm mang ≥ 2 vạn phòng chủ game bật lại.
- 🔴 **Mọi nhánh từ chối đều `return` TRẦN, KHÔNG có thông báo chắc chắn** ⇒ cách DUY NHẤT biết báo danh thành công là **kiểm tra bản thân đã bị chuyển sang map 379 chưa** (bài học xương máu của dự án bot — đúng nguyên cho auto).

### 1.3 Đường VÀO cho người chơi thật — item "Tống Kim Chiêu Thư"

**Trên server này KHÔNG có NPC báo danh nào đặt trong thành** (hai script
`script\global\ÌØÊâÓÃµØ\ËÎ½ð±¨Ãûµã\npc\*_signup.lua` là đồ thừa bản gốc TQ, không NPC nào dùng —
đã grep toàn bộ script + settings). Người chơi thật vào bằng:

- **Item "Tống Kim Chiêu thư" — Genre 6 / Detail 1 / Particular 154** (`magicscript.txt`, icon `\spr\item\songjin\rescript.spr`), script `script\item\battles\rescript.lua`.
- Dùng item → thoại 3 dòng: `Vào điểm báo danh bên Tống` / `Vào điểm báo danh bên Kim` / `Chưa muốn đi`.
- Chọn xong `NewWorld` tới map **324**: Tống đáp ô **(1529,3196)**, Kim đáp ô **(1592,3075)**.
- 🔴 **Item BỊ TRỪ 1 CÁI mỗi lần dùng thành công** (`rescript.lua:15,21` RemoveItem sau NewWorld) ⇒ auto phải đếm tồn kho, báo người chơi khi sắp hết. Dùng được từ MỌI map (main() không có kiểm map cấm).
- ❓CẦN ĐO C1: nguồn mua Chiêu Thư (shop nào/Kỳ Trân Các/giá) — hỏi chủ game hoặc soi shop, để auto nhắc người chơi trữ sẵn.

### 1.4 Map 324 — đủ 8 NPC chức năng (`startgame.lua:84-96`, thêm lúc boot, TỒN TẠI VĨNH VIỄN)

| Phe | NPC | res | Ô | Script | Vai trò |
|---|---|---|---|---|---|
| Tống | Tống Binh Báo Danh | 62 | (1550,3179) | song_signup | Báo danh (value 1) |
| Tống | Xa Phu | 235 | (1535,3153) | song_transport | **ĐƯỜNG RA khỏi 324** |
| Tống | Shop | 55 | (1546,3158) | song_shop | Mua đồ |
| Tống | Rương chứa | 625 | (1561,3194) | ruongchua | Kho đồ |
| Kim | Kim Binh Báo Danh | 61 | (1555,3082) | jin_signup | Báo danh |
| Kim | Xa Phu | 235 | (1568,3075) | jin_transport | **ĐƯỜNG RA khỏi 324** |
| Kim | Shop | 49 | (1580,3074) | jin_shop | Mua đồ |
| Kim | Rương chứa | 625 | (1593,3094) | ruongchua | Kho đồ |

### 1.5 Thoại NPC báo danh (`mobinhtk.lua:17-81` — auto bấm thoại này)

- Trận **ĐANG MỞ**: Say 2 lựa chọn — dòng 1 = `"Ta muốn đầu quân cho phe Tống"` (NPC Tống) hoặc `"Ta muốn đầu quân cho phe kim"` (NPC Kim — MỖI NPC CHỈ MỜI PHE CỦA NÓ), dòng 2 = `"Kết thúc đối thoại."`.
- Trận **CHƯA MỞ**: Say 1 lựa chọn duy nhất (`"Ta chỉ ghé ngang qua."`) kèm câu giới thiệu CÓ GIỜ MỞ TRẬN (sinh từ `GetTimeTongKimStr()`) ⇒ auto có thể **đọc lịch thật từ chính thoại này** thay vì tin config.
- Muốn đổi phe: đi bộ sang NPC phe kia (~100 ô) hoặc dùng chính Xa Phu 235 (có dòng "Đến điểm báo danh phe Kim/Tống").
- Bấm dòng 1 → `go_tong`/`go_kim` chạy 8 cửa chặn → thành công thì server TỰ: `SetPunish(1)`, `SetPKMode(1,1)` (khoá), `SetCurCamp(1/2)`, **`LeaveTeam()` + `SetCreateTeam(0)`**, `SetRevPos(324)`, `SetTempRevPos(379, hậu doanh)`, `NewWorld(379, hậu doanh)`, `SetTimer(90*18, 2)`, `SetNpcTimeIdle(5*60)`.

### 1.6 Trong hậu doanh (map 379)

- Vị trí 2 phe **ĐẢO NGẪU NHIÊN MỖI TRẬN** (`RandPosTK` → `M_VITRI_TRENDUOI`). Client KHÔNG đọc được biến mission ⇒ auto nhận diện thế trận bằng **toạ độ của CHÍNH MÌNH** (xem 2.4).
- Hậu doanh: Tống-thế-1 ô (1229,3561) · Kim-thế-1 ô (1689,3074) (đảo theo thế trận).
- Quân Y (res 203) cạnh hậu doanh — bán máu. Người thật mua qua shop (`Sale`) BÌNH THƯỜNG (client có đường mua, khác bot). 🔴 **TUYỆT ĐỐI KHÔNG chọn dòng thoại "mua máu nhanh" (`muamaunhanh`) — nó LẤP ĐẦY 100% TÚI** ⇒ hỏng nhặt đồ + kẹt trả nhiệm vụ Dã Tẩu (cần ≥5 ô trống). Giữ túi trống ≥ 5 ô.
- **Ra trận — người thật có HAI đường TỰ ĐỘNG của game, auto gần như không phải làm gì:**
  1. **Đồng hồ 90 giây** (`SetTimer(TIME_IN_TRAI*18,2)` lúc báo danh): hết giờ server TỰ NÉM ra chiến trường (timer id 2 → `timertask\task02.lua` chạy `tong_ratran`/`kim_ratran` cho CÁ NHÂN đó). Đây là đường "lười nhất": auto chỉ việc đứng chờ.
  2. **Nhanh hơn:** đi bộ tới **vết trap ra trại** — 2 cửa toạ độ CỐ ĐỊNH: `(1251,3529)→(1261,3539)` và `(1661,3098)→(1671,3108)` (vết chéo 11 ô, `lib_tktc.lua:648,650`); lấy **cửa GẦN MÌNH NHẤT** (2 phe đảo chỗ nhưng trap không đảo). Người thật giẫm lên là trap TỰ KÍCH: 10 giây đầu sau báo danh nó từ chối kèm thông báo "Sau X giây nữa … vào chiến trường" (`tongratrai.lua:21-27`), hết 10 giây thì cho qua: SetPos 1/8 điểm xuất quân + bất tử 3 giây + gắn death-script.
- Sau khi qua trap lần đầu, `StopTimer()` đã tắt đồng hồ; **mỗi lần CHẾT server ĐẶT LẠI đồng hồ 90s** (`tongtu.lua:21`) ⇒ vòng hồi sinh cũng có 2 đường ra y hệt.

### 1.7 Trong trận

- Địch = người chơi/bot **khác `m_CurrentCamp`** (1=Tống, 2=Kim), cả hai bật FightMode + cờ PK — server đã ép sẵn cho người tham gia. Client CÓ camp của các nhân vật quanh mình (Core client cùng layout — WAuto đọc bộ nhớ như vẫn làm với các trường KNpc khác).
- 🔴 **Bài học trận 23/08 (đã vá cho bot, auto NÊN làm tương tự phía client):** tầm nhìn xuyên tường + chọn "gần nhất tuyệt đối" làm hai đàn **đứng hai bên tường đấm vách vô hạn** (đòn không tới nơi). Auto chọn mục tiêu nên **loại ứng viên bị ô vật cản chắn trên đoạn thẳng nối hai bên** (client có sẵn dữ liệu vật cản map); không thấy ai hợp lệ thì đi tiếp thay vì đứng đánh.
- Không có địch trong tầm → chạy về hướng doanh trại địch theo **bảng toạ độ NPC binh đoàn** (`lib_tktc.lua:163-425`: TONGBINH 78 điểm, KIMBINH 139 điểm — đơn vị Ô; bốc ngẫu nhiên, tới nơi bốc tiếp). Trinh Sát (res 55/49, thu 500 lượng) dịch chuyển ra 3 điểm tiền tuyến — auto dùng được như người nếu muốn nhanh.
- Điểm tích luỹ: giết người +30+30×rank(+×2 Cửu Sát)+30×liên trảm (`tongtu.lua:73-81`); chết KHÔNG mất gì (`SetPunish(1)`).
- 🔴 **Treo 5 phút = bị đá về 324** (`SetNpcTimeIdle(5*60)`, `OnPlayerTimerIdle` trong tongtu/kimtu: đá về 324 ô 1541,3178 / 1570,3085 + LƯU điểm vào task) ⇒ auto phải luôn có hành động; nếu lỡ bị đá giữa trận: NPC báo danh ở NGAY CẠNH chỗ đáp → báo danh lại là vào tiếp.

### 1.8 Chết và hồi sinh

- Chết → client hiện hộp hồi sinh (người thật phải BẤM — WAuto cần tự bấm; kiểm tra WAuto đã có sẵn "tự hồi sinh" chưa — CẦN ĐO C4) → sống lại TRONG HẬU DOANH (`SetTempRevPos`), đồng hồ 90s được đặt lại, trap giữ 10 giây đầu như lượt mới.
- Vòng lặp mỗi mạng: hồi sinh → (tuỳ chọn mua thuốc) → ra cổng (đường 1.6).

### 1.9 Kết thúc trận

- Server (task03) TỰ trả nguyên trạng: PKMode(0,0), FightState(0), Punish(0), camp gốc, cho lập nhóm lại, xoá death-script, `SetNpcTimeIdle(0)`… và `NewWorld` mọi người về **324**: Tống (1543,3177), Kim (1576,3087). Điểm cao có thưởng (top nhận Xu + exp).
- 🔴 **Map 324 nằm trong danh sách CẤM Thần Hành Phù** (`shenxingfu.lua:246, :325`) — KHÔNG phù trực tiếp từ 324 được.
- **Đường RA đúng: Xa Phu Tống Kim (res 235) ngay cạnh** (`xaphu.lua`): menu gồm `Những nơi đã đi qua/WayPointFun`, `Những thành thị đã đi qua/StationFun`, **`Trở lại chỗ lúc nãy/TownPortalFun`**, `Đến điểm báo danh phe kia`, `Không cần đâu`.
  - Khuyến nghị: thử **"Trở lại chỗ lúc nãy"** (về đúng chỗ trước khi vào — đẹp nhất cho "quay lại luyện công"); nếu neo của TownPortal không như kỳ vọng (CẦN ĐO C2) thì fallback "Những thành thị đã đi qua" → về thành → nối vào luồng **phù-về-bãi có sẵn** của WAuto (đã dùng cho Dã Tẩu).

---

## 2. NHỮNG THỨ AUTO PHẢI TỰ LO (không có sẵn trong game)

1. **Lịch hẹn giờ**: config các khung giờ (mặc định 13:23/17:50/20:50/22:50) + "đi sớm T−N phút" (mặc định 2). Giờ so theo **giờ máy chủ** (mục 6.3). Có thể đối chiếu lịch thật bằng thoại NPC lúc trận chưa mở (mục 1.5).
2. **Chuỗi hành động**: dùng Chiêu Thư → (đáp 324) → tới NPC phe đã chọn → bấm thoại → xác nhận đổi map → hậu doanh → mua thuốc (tuỳ chọn) → ra cổng → đánh/di chuyển → chết-hồi sinh lặp → hết trận → Xa Phu rời 324 → trả lại hoạt động cũ.
3. **Phát hiện từ chối im lặng** (mục 1.2): bấm xong đợi ~2 giây, còn ở 324 ⇒ bị chặn. Xử lý theo config phe: `Tống cố định` / `Kim cố định` / `Tự cân bằng` (đọc quân số 2 phe từ CHÍNH câu thoại NPC — "bên Tống: X người, bên Kim: Y người" — parse từ text thoại, khỏi cần đọc mission); tự cân bằng thì sang NPC phe ít hơn. Thử lại giãn ≥3 giây, quá ~10 lần thì bỏ khung giờ này, log lý do.
4. **Nhận diện thế trận (thay `GetMissionV(47)` của bot)**: sau khi qua trap, mình bị SetPos vào 1/8 điểm xuất quân của phe mình. So toạ độ bản thân với 2 bộ 8 điểm:
   - Bộ A (tongratrai): (1308,3461)(1311,3442)(1324,3428)(1337,3420)(1352,3419)(1350,3439)(1342,3463)(1321,3468)
   - Bộ B (kimratrai): (1585,3181)(1570,3173)(1551,3175)(1544,3204)(1550,3223)(1569,3227)(1586,3216)(1592,3198)
   Rơi gần bộ A ⇒ mình ở nửa Tây-Nam, địch ở Đông-Bắc (dùng bảng KIMBINH làm hướng tiến); gần bộ B thì ngược lại. (Cách khác tương đương: so vị trí hậu doanh mình hồi sinh với 2 toạ độ hậu doanh.)
5. **Trạng thái trước/sau**: lưu (map, toạ độ, chế độ auto đang chạy) trước khi đi; sau trận khôi phục — tạm dừng Dã Tẩu/Liên Đấu/bán rác trong lúc TK, resume sau (khuôn ưu tiên lấy theo AUTO_LIENDAU_SPEC).

---

## 3. BẢNG KHÁC BIỆT BOT ↔ AUTO NGƯỜI THẬT (mấu chốt chống nhầm lẫn)

| Việc | BOT (server C++) đã phải làm | AUTO người thật (WAuto) làm |
|---|---|---|
| Vào điểm báo danh | `ChangeWorld(324, …)` thẳng bằng engine | **Dùng item Chiêu Thư (6/1/154)** — tốn 1 cái/lần |
| Báo danh | `ExecuteScript go_tong/go_kim` + tự đặt `TaskTemp`, tự vá `NpcValue` NPC Kim | **Bấm thoại NPC như người** — main(sel) của NPC tự lo NpcValue, KHÔNG cần vá gì |
| Xếp hàng cân bằng | Phải tự xếp hàng 1 con/nhịp (500 con cùng bấm) | KHÔNG cần — 1 người; chỉ cần retry + đổi phe khi bị từ chối im lặng |
| Trap ra trại | Bot MIỄN trap ⇒ C++ phải tự gọi kịch bản + tự gate theo mission timer 1 | **Trap tự kích khi giẫm lên**; server tự giữ 10 giây; KHÔNG cần biết mission timer |
| Đồng hồ 90s ném ra trận | CHẾT với bot (KPlayer::Active return sớm) ⇒ C++ tự đếm | **SỐNG với người thật** — không làm gì cũng được ném ra đúng hạn |
| Biết thế trận đảo | Đọc `GetMissionV(M_VITRI_TRENDUOI)` trong C++ | Suy từ toạ độ bản thân (mục 2.4) — client không đọc được mission |
| Mua máu Quân Y | Đường shop `Sale` CHẾT với bot ⇒ hàm riêng `bot_tk_muamau` | **Mua shop bình thường**; cấm dòng "mua máu nhanh" (đầy túi) |
| Hồi sinh khi chết | C++ `Revive()` cưỡng bức sau 1 giây | **Tự bấm nút hồi sinh trên UI client** |
| Quân số 2 phe | Đọc `GetGroupPlayerCount` từ mission | Parse từ câu thoại NPC báo danh |
| Rời 324 sau trận | Thần Hành Phù qua `gopos_step3lvXX` (gọi thẳng hàm, né kiểm map) + fallback ChangeWorld | **KHÔNG phù được (324 cấm phù)** → thoại **Xa Phu res 235**: "Trở lại chỗ lúc nãy" / "Những thành thị đã đi qua" |
| Chống đánh xuyên tường | Đã vá server-side cho bot (`pb_ThayDuoc`) | Client tự lọc mục tiêu bị tường chắn (nên có — mục 1.7) |
| Nhóm/đội | C++ phải chặn ghép nhóm 2 đầu | Server tự `LeaveTeam` khi báo danh — auto chỉ cần BIẾT là đội hình follow sẽ tan (mục 6.5) |

---

## 4. LUỒNG MÁY TRẠNG THÁI ĐỀ XUẤT (A0→A8)

```
A0 CHỜ LỊCH   : tới (giờ trận − N phút) & bật công tắc → A1. Kiểm: đủ cấp 80, có phái,
                có Chiêu Thư (không đủ điều kiện → báo user, bỏ khung giờ).
A1 DỌN MÌNH   : lưu trạng thái cũ; tạm dừng auto khác; nếu đang trong nhóm → tự rời trước
                cho gọn (server cũng sẽ ép rời); dọn túi còn ≥5 ô (bán rác nếu user bật).
A2 DÙNG THƯ   : dùng Chiêu Thư → chọn dòng theo phe config → xác nhận map == 324.
A3 BÁO DANH   : đi bộ tới NPC phe mình (toạ độ mục 1.4) → bấm thoại → chọn dòng 1
                → đợi 2s → map == 379 ? A4 : (retry / đổi phe / quá hạn → A8-lỗi).
                Trận chưa mở (thoại chỉ 1 dòng) → đứng chờ, thử lại mỗi 10s tới giờ+2'.
A4 HẬU DOANH  : (tuỳ chọn) tới Quân Y mua máu qua shop (số lượng config, KHÔNG mua-nhanh)
                → đi tới VẾT TRAP GẦN NHẤT (2 cửa mục 1.6) → đứng đó. Trap tự cho qua
                (hoặc 90s tự bị ném). Map-position đổi sang vùng xuất quân → A5.
A5 CHIẾN ĐẤU  : vòng: chọn địch khác camp gần nhất CÓ đường nhìn → đánh; không có →
                di chuyển tới điểm ngẫu nhiên bảng binh đoàn phe địch; luôn giữ hành động
                (chống idle-kick 5'). Máu/thuốc theo hệ uống thuốc sẵn có của WAuto.
A6 CHẾT       : bấm hồi sinh → về hậu doanh → lặp A4 (mua thuốc lại nếu config).
A7 HẾT TRẬN   : phát hiện bị chuyển về 324 (mà không phải mình bấm) → tới Xa Phu 235
                → "Trở lại chỗ lúc nãy" (fallback: thành đã qua → phù về bãi) → A8.
A8 TRẢ LẠI    : khôi phục auto/chế độ cũ; ghi log tổng kết (số mạng, điểm nếu parse được);
                về A0 chờ khung giờ sau.
Bất kỳ pha nào phát hiện mình ở 324 ngoài ý muốn (idle-kick): nếu trận còn mở → quay lại A3;
trận đóng → A7.
```

Mỗi pha có **thời hạn** (như `PB_TK_PHA_HAN` của bot): kẹt quá 2-3 phút ở một pha → bỏ về A8
+ log — tuyệt đối không để auto kẹt vòng lặp bấm thoại vô hạn.

---

## 5. TOẠ ĐỘ DÙNG TRỰC TIẾP (đơn vị Ô; MPS = Ô×32)

| Thứ | Toạ độ |
|---|---|
| Đáp 324 từ Chiêu Thư | Tống (1529,3196) · Kim (1592,3075) |
| NPC báo danh 324 | Tống (1550,3179) · Kim (1555,3082) |
| Xa Phu ra khỏi 324 | Tống (1535,3153) · Kim (1568,3075) |
| Bị đá idle về 324 tại | (1541,3178) / (1570,3085) |
| Hết trận về 324 tại | Tống (1543,3177) · Kim (1576,3087) |
| Trap ra trại (379, cố định, vết chéo 11 ô) | (1251,3529)→(1261,3539) · (1661,3098)→(1671,3108) |
| Hậu doanh (379, đảo theo thế trận) | (1229,3561) · (1689,3074) |
| Quân Y (379, MPS, đảo theo thế trận) | (1249×32, 3557×32) · (1700×32, 3066×32) |
| 8+8 điểm xuất quân (nhận diện thế trận) | mục 2.4 |
| Bảng binh đoàn (đích di chuyển) | `lib_tktc.lua:163-242` (TONGBINH 78 điểm) · `:244-384` (KIMBINH 139 điểm) — chép vào WAuto dạng bảng hằng |

---

## 6. SÁU LƯU Ý WAUTO — PHIÊN THI CÔNG PHẢI THUỘC

1. **Cây WAuto đúng**: có BA cây WAuto của HAI dự án (ký ức `wauto-canonical-tree`). Trước khi sửa: `wc -l WAuto.cpp` đối chiếu theo ký ức + hỏi chủ game nếu lệch. Cwd phiên này thường là `J:\CayChay\Src_Auto_Ngoai\WAuto\WAuto`. Build theo khuôn `auto-ngoai-port`: sửa + build sandbox → copy, KHÔNG build đè chỗ khác.
2. **Cấu trúc lưu cấu hình**: WAuto ghi **NGUYÊN STRUCT `autoconfig`** vào `APdata\<ID NHÂN VẬT>.dat` (ký ức `wauto-luu-cau-hinh-apdata`). ⇒ trường config mới của Tống Kim PHẢI THÊM VÀO CUỐI struct + xử lý đọc file cũ thiếu trường (mặc định tắt) — thêm vào giữa là NÁT config của toàn bộ người chơi cũ.
3. **Múi giờ**: lịch là GIỜ MÁY CHỦ; ký ức `gio-server-mui-gio-wauto` từng dính bẫy hardcode UTC−5 trong khi máy chủ Pacific UTC−8. Cho user config offset hoặc lấy giờ từ nguồn server (thoại NPC mục 1.5 là nguồn kiểm chứng rẻ nhất).
4. **UI cấu hình theo luật** `feedback-khong-tu-y-doi-cau-truc-auto`: mọi hành vi phải có Ô CẤU HÌNH đàng hoàng, không chôn hằng số: bật/tắt tổng, 4 checkbox khung giờ, phút đi sớm, chọn phe (Tống/Kim/Tự cân), mua máu (số bình, 0 = không mua), có quay lại chỗ cũ sau trận không.
5. **Va chạm tính năng sẵn có**: (a) auto-follow/đội: server ÉP RỜI NHÓM khi báo danh — nếu người chơi bật "theo đội trưởng" thì auto TK phải hoặc từ chối chạy hoặc báo rõ; (b) Dã Tẩu đang giữa nhiệm vụ: tạm dừng, KHÔNG huỷ nhiệm vụ; (c) auto bán rác/dọn túi chạy TRƯỚC khi đi (giữ ≥5 ô); (d) trong TK tắt nhặt đồ lung tung nếu túi gần đầy.
6. **Không đụng server / không đụng đồ bot**: nhắc lại lần cuối — toàn bộ thay đổi nằm trong WAuto. Grep thấy `pb_Tk*`/`PB_SetTongKim`/`bot_tongkim.lua` = đồ của BOT, chỉ đọc.

---

## 7. CẦN ĐO TRƯỚC KHI GÕ CODE (30 phút trong game)

| # | Đo gì | Cách |
|---|---|---|
| C1 | Nguồn mua "Tống Kim Chiêu Thư" (6/1/154) + giá | Hỏi chủ game / soi Kỳ Trân Các + shop 324 |
| C2 | "Trở lại chỗ lúc nãy" (TownPortalFun) neo về đâu sau chuỗi Chiêu Thư → trap → chết nhiều lần | Đi 1 trận thật, bấm thử sau trận |
| C3 | Hộp thoại client hiển thị các dòng Say theo THỨ TỰ server gửi — xác nhận index dòng ổn định để auto bấm theo index (hay phải khớp text) | Soi UI thoại 1 lần |
| C4 | WAuto đã có "tự bấm hồi sinh" chưa (Dã Tẩu/luyện công có chết bao giờ chưa?) | Grep WAuto: revive/hồi sinh |
| C5 | Sau `LeaveTeam` cưỡng bức lúc báo danh, trạng thái follow/đội của WAuto có tự gỡ sạch không | Test 2 acc |
| C6 | Idle-kick 5 phút: đứng yên thử để xem client nhận gì (để auto phát hiện "mình vừa bị đá") | 1 lần đứng im |

---

## 8. NGHIỆM THU

1. Đúng giờ (cả 4 khung, đổi giờ config được), tự đi, tự báo danh, vào trận không cần người bấm gì.
2. Bị từ chối cân bằng → tự đổi phe (nếu config Tự cân) hoặc retry, không kẹt thoại.
3. Cả trận không dính idle-kick; chết bao nhiêu mạng cũng tự quay lại trận.
4. Không đánh xuyên tường đứng lì; không mua-nhanh đầy túi; túi luôn ≥5 ô trống.
5. Hết trận ≤1 phút sau khi về 324 đã rời map qua Xa Phu và quay lại đúng hoạt động cũ (bãi cũ, chế độ cũ, Dã Tẩu tiếp tục).
6. Hết Chiêu Thư / thiếu cấp / chưa vào phái → báo người chơi rõ ràng, không lặp vô hạn.
7. Config cũ của người chơi (file .dat cũ) đọc lên không hỏng, tính năng mặc định TẮT.

---

## PHỤ LỤC — FILE THAM CHIẾU (đều thuộc `E:\...\bin\server\script`)

| File | Nội dung |
|---|---|
| `item\battles\rescript.lua` | Item Chiêu Thư vào 324 |
| `tinhnang\tong_kim_tcap\mobinhtk.lua` | Thoại báo danh + 8 cửa chặn + go_tong/go_kim |
| `tinhnang\tong_kim_tcap\lib_tktc.lua` | Toàn bộ hằng số, lịch, toạ độ, trap |
| `tinhnang\tong_kim_tcap\xaphu.lua` | Xa Phu ra khỏi 324 |
| `tinhnang\tong_kim_tcap\tongtu.lua` / `kimtu.lua` | Chết/hồi sinh, idle-kick, tính điểm |
| `maps\tongkim\trap\tongratrai.lua` / `kimratrai.lua` | Trap ra trại + giữ 10 giây |
| `timertask\task01/02/03.lua` | Bắt đầu chính thức / ném ra trận / kết thúc |
| `item\ib\shenxingfu.lua:246,325` | Danh sách map CẤM phù (có 324) |
| `startgame.lua:84-96` | 8 NPC trên 324 |
