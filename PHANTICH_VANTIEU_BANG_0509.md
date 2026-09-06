# PHÂN TÍCH VẬN TIÊU BANG (ÁP TIÊU BANG) — BẢN LINUX, 05/09/2026

Chủ yêu cầu: *"tìm tính năng vận tiêu bang ở bản linux và phân tích"*. Đây là **báo cáo khảo sát**, chưa thi công dòng nào.

Tên gốc: **Long Môn Tiêu Cục** (龙门镖局). Hệ này có **hai nhánh dùng chung một bộ mã**:

| Nhánh | Sao | Ai làm | Nguồn |
|---|---|---|---|
| Vận tiêu **cá nhân** | 1–9 sao | mọi người chơi | `script\event\longmenbiaoju\` (23 tệp) |
| **Vận tiêu BANG** | **10 sao** | bang chiếm thành | `script\activitysys\config\129\` (15 tệp) ← **cái chủ hỏi** |

Cả hai **đang chạy thật** bên Linux (0 tệp bị chú thích 100%).

---

## 1. Vị trí tệp

| Vai trò | Đường dẫn | Dòng sống |
|---|---|---|
| **Bộ điều khiển vận tiêu bang** | `server1\script\activitysys\config\129\` (15 tệp) | **2.545** |
| Logic chính | `129\extend.lua` | 1.202 |
| NPC giao tiêu bang | `129\npc_consigner.lua` ("Tiếp Dẫn áp Tiêu Bang") | 427 |
| NPC nhận tiêu bang | `129\npc_receiver.lua` ("Nhận Hàng áp Tiêu Bang") | 121 |
| Xe tiêu | `129\npc_lmbiaoche.lua` | 111 |
| Rương tiêu vật bang | `129\npc_lmbiaobox.lua` | 97 |
| Bảng thưởng | `129\award.lua` | 163 |
| Móc engine xe tiêu | `server1\script\global\yunbiao_system.lua` | 74 |
| Dữ liệu liên server | `server1\script\event\longmenbiaoju\event.lua` (chạy trên relay) | — |
| Nơi nộp rương + nhận thưởng nhóm | `server1\script\tong\npc\jitan.lua` (tế đàn bang) dòng 119/121, 592–625 | — |
| Lịch tăng thưởng | `gateway\s3relay\relaysetting\task\longmenbiaoju.lua` (mỗi giờ; nhân đôi 13–14h và 19–20h) | — |
| Bảng lộ trình tham chiếu | `server1\settings\event\longmenbiaoju\route.txt` (26 tuyến 1–9 sao) | — |

---

## 2. Luật chơi (đọc từ mã, không suy đoán)

**Mở hoạt động** — chỉ **bang chủ** (`npc_consigner.lua:57-122`):
- Chỉ **thứ Bảy và Chủ nhật**, **12:00–23:00** (kiểm hai lần, cả lúc bấm và lúc xác nhận).
- Bang phải **đang chiếm thành**: `GetCityOwner` của đúng bản đồ đang đứng phải là bang mình, nếu không NPC từ chối nói chuyện (`main()` + `GetOccupyTongId`, `extend.lua:1087-1106`).
- Số lượt tiêu trong tuần = `floor((số thành chiếm + 1) / 2)`.
- Mở xong khoá luôn việc **bang chủ thoái vị** trong tuần đó (`CheckCanChangeMaster`).

**Nhận tiêu** — **bang chủ hoặc trưởng lão** (`RecvTongBiaoChe`, `:124-199`):
- Cấp ≥ 150 hoặc đã chuyển sinh; **vào bang đủ 7 ngày**; đang đứng cạnh NPC (lệch < 600 đơn vị).
- **Mỗi bang chỉ một xe cùng lúc**: chặn bằng `TONG_TASK_YABIAO_TIME`, ai nhận trong vòng 30 phút thì người khác không nhận được.
- Tuyến đi **random cố định trước** (`TONG_TASK_YABIAO_RAND` 1–4), mỗi thành 3–4 tuyến, đích là các hang động quanh thành.
- Nhận xong: **thông báo toàn server** ("Bang X-Y tại Z thành công nhận áp Tiêu Bang 10 Sao") qua `AddGlobalNews` + `LG_ApplyDoScript` sang liên server.

**Áp tiêu** — 30 phút (`refreshBiaoChe(30*60)`, `extend.lua:1018-1053`):
- Xe = NPC template **2233**, tên "Tiêu Xa bang của [tên bang]tên người", tham số NPC 1=cấp, 2/3=hash tên người, 5/6=hash id bang.
- Người áp tiêu được **truyền tống tới xe**, CD tăng dần **+20 giây mỗi lần, trần 60 giây** (`GoToTongBiaoChe`).
- **Không được rời bang / không bị đá khỏi bang** khi đang áp tiêu (`CheckCanLeaveTong`, `CheckCanKick`).
- Không được bỏ nhiệm vụ khi xe còn sống (`CancelTongTask`).
- Xe đổi bản đồ → **loa toàn server** báo vị trí (`yunbiao_system.lua:36-74`) → đây chính là cơ chế mời PvP.
- Cứ **1 phút** (`SetNpcTimer 18*60`) xe quét người trong bán kính 25 và cộng **4.000.000 kinh nghiệm** cho thành viên **cùng bang, đã vào bang ≥ 7 ngày, đang ở trạng thái chiến đấu** (`AddFollowExp`).

**Cướp tiêu (PvP)** — `npc_lmbiaoche.lua:98-134`:
- Xe bị phá → sinh **"Rương Tiêu Vật Bang"** (NPC 2232) tại chỗ, tồn tại 15 phút.
- **Bảo hộ 3 phút**: chỉ bang chủ xe và bang kẻ cướp nhặt được; sau 3 phút ai cũng nhặt (`CanPickBiaoWu`).
- Nhặt tối đa **6 lượt**, mỗi lượt 1 rương, có thanh tiến trình 4 giây; mỗi lượt **loa toàn server**.
- Bang mất xe nhận tin nhắn bang: "Tiêu Xa Bang của quý bang tại ... bị hủy."

**Nộp tiêu thành công** — `npc_receiver.lua:30-90`:
- Phải đúng NPC đích của tuyến, xe phải còn sống và **cách NPC < 600**.
- Thưởng **8 Rương Tiêu Vật Bang** (`tbAllCountCell[10] = {8, 1}`), hạn dùng đến cuối tuần.

**Đổi thưởng** — tại **tế đàn bang** (`jitan.lua`):
- Ai cũng **nộp rương** vào quỹ bang (`HandInBiaoCheBox` → biến bang `TONG_TASK_YABIAO_AWARD`), rương quá hạn bị xoá.
- **Bang chủ** nhận thưởng nhóm, tối đa 50 lượt/lần, mỗi lượt cần 5 ô túi. Thưởng mỗi lượt (`award.lua:164-170`): Đằng Long Thạch-Hạ ×10, Huyết Long Đằng cấp 12 ×5, Lệnh bài Cổ Tháp-Bạc, Tinh Ngọc Nguyên Thạch, Thề Non Hẹn Biển.
- **Thưởng cá nhân hộ tiêu** tại NPC nhận hàng, theo số phút bám xe trong tuần (`getAward`): < 5 lượt = Cẩm nang thay đổi trời đất; 5–11 = Thưởng Tận Trung; ≥ 12 = Thưởng Trung Thành. Lưu 4 ô đếm `3542–3545`.

**Biến bang hội dùng** (`variables.lua:36-43`): `1149` số thành chiếm tuần, `1150` trạng thái mở, `1151` số lượt còn lại, `1152` mốc thời gian nhận, `1153` số rương trong quỹ, `1154` random tuyến, `1155/1156` hash tên người đang áp tiêu. Tất cả **tự xoá theo tuần** bằng thủ thuật `năm+tuần × 100 + giá trị` (`GetTongWeekTaskValue`).

---

## 3. Đối chiếu với JX1 — cái gì đã có, cái gì thiếu

### 3.1 Đã có sẵn, dùng được ngay
- **Bảng NPC đã có đủ 4 NPC**: `npcs.txt` JX1 id **2230** "Tiếp Dẫn áp Tiêu Bang", **2231** "Nhận Hàng áp Tiêu Bang", **2232** "Rương Tiêu Vật Bang", **2233** "Tiêu Xa Bang 10 sao" — trùng khớp Linux, không phải thêm dòng nào. (Ngược lại dải 2145–2158 của vận tiêu **cá nhân** ở JX1 đang trống.)
- **Hàm bang hội engine đã có**: `TONG_GetTaskValue`, `TONG_ApplySetTaskValue`, `TONG_ApplyAddTaskValue`, `TONG_Name2ID`, `GetCityOwner`, `GetJoinTongTime`, `String2Id`, `Msg2Tong`, `AddGlobalNews`, `LG_ApplyDoScript`, `GetNpcAroundPlayerList/NpcList`, `Set/GetNpcParam`, `AddNpcEx`, `DynamicExecuteByPlayer`, `RemoteExecute`, `AddStatData`.
- **Thư viện script đã có**: `activitysys` (khung hoạt động + `playerfunlib`), `awardtemplet`, `objbuffer_head`, `progressbar`, `misc\taskmanager`, `dailogsys`.
- **18 task id đều TRỐNG** (`3500,3501,3502,3506,3508,3540,3541,3542,4178–4187`) — dùng nguyên si được.

### 3.2 Thiếu — và đây là khối lượng thật

**A. Sáu hàm xe tiêu là ENGINE C++, JX1 không có gì tương đương** (grep `Core\Src\*.cpp` = 0):

| Hàm | Việc |
|---|---|
| `CreateBiaoChe` | sinh xe đi theo người chơi, tự tìm đường |
| `DeleteBiaoChe` | xoá xe khi nộp thành công |
| `GetBiaoChePos` | vị trí xe (bản `event.lua` nhận thêm tên người để tra **liên server**) |
| `IsBiaoCheAlive` | xe còn sống không |
| `SyncBiaoCheDeathInfoToRelay` | báo relay khi xe chết |
| `WriteYunBiaoLog` | ghi `Logs\KSG_YunBiaoLog_*.txt` |

Kèm 4 hàm gọi ngược từ engine: `OnBiaoCheDisapper`, `OnBiaoCheFarAwayPlayerDisapper`, `OnBiaoCheChangeMapNotice`, và luật "xe rời người chơi 750m quá giờ thì biến mất". **Đây là phần bắt buộc viết C++**, không thay bằng Lua được, vì xe là NPC tự bám người qua nhiều bản đồ và tồn tại độc lập với người chơi.

**B. Bốn hàm bang hội nhỏ — viết bằng Lua được**: `CheckIsMaster`, `CheckIsElder`, `CheckHasTong`, `GetTongFigureStr` (JX1 có `TONGM_GetFigure` / `GetTongName` để dựng lại).

**C. Ba hàm còn lại**: `SetNpcCurLife`, `GetPlayerEnteringTrap`, `SetPlayerEnteringTrap` — chỉ dùng ở nhánh cá nhân, nhánh bang không cần.

**D. Bốn tệp include JX1 chưa có**: `global\maplist.lua` (bảng tên bản đồ, dựng lại dễ), `tong\tong_award_head.lua`, `tong\npc\jitan.lua` (**tế đàn bang** — nơi nộp rương và nhận thưởng nhóm; JX1 chưa có NPC này), `event\longmenbiaoju\head.lua` + `event.lua` (bản chạy trên relay).

**E. Vật phẩm: 33 mã, không mã nào dùng lại được.** Toàn bộ id vận tiêu ở JX1 **đã mang nghĩa khác** — nặng nhất là `4475` (Linux "Rương Tiêu Vật Bang", JX1 "Ngũ Hành Văn Tinh-Thổ [Cấp 6]"), `4473/4474` (thưởng cá nhân → Văn Tinh), `4200/4202/4203/4204` (Tiêu Kỳ, ủy nhiệm trạng, Hộ Tiêu Lệnh, Khoái Mã Gia Tiên → Thương Long Văn Tinh). Bốn mã Linux JX1 không có: `30229`, `30289`, `30538`, `30557`. **Phải cấp id mới và tra lại theo TÊN**, tuyệt đối không dùng lại số của Linux.

---

## 4. Điểm phải hỏi chủ trước khi làm

1. **JX1 đã có hệ vận tiêu riêng** — `script\event\event_vantieu\` (tự viết 08/2021, 401 dòng `tieudau.lua`): vận tiêu **cá nhân**, 15:00–23:00 hằng ngày, mua Hộ Tiêu Lệnh + 50 vạn, ba mức đồng/bạc/vàng, thưởng kinh nghiệm và **cộng điểm bang** (`SetTongExp`), đã nối vào bộ cấu hình web (`VT_CFG`). **Vận tiêu bang của Linux là tính năng khác hẳn** (theo bang, cuối tuần, cần chiếm thành, có cướp tiêu PvP). Chủ muốn: (a) thêm song song, (b) thay thế hệ cũ, hay (c) ghép điểm mạnh của hai bên?
2. **Xe tiêu phải viết vào engine** (6 hàm + 3 gọi ngược + logic bám đường). Đây là việc C++ thật, không né được — khác với các đợt port trước thường xong bằng Lua. Cần chủ đồng ý trước.
3. **Gắn với chiếm thành**: luật gốc đòi bang **đang chiếm thành** mới mở được. JX1 có công thành chiến, cần chủ xác nhận dùng đúng luật này hay nới ra.
4. **Cân bằng**: kinh nghiệm bám xe 4 triệu/phút và thưởng nhóm (Huyết Long Đằng cấp 12, Lệnh bài Cổ Tháp) là số của bản Linux, phải để chủ chốt lại theo tỉ lệ máy chủ mình.

---

## 5. Ước lượng khối lượng (nếu chủ duyệt)

| Phần | Việc | Mức |
|---|---|---|
| Engine C++ | 6 hàm xe tiêu + 3 gọi ngược + xe bám đường qua bản đồ | **Nặng** — phần chính |
| Script bang | port 15 tệp `config\129` (2.545 dòng), đổi id vật phẩm theo tên | Trung bình |
| Script phụ trợ | `maplist.lua`, `tong_award_head.lua`, tế đàn `jitan.lua`, `event\longmenbiaoju` | Trung bình |
| Vật phẩm | cấp mới ~10 mã thật sự cần (rương bang, 2 thưởng cá nhân, đạo cụ), thêm cả server lẫn client | Nhẹ |
| Relay | lịch tăng thưởng + `g2s_GetBiaoChePos` liên server (có thể bỏ nếu chạy một máy chủ) | Nhẹ |

Thuận lợi lớn: **NPC, task id, hàm bang hội và khung activitysys đã sẵn sàng ở JX1** — phần khó duy nhất là xe tiêu trong engine.
