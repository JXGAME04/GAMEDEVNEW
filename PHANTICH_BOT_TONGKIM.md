# PHÂN TÍCH: CHO BOT TỰ THAM GIA TỐNG KIM

> **CHỈ PHÂN TÍCH — chưa sửa một dòng nào.** Ngày 21/08/2026.
> Nguồn: script sống `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`, C++ `D:\GAMEDEVNEW\Sources\Core\Src`.
> Phương pháp: 9 mũi đọc song song + 4 vòng phản biện đối kháng + tự kiểm chứng lại 8 khẳng định nặng nhất.
> **Đã xác minh: `CoreServer.dll` đang chạy khớp md5 với bản build từ nguồn `D:` (`404558ad…`)** ⇒ mọi kết luận C++ dưới đây đúng với máy chủ thật.

---

## 0. TÓM TẮT CHO NGƯỜI BẬN

Luồng chủ game mô tả **làm được**, nhưng có **3 chặn cứng phải gỡ trước** và **4 chỗ mô tả lệch với sự thật trên máy chủ**:

| | Điều | Sự thật đo được |
|---|---|---|
| 🔴 | Map Tống Kim **không có lưới A\*** | `379` và `324` không nằm trong danh sách trắng `IsBotPathMap` (KSubWorld.cpp:1706-1725); đĩa không có `379_srv.fp`/`324_srv.fp`. **Bot không đi nổi một bước.** ✅ May: 379 dùng **chung y hệt** thư mục dữ liệu với 380 (byte trùng tuyệt đối) nên lưới chắc chắn dựng được |
| 🔴 | **Bot được miễn toàn bộ trap** | KNpc.cpp:10068-10079 `if (PB_IsBot(...)) { PB_TrapLog(...); return; }`. Mà **toàn bộ luật Tống Kim chạy bằng trap** (12 trap) — kể cả đường chính thống rời hậu doanh ra trận |
| 🔴 | **`SetTimer`/`OnTimer` của Lua chết với bot** | KPlayer.cpp:371-373 `if (m_nNetConnectIdx == -1) return;` mà bot luôn `= -1` (KPlayerBot.cpp:832). Đồng hồ 90 giây đẩy người ra chiến tuyến **không tồn tại** với bot |
| 🟠 | "random toàn bộ bot vào" | Trần cứng **56 người/phe = 112 con/trận** (`PLAYER_MS_LIMIT=55`, so bằng `>`) |
| 🟠 | "random đều chia đều 2 phe" | **Không được bốc ngẫu nhiên**: `MAX_PLAYER_CL = 1` ⇒ chỉ vào được phe đang ít hơn/bằng. Bốc random bị **từ chối im lặng ~50%** |
| 🟠 | "bot tự mở Thần Hành Phù" | Bot **không có** item phù (1271) và map 324 nằm trong **danh sách cấm** dùng phù. Nhưng **kết quả cuối vẫn đúng ý** bằng cơ chế có sẵn (xem P7) |
| 🟠 | Giới hạn IP | **Đang tắt sẵn** (khối chặn bị chú thích), nhưng `GetIP()` trả **rỗng cho mọi bot** ⇒ nếu ai bật lại thì cả đàn bị coi là **một máy** |

---

## 1. SỰ THẬT NỀN — Tống Kim trên máy chủ này

### 1.1 Bản đang chạy là "Tống Kim trung cấp"

| Hằng | Giá trị | Bằng chứng |
|---|---|---|
| Map báo danh `MAP_BD_TC` | **324** | lib_tktc.lua:53 |
| Chiến trường `MAP_TK_TC` | **379** | lib_tktc.lua:54 |
| Mission id `MS_TONGKIM` | **1** | lib/lib_task.lua:282 |

Cả hai map đều được nạp lúc khởi động (`Maps\WorldSet_GameServer.ini`: `World306=324`, `World358=379`).

> ⚠️ Các tệp mà tài liệu/ghi nhớ CŨ hay nhắc — `baodanh-tongkim.lua`, `reset_stateTK`, `KBotAutoAI_TK.cpp`, `KPlayerAI.cpp`, map **380/325** dùng cho TK — **KHÔNG tồn tại trong cây này**. Đã grep xác nhận. Đừng tra theo trí nhớ cũ.

### 1.2 Lịch trận — hằng ngày, 4 khung

`lib_tktc.lua:70-82` → **13:13 · 17:50 · 20:50 · 22:50**, mọi ngày, không lọc thứ. Theo **giờ hệ điều hành máy chủ**.
Điều phối: `timerserver.lua:672-739` hàm `sukien_tongkim`, được `RunTime` gọi **1 lần/phút tại giây 0** (CoreServerShell.cpp:1155-1165).

### 1.3 Ba mốc của một trận

| Timer | Hằng | Giá trị **THỰC** | Chú thích trong file ghi | Script |
|---|---|---|---|---|
| 1 | `TIME_BD_TK` | **1 phút** | "10 phút báo danh" ❌ | timertask/task01.lua |
| 2 | `TIME_NS_TK` | **1 phút** | "30 phút" ❌ | task02.lua (ra Nguyên Soái) |
| 3 | `TIME_KT_TK` | **45 phút** | "70 phút" ❌ | task03.lua (kết thúc) |

🔴 **CỬA SỔ BÁO DANH CHỈ 60 GIÂY.** Toàn bộ khâu gọi bot → về map 324 → báo danh phải xong trong 1 phút. Chú thích trong file ghi 10 phút là **sai lệch với giá trị thật** — cần chủ game xác nhận đây là cố ý hay sót lại từ lúc test.

### 1.4 Thể thức do GIỜ quyết định

`lib_tktc.lua:452-459`: giờ **22** → `PT_CuuSat`, còn lại → `PT_BaoVeNguyenSoai`.
`task01.lua:81-89`: Cửu Sát **không sinh một con quái nào**.
⇒ **Trận 22:50 hiện tại vô nghĩa với bot** (bot không đánh người chơi ⇒ 0 mục tiêu ⇒ đứng im ⇒ bị đá sau 5 phút).

### 1.5 Hai NPC báo danh trên map 324

| NPC | res | Ô | NpcValue | Script |
|---|---|---|---|---|
| Tống Binh Báo Danh | 62 | (1550, 3179) | **1** (đặt sẵn) | `startgame\tinhnang\tongkim\song_signup.lua` |
| Kim Binh Báo Danh | 61 | (1555, 3082) | **0** → chỉ thành 2 sau khi `main()` chạy | `…\jin_signup.lua` |

`startgame.lua:80`, `:89`.
Luồng người chơi: `main(sel)` → `SetNpcValue` → `main_mobinh` (mobinhtk.lua:17) → `SetTaskTemp(TMP_INDEX_NPC=7, nNpcIndex)` → `Say` → bấm → **`go_tong()` / `go_kim()`** → `common_tong()` / `common_kim()`.

🔴 **Bẫy NpcValue:** NPC Kim đặt với value **0**. `go_kim` đọc `GetNpcValue(...)` làm số phe ⇒ nếu bot gọi trước khi có ai bấm `main()`, phe = 0 ⇒ `AddMSPlayer(MS_TONGKIM, 0)` sai nhóm và `TAB_PHE_TONGKIM[0]` = nil.

### 1.6 Tám cửa chặn báo danh (đúng thứ tự code)

| # | Chặn | Vị trí |
|---|---|---|
| 1 | Trận chưa mở (`IsMission==0`) | mobinhtk.lua:43-47 |
| 2 | Tên `"LongYaz"` bị chặn cứng | :49-52 |
| 3 | **Chưa vào môn phái** (`GetFaction()==""`) | :62-65 |
| 4 | **Camp == 4** (xuất sư) | :100-103, :217-220 |
| 5 | **Phe đã đủ 56 người** | :125-128, :237-240 |
| 6 | **Chênh lệch quân số ≥ 1** | :130-136, :242-248 |
| 7 | **Cấp < 80** (`LEVEL_ENOUGH_TK`) | :141-144, :253-256 |
| 8 | Đã báo danh phe khác trong cùng trận | :145-159 |

Phí `NEEDMONEY=20000` **không bị thu** (mọi `Pay`/`GetCash` đều đã chú thích).
🔴 **Mọi nhánh từ chối đều `return` trần** ⇒ C++ **không thể biết** báo danh thành hay bại từ giá trị trả về. Phải tự kiểm bằng `Npc[].m_SubWorldIndex` có đổi sang 379 hay không.

### 1.7 Giới hạn IP (yêu cầu riêng của chủ game)

| Điều | Sự thật |
|---|---|
| Khối chặn IP trong `go_tong`/`go_kim` | **ĐÃ CHÚ THÍCH** — mobinhtk.lua:114-119, :226-231 |
| `CheckLimited_Join()` | **ĐÃ CHÚ THÍCH** — kimtu.lua:136 |
| Kiểm còn sống ở `main_mobinh:68` | Chỉ so **TÊN**, mà bảng chỉ có dòng giữ chỗ `{"KEY_COMPUTER","NAME_PlAYER"}` ⇒ **không chặn ai** |
| `GetIP()` với bot | ScriptFuns.cpp:12323 → `GetClientInfo` trả NULL (bot không có kết nối) ⇒ trả **chuỗi rỗng** + **`printf` một dòng mỗi lần gọi** |
| `SAVEDULIEU` | mobinhtk.lua:166 và :185 **VẪN CHẠY**: ghi đè **cả tệp** `LIMITED_1_ACCOUNT.lua` mỗi lần báo danh thành công (header/tongkim.lua:34-44) |

**Kết luận:** bot **không bị chặn IP**. Nhưng dựa vào việc mấy dòng đó đang bị chú thích là **rủi ro** — ai bỏ chú thích là cả đàn bot (cùng "IP" rỗng) chỉ 1 con vào được. Hướng an toàn: **cho bot đi đường riêng, không chạm nhánh `SAVEDULIEU`/`GetIP`**.

🔴 Ngoài ra `SAVEDULIEU` được gọi **SAU `AddMSPlayer` nhưng TRƯỚC `common_tong`**: nếu `openfile` trả nil (tệp bị khoá/AV quét) → lỗi Lua → hàm dừng ⇒ **bot đã chiếm khe mission, được đếm vào cân bằng quân số, nhưng KHÔNG được gán camp và KHÔNG được vào map 379** = "bot ma" làm lệch cân bằng cả trận.

### 1.8 Sau khi báo danh thành công

mobinhtk.lua:424-445 (Tống) / :353-376 (Kim):

```
SetPunish(1)                       -- chết không mất gì
SetPKMode(1, 1)                    -- ép bật cờ PK VÀ KHOÁ
SetRevPos(MAP_BD_TC, 1)
SetTempRevPos(MAP_TK_TC, xPS, yPS)
SetLogoutRV(1)
SetCurCamp(1) / SetCurCamp(2)
LeaveTeam() ; SetCreateTeam(0)
NewWorld(MAP_TK_TC, xHauDoanh, yHauDoanh)
SetTimer(TIME_IN_TRAI*18, 2)       -- 90 giây  <-- CHẾT VỚI BOT
SetNpcTimeIdle(5*60)               -- 5 phút đứng im bị đá  <-- VẪN CHẠY với bot
```

Hậu doanh (`lib_tktc.lua:580-584`): Tống ô (1229,3561) · Kim ô (1689,3074).

### 1.9 🔴 VỊ TRÍ 2 PHE ĐẢO NGẪU NHIÊN MỖI TRẬN

`timerserver.lua:705-707` → `RandPosTK()` (50/50) → `SetMission(M_VITRI_TRENDUOI=47, nViTri)`.
`lib_tktc.lua:486-508`: khi `nViTri==2`, **toàn bộ bảng toạ độ của Tống và Kim đổi chỗ cho nhau**.

⇒ **Bot tuyệt đối không được ghi cứng toạ độ. Phải đọc `GetMissionV(47)` mỗi trận.**

### 1.10 Toạ độ NPC 2 phe (đơn vị Ô)

| Bảng | Số điểm | npc id | Dòng (lib_tktc.lua) |
|---|---|---|---|
| `TONGBINH_TOADO` | **78** | 631 | 163-242 |
| `KIMBINH_TOADO` | **139** | 637 | 244-384 |
| `TONGHIEUUY_TOADO` | 7 | 632 | 386-394 |
| `TONGTLINH_TOADO` | 3 | 633 | 396-400 |
| `KIMHIEUUY_TOADO` | 7 | 638 | 402-410 |
| `KIMTLINH_TOADO` | 3 | 639 | 412-416 |
| `KIMPTUONG_TOADO` | 2 | 640 | 418-421 |
| `KIMDTUONG_TOADO` | 1 = (1570,3198) | 641 | 423-425 |
| `TONGPTUONG_TOADO` | 2 | 634 | 427-430 |
| `TONGDTUONG_TOADO` | 1 = (1331,3444) | 635 | 432-434 |

**Nguyên Soái không nằm trong bảng** — ghi cứng trong `task02.lua:83-87` (Tống, id 636) và `:99-103` (Kim, id 642).

✅ **Quái 2 phe đã có camp đúng sẵn** (`AddNpcEx3(..., nCurCamp, ...)`: Tống=1 lib_tktc.lua:513, Kim=2 :544) ⇒ bot mang camp 1 **tự động coi quái Kim là địch**, KHÔNG cần sửa bộ lọc PK nào.

**8 điểm xuất quân** (nơi hệ thống ném người ra trận) — `task02.lua:209-229`:

```
RANDOM_POS_TONG = (1308,3461)(1311,3442)(1324,3428)(1337,3420)
                  (1352,3419)(1350,3439)(1342,3463)(1321,3468)
RANDOM_POS_KIM  = (1585,3181)(1570,3173)(1551,3175)(1544,3204)
                  (1550,3223)(1569,3227)(1586,3216)(1592,3198)
```

**Đơn vị:** `MPS = Ô × 32`. `NewWorld`/`SetPos`/`GetWorldPos` dùng **Ô**; `AddNpc`/`SetTempRevPos`/`SetRevPos` dùng **MPS**.

### 1.11 Shop máu trong trận

**Quân Y** — npc res **203**, `tong_kim_tcap\quany.lua`, sinh mỗi trận. Toạ độ `TKPOS_NPC_DIALOG` (lib_tktc.lua:574-578, đơn vị MPS): bộ 1 (1249,3557) · bộ 2 (1700,3066) — **cũng đảo theo `nViTri`**.

| Đường | Cơ chế | Bot dùng được? |
|---|---|---|
| `muamaunhanh()` quany.lua:28-40 | Đếm ô trống → `AddItem` trực tiếp, 1 lượng/viên | ✅ **ĐƯỢC** (không qua giao diện) |
| `muaquanluong()` quany.lua:20-26 | `Sale(53,0)` → gửi gói mở hộp shop, chờ `c2s_playerbuyitem` | ❌ **CHẾT** — bot không có client |

🟠 Nhưng `muamaunhanh` **mua lấp đầy 100% túi** ⇒ xem rủi ro #16.
NPC khác trong 379: Rương (625), Trinh Sát (55 Tống/49 Kim — dịch chuyển ra 3 điểm tiền tuyến, 500 tiền), Cờ Soái (630). **Xa Phu KHÔNG có trong 379** (2 dòng add đã chú thích).

### 1.12 Kết thúc trận — hệ thống TỰ đẩy người ra ✅

Chủ game nói đúng. `task03.lua`:

- `:144` → `PlayerEndTongKim()` → `:179` `NewWorld(324, 1543,3177)` (Tống) / `(324, 1576,3087)` (Kim)
- `:168-217` dọn sạch: `SetPKMode(0,0)`, `SetFightState(0)`, `SetPunish(0)`, `SetCreateTeam(1)`, `ReSetMask()`, `RestoreRunSpeed()`, **`SetCurCamp(GetCamp())`**, `SetDeathScript("")`, `RemoveRankBattle()`, `SetLogoutRV(0)`, `SetNpcTimeIdle(0)`
- `:215` `SetTempRevPos(53, …)` — Ba Lăng Huyện
- `:158-163` xoá 20 biến mission, `CloseMission`, `DelAllNpc`
- `:165` `-- KickOutSelf()` **đã chú thích** ⇒ không ai bị đăng xuất

🔴 **NHƯNG cả vòng dọn dẹp này kẹp bởi `GetNextPlayer(MS_TONGKIM, idx, 0)` — chỉ lấy người có param 0 == 1** (task03.lua:65). Bot **không tự có** cờ này ⇒ xem rủi ro #6.

---

## 2. SƠ ĐỒ LUỒNG CHO BOT

```
================================================================================
 P0. CANH GIỜ                                        [C++ trong PB_Breathe]
================================================================================
 Vào : mỗi nhịp (18 lần/giây)
 Làm : hỏi "trận đang mở?" -> nếu mở và chưa gửi quân => bật P1
 Ra  : cờ toàn cục g_nTkDangMo = 1
 CẦN : chưa có cách C++ hỏi trạng thái mission. Rẻ nhất: viết 1 hàm Lua
       PB_TkTrangThai() trả (dangMo, nViTri, soTong, soKim), C++ gọi 1 lần/giây
 !!  : chỉ có 60 GIÂY báo danh - mọi thứ dưới đây phải xong trong 1 phút

================================================================================
 P1. CHỌN BOT + CHIA PHE                             [C++]
================================================================================
 Vào : g_nTkDangMo == 1
 Làm : duyệt s_bots[], LOẠI BỎ:
         - đang bán sạp        (b.nBanSap)         <- yêu cầu chủ game
         - đang chết/hồi sinh  (m_Doing)
         - cấp < 80                                <- lib_tktc.lua:19
         - camp == 4 / chưa vào môn phái           <- mobinhtk.lua:62,100
       Với MỖI ứng viên: ĐỌC LẠI số quân 2 phe -> chọn phe ĐANG ÍT HƠN
 Ra  : <= 56 con/phe = 112 con
 CẦN : - accessor PB_DangBanSap(nPlayerIdx)  (s_bots là static trong .cpp)
       - đường đọc GetMSPlayerCount từ C++
 !!  : MAX_PLAYER_CL = 1 => KHÔNG ĐƯỢC bốc phe ngẫu nhiên (bị từ chối im lặng)

================================================================================
 P2. VỀ MAP BÁO DANH 324                             [C++]
================================================================================
 Vào : bot được chọn ở P1
 Làm : bật cờ b.nTongKim = 1 (khoá AI cũ)
       -> ChangeWorld(324, ô trống quanh NPC báo danh phe mình), so le 2-4 con/khung
 Ra  : m_SubWorldIndex == subworld(324)
 !!  : ChangeWorld KHÔNG kiểm vật cản => BẮT BUỘC lọc ô qua pb_ODat
       KHÔNG đi bộ A* được lúc này: map 324 chưa có lưới (rủi ro #1)

================================================================================
 P3. BÁO DANH - gọi thẳng hàm Lua, bỏ qua giao diện  [C++ -> Lua]
================================================================================
 Làm : TaskSetClearVal(7, nNpcIdxBaoDanh)      <- thay cho main() đặt TMP_INDEX_NPC
       ExecuteScript("\\script\\startgame\\tinhnang\\tongkim\\song_signup.lua",
                     "go_tong", 0, /*bGlobal=*/false)     (Kim: jin_signup + go_kim)
 Ra  : KIỂM m_SubWorldIndex đã sang 379 chưa  (go_tong KHÔNG trả về gì)
 CẦN : hàm tìm NPC báo danh - bộ nhớ NPC của bot hiện chỉ biết 2 loại (Dã Tẩu, Xa Phu)
 !!  : - bGlobal PHẢI = false
       - NPC Kim có NpcValue = 0 lúc đặt (mục 1.5)
       - SAVEDULIEU ghi đè cả tệp mỗi lần (mục 1.7 + rủi ro #10)

================================================================================
 P4. MUA MÁU TẠI QUÂN Y                              [KHUYẾN NGHỊ BỎ]
================================================================================
 Làm : ExecuteScript("...\\quany.lua", "muamaunhanh", 0, false)
 !!  : hàm này mua LẤP ĐẦY TÚI, mà pb_DonTui GIỮ LẠI thuốc => túi đầy vĩnh viễn
       => khâu trả nhiệm vụ Dã Tẩu đòi >= 5 ô trống => BOT KẸT DÃ TẨU SAU TRẬN
 -> Bot đã có Túi Dược Phẩm vĩnh viễn, tự rót bình mỗi 12 giây. ĐỀ NGHỊ BỎ PHA NÀY.

================================================================================
 P5. RA TRẬN - C++ PHẢI TỰ LÀM       [cả 2 đường của người chơi ĐỀU CHẾT]
================================================================================
 Vào : bot ở hậu doanh 379, sau ~90 giây
 Làm : gọi lại chính mã gốc: ExecuteScript(task02.lua, "tong_ratran"/"kim_ratran")
       -> tự động có: SetPos 1/8 điểm xuất quân + SetFightState(1)
                      + bất tử 3 giây + SetDeathScript + cờ param 1 = 1
 VÌ SAO PHẢI TỰ LÀM:
       - trap ra trại KHÔNG kích cho bot        (KNpc.cpp:10068-10079)
       - SetTimer 90 giây KHÔNG chạy cho bot    (KPlayer.cpp:371-373)

================================================================================
 P6. DI CHUYỂN + ĐÁNH                                [C++]
================================================================================
 Làm : nViTri = GetMissionV(47)             <- BẮT BUỘC, vị trí 2 phe đảo mỗi trận
       bảng đích = bảng toạ độ PHE ĐỊCH (đã hoán theo nViTri)
       bốc ngẫu nhiên 1 điểm -> PB_WalkTo(map 379, x*32, y*32)
       tới nơi / gặp quái -> pb_Fight (bot tự bật FightMode)
       PB_WalkTo trả -1 -> BỐC ĐIỂM KHÁC, không bỏ cuộc
 CẦN : - LƯỚI A* CHO 379 (rủi ro #1 - chặn cứng)
       - vòng chọn đích riêng cho TK
 !!  : - tầm nhìn bot 700 MPS (~22 ô) mà 2 sân cách ~498 ô => bot KHÔNG tự tìm ra
         địch, phải được LỆNH đi tới toạ độ (đúng như chủ game mô tả)
       - "giãn đàn" sẽ chủ động sơ tán bot khỏi đám đông (rủi ro #13)
       - bot KHÔNG đánh người chơi (rủi ro #9)
       - trận 22:50 Cửu Sát KHÔNG CÓ QUÁI => bot đứng im => bị đá sau 5 phút

================================================================================
 P7. HẾT TRẬN - DỌN DẸP                     [gần như KHÔNG cần viết gì]
================================================================================
 Vào : mission đóng, hoặc bot bị NewWorld về 324
 Làm : b.nTongKim = 0 ; ÉP b.nAi = PB_AI_FIGHT ; Reset cả 6 lộ trình
 Ra  : bot tự chạy tiếp - CƠ CHẾ CÓ SẴN:
         map 324 không phải bãi, không phải thành
         -> pb_RaBai vào pha 1 -> pb_DtVeThanh -> về thành nhà
         -> đi bộ tới Xa Phu -> station.lua bot_bai_go -> LÊN BÃI ĐÚNG CẤP  [OK]
       Bot Dã Tẩu: nDaTauChon không bị xoá, nDtPha tự đồng bộ theo task 1028
         -> TỰ CHẠY DÃ TẨU LẠI  [OK]
 !!  : - chủ game mô tả "bot tự mở Thần Hành Phù" - bot KHÔNG có item 1271 và
         map 324 nằm trong danh sách CẤM dùng phù. Kết quả cuối vẫn ĐÚNG Ý,
         nhưng bằng đường Xa Phu có sẵn.
       - PHẢI ép b.nAi = PB_AI_FIGHT, không thì bot ở trạng thái IDLE đứng im
         vĩnh viễn trên map 324.
       - PHẢI tự gọi lại chuỗi dọn camp của PlayerEndTongKim (rủi ro #6)
================================================================================
```

---

## 3. CHỖ SẼ VỠ (xếp theo mức nghiêm trọng)

### 🔴 #1 — Map 379 và 324 KHÔNG có lưới A\* (CHẶN CỨNG)

Bot không đi được một bước trong Tống Kim. P6 và mọi ý định "chạy tới NPC" đều chết.

- `KSubWorld.cpp:1706-1725` mảng trắng `s_map[]` có **380** nhưng **không có 379, không có 324**
- `KSubWorld.cpp:1856` chỉ nạp lưới khi `IsBotPathMap()` đúng; `:3435` `if (!m_bHavePath) return -2`
- Đĩa: `bin\server\Maps\` có `380_srv.fp` (8,6 MB), **không có** `379_srv.fp`/`324_srv.fp`

✅ **Tin tốt (tự kiểm chứng thêm):** `MapList.ini` cho thấy **379 và 380 trỏ CÙNG MỘT thư mục dữ liệu, byte trùng tuyệt đối** ⇒ lưới 379 chắc chắn dựng được và hợp lệ (vì 380 đang chạy tốt). Rủi ro "lưới rỗng, A\* vẽ đường xuyên tường" gần như bị loại.

**Hướng:** thêm `379`, `324` vào `s_map[]`, khởi động lại, đọc log `[PathSrv] map 379: dung luoi … o bi chan`.

### 🔴 #2 — Bot được miễn TOÀN BỘ trap

`KNpc.cpp:10068-10079`. Toàn bộ luật TK chạy bằng 12 trap (`lib_tktc.lua:647-663`): ra trại / vào trại / chặn công / cắm cờ. Đường **chính thống** rời hậu doanh ra trận cũng là trap — nơi gán `SetDeathScript` và cờ `param 1 = 1`.

**Hướng:** C++ tự làm khâu ra trận (P5). **Không gỡ luật miễn trap** — nó là chủ ý và gỡ ra ảnh hưởng toàn hệ bot.

### 🔴 #3 — `SetTimer`/`OnTimer` Lua không bao giờ chạy cho bot

`KPlayer::Active()` return sớm khi `m_nNetConnectIdx == -1` (KPlayer.cpp:371-373); bot luôn `-1` (KPlayerBot.cpp:832). Đồng hồ 90 giây (`mobinhtk.lua:371`, `:441`) không tồn tại ⇒ **bot báo danh xong nằm im trong hậu doanh đến khi bị đá sau 5 phút**.

⚠️ Nhưng **idle-kick VẪN CHẠY** với bot vì nó nằm trong engine `KNpc::Activate` — con dao hai lưỡi.

### 🔴 #4 — Trần 112 bot và KHÔNG được chia phe ngẫu nhiên

`PLAYER_MS_LIMIT=55` kiểm bằng `>` ⇒ 56/phe. `MAX_PLAYER_CL=1` (chú thích ghi 5 — **sai**) ⇒ chỉ vào được phe ít hơn/bằng.

**Hướng:** đọc lại số quân 2 phe **ngay trước mỗi lần gọi**, xen kẽ tuyệt đối. Yêu cầu "random toàn bộ bot" là **bất khả thi** với trần này.

### 🔴 #5 — AI của chính bot TỰ KÉO BOT RA KHỎI TRẬN

| Nhánh | Sau bao lâu | Dòng |
|---|---|---|
| Dã Tẩu `DTB_TOI_NPC` lệch map | **3 giây** | KPlayerBot.cpp:2990-3005 |
| Bán sạp lệch map | **3 giây** | :3635-3694 |
| `pb_RaBai` (bot thường) | 0-60 giây | :4598-4612 |
| `DTB_FARM_BAI` chu kỳ | 5 phút | :3331-3336 |

**Hướng:** cờ `b.nTongKim` kiểm ở **đầu `pb_DriveBot`** — vị trí đúng duy nhất là **SAU khối tự hồi sinh** (:6847-6879, để bot chết vẫn sống lại) và **TRƯỚC nhánh bán sạp** (:6885).

### 🔴 #6 — CAMP DÍNH VĨNH VIỄN nếu bot rời 379 sớm

Bot mang `m_CurrentCamp = 1/2` ra thế giới thường + `SetPKMode(1,1)` khoá cờ PK ⇒ **người chơi thật khác camp thành ĐỊCH của bot đó ở giữa thành.**

Chuỗi bằng chứng:

1. Đổi map **không** gỡ khỏi mission — hai dòng `RemovePlayer` trong `KNpc::ChangeWorld` **đều bị chú thích** (KNpc.cpp:9748, :9771)
2. `OnLeave` (đường phục hồi camp duy nhất) chỉ chạy lúc **ĐĂNG XUẤT** (KPlayerSet.cpp:375) — bot không bao giờ đăng xuất
3. `CloseMission` không duyệt người chơi (ScriptFuns.cpp:10581-10608)
4. Cả 2 vòng phục hồi camp đều kẹp bởi `param 0 == 1` (task03.lua:65, task01.lua:50)
5. Luật địch giữa 2 người chơi **không có** điều kiện cùng map/cùng mission (KNpcSet.cpp:1778-1785)

**Hướng:** khi gỡ cờ TK, C++ **phải tự gọi lại** chuỗi dọn của `PlayerEndTongKim` (task03.lua:168-217). Không được phó mặc.
*Giảm nhẹ:* camp dính không sống sót qua khởi động lại server.

### 🟠 #7 — TỔ ĐỘI GHI ĐÈ CAMP → phá nát chia phe

`KPlayer::AcceptTeam` (KPlayer.cpp:1626) ghi `m_CurrentCamp` thành viên = camp đội trưởng. Đo thật: **~795/1000 bot đang trong tổ đội**. Hai bot khác phe TK mà cùng nhóm → `GetRelation` trả **ALLY** (tổ đội thắng luật camp).

**Hướng:** `pb_RoiNhom` trước khi báo danh + thêm `!b.nTongKim` vào điều kiện ghép nhóm (:7832).

### 🟠 #8 — Trận 22:50 (Cửu Sát) vô nghĩa với bot

Không quái + bot không đánh người ⇒ 0 mục tiêu ⇒ đứng im ⇒ bị đá.

### 🟠 #9 — Mở bộ lọc `kind_player` là thay đổi NGUY HIỂM NHẤT

Quan hệ địch giữa 2 người chơi chỉ cần **khác camp + cả hai bật FightMode + cả hai có cờ PK (mặc định = 1)** — **không có điều kiện map/mission**. Bỏ 2 dòng lọc (`:4258`, `:4977`) ⇒ bot đánh người chơi thật khác camp **ở giữa thành, ở bãi luyện, ở mọi nơi**.

**Hướng:** nếu mở, **bắt buộc kẹp** `b.nTongKim && map == 379`.

### 🟠 #10 — `SAVEDULIEU` ghi đè cả tệp + nguy cơ "bot ma"

Xem mục 1.7. Hai vấn đề: (A) 112 lượt ghi đĩa đồng bộ trên luồng game trong 60 giây; (B) lỗi ghi tệp ⇒ bot chiếm khe mission nhưng không vào được trận, làm lệch cân bằng.
*Giảm nhẹ:* `DELDULIEU()` chạy mỗi đầu trận nên tệp không phình vô hạn.

### 🟠 #11 — Bảng xếp hạng mission: O(N) + 11N gói tin mỗi lần cộng điểm

`KMission::SetPlayerParam` (KMission.cpp:241-287): mỗi lần điểm đổi → quét toàn danh sách dựng top 10 → quét lần nữa gửi **11 gói tin cho từng người**. Kích hoạt rất dày: giết quái, giết người, nhặt bảo vật, **và ngay lúc báo danh**.

112 bot ⇒ ~1.232 lời gọi/lần cộng điểm: chấp nhận được. **Nới trần lên 1000 thì đây là điểm nổ số 1.**

⚠️ Bot **không bị lọc** khỏi hàm này ⇒ bot còn **chiếm chỗ top 10, ăn Xu + 100 triệu exp + danh hiệu** của người thật (task03.lua:85-98).

### 🟡 #12 — `pb_FindTarget` quét 9 vùng MỖI KHUNG

TK chỉ có ~243 quái; sau vài phút quái chết sạch ⇒ gần 100% bot quét vô ích mỗi khung.
**Hướng:** giãn nhịp (1 giây/lần) khi `b.nTongKim`.

### 🟡 #13 — "Giãn đàn" chủ động sơ tán bot khỏi chiến trường

Cứ 8 giây, >20 bot trong 20 ô thì bốc điểm xa mà đi (:7428-7495). Trong TK đám đông luôn >20 ⇒ bot liên tục bỏ chiến tuyến.

### 🟡 #14 — `PB_SetBanSap`/`PB_SetDaTau` re-roll giữa trận

`PB_SetBanSap` **đóng hết sạp rồi bốc lại từ đầu** ⇒ bấm lại lệnh giữa trận có thể gán cờ sạp cho bot đang trong TK ⇒ 3 giây sau nó `ChangeWorld` ra khỏi trận. Vòng bù Dã Tẩu 60 giây/lần cũng vậy.
**Hướng:** thêm `if (s_bots[i].nTongKim) continue;` vào cả 4 vòng.

### 🟡 #15 — Chưa có cách đọc "bot này đang bán sạp" từ ngoài tệp

`s_bots` là `static`, `struct PB_Bot` khai trong `.cpp`. Hai đường vòng **đều SAI**:

- `m_BaiTan`: engine tự hạ về 0 khi sạp bán hết hàng ⇒ bỏ sót bot đang bán thật
- `do_sit`: có cửa sổ tới 5 giây bot đang bán mà `m_Doing != do_sit`

**Hướng:** thêm accessor `PB_DangBanSap(nPlayerIdx)` theo khuôn `PB_IsBot` — **chốt CẢ `(nPlayerIdx, dwID)`**, thiếu `dwID` thì khe bot đã tháo cấp lại cho người thật sẽ bị nhận nhầm.

### 🟡 #16 — Túi đầy do `muamaunhanh` → hỏng Dã Tẩu sau trận

Chuỗi: túi đầy → `pb_NhatDo` vô dụng → thưởng cuối trận không vào được → **khâu trả nhiệm vụ Dã Tẩu đòi ≥ 5 ô trống (:3398) ⇒ bot kẹt vòng Dã Tẩu vĩnh viễn**.

### 🟡 #17 — Thêm trường mới vào `PB_Bot` phải nhớ khởi tạo

`struct PB_Bot` hiện có **đúng 106 trường** (:158-294) và **cả 106 đều được gán lại** trong khối tái dùng khe (:939-1020). Mọi trường TK mới **phải** thêm vào đó, nếu không khe tái dùng kế thừa "đang trong Tống Kim" của bot trước.

### 🟢 #18 — Những rủi ro ĐÃ AN TOÀN (không cần làm gì)

| Rủi ro | Vì sao an toàn |
|---|---|
| Bot hút đồ rơi của người thật | Bot chỉ nhặt đồ của chính nó (:5522-5531) |
| Bot chết mất đồ/exp | `SetPunish(1)` lúc báo danh ⇒ `DeathPunish` return ngay |
| Bot mài mòn trang bị | Bot được miễn `Abrade` (KItemList.cpp:4643) |
| Cần sửa bộ lọc PK để đánh quái TK | **Không cần** — quái đã có camp đúng sẵn |
| NPC-là-tường làm bot kẹt nhau | Đã tắt toàn cục `g_nPbNpcChan = 0` (21/08) |
| Bot bị xích neo bãi trong 379 | `pb_NeoBai` trả 0 khi map khác map bãi |
| `Say`/`Talk` treo luồng khi gọi cho bot | Chỉ đặt cờ rồi gửi gói, không chờ |
| `Pay()` không chạy với bot | Bot có `m_nLicReg = true` ⇒ chạy bình thường |
| `GetIP()` gây lỗi Lua cho bot | Trả chuỗi rỗng an toàn (nhưng in 1 dòng printf/lần) |
| Tràn bộ nhớ khi thêm lưới | `GameServer.exe` là x64 |

---

## 4. CẦN ĐO TRƯỚC KHI GÕ CODE

| # | Chưa chắc | Cách làm cho chắc |
|---|---|---|
| 1 | ~~Lưới 379 có hợp lệ không~~ | ✅ **ĐÃ GIẢI QUYẾT**: 379 và 380 dùng chung y hệt thư mục dữ liệu (byte trùng) |
| 2 | ~~DLL chạy có khớp nguồn không~~ | ✅ **ĐÃ GIẢI QUYẾT**: md5 khớp tuyệt đối `404558ad…` |
| 3 | `sj_checklastbattle` khoá phe theo NGÀY hay theo TRẬN | Đọc `typedef Lua_Number` (nếu float 32-bit thì số unix 10 chữ số mất độ chính xác ⇒ hoá ra khoá theo ngày); hoặc chạy thử 2 trận liên tiếp |
| 4 | Chi phí thực A\* trên 379 — `FindPath_Block` **không có trần số nút** | Thả 100-200 bot vào 379, đọc `[BotPerf]` trước/sau |
| 5 | `AddItemSL` xử lý sao khi túi bot đầy (thưởng cuối trận) | Đọc `LuaAddItemSL`; nếu rơi ra đất thì map 324 bị rải đồ mỗi trận |
| 6 | Có bao nhiêu bot đạt cấp ≥ 80 (Dã Tẩu đã hạ ngưỡng xuống 70) | Đếm `m_Level` của `s_bots` |
| 7 | Map 324 còn NPC gì sau khi trận kết thúc (`DelAllNpc`) | Vào game quan sát ngay sau một trận |
| 8 | Múi giờ hệ điều hành máy chạy GameServer | `Get-Date` trên máy đó |
| 9 | Thời gian boot tăng thêm khi nạp lưới 379+324 (hiện đã 60-90 giây) | Bấm giờ giữa hai dòng `[PathSrv]` lúc boot |
| 10 | Chưa có số `[SvPerf]` (nhiệt kế khung server) trong log phiên này | Bật lại trước khi kết luận "còn bao nhiêu dư địa" |

---

## 5. CÁC QUYẾT ĐỊNH CẦN CHỦ GAME CHỐT

**Q1. Bao nhiêu bot mỗi trận?** Trần script = 112 con.
→ **Khuyến nghị: giữ 112, xoay vòng bot giữa các trận.** Nới trần sẽ đánh thức rủi ro #11 và **khoá cửa người chơi thật khỏi trận**.

**Q2. Bot có đánh người chơi thật trong TK không?**
→ **Khuyến nghị: KHÔNG mở; nếu mở thì bắt buộc kẹp `nTongKim && map==379`.** Mở trần = bot đánh người thật khắp thế giới.

**Q3. Trận 22:50 (Cửu Sát, không quái) có cho bot vào?**
→ **Khuyến nghị: KHÔNG**, trừ khi Q2 = có.

**Q4. Có giữ pha "mua máu tại Quân Y"?**
→ **Khuyến nghị: BỎ.** Bot đã có Túi Dược Phẩm tự rót. Giữ thì phải sửa `muamaunhanh` nhận tham số số lượng.

**Q5. Thêm map 379 + 324 vào danh sách trắng lưới A\*?**
→ **Khuyến nghị: CÓ — điều kiện tiên quyết.** Chi phí ~8,7 MB RAM + một lần dựng lưới lúc boot.

**Q6. Cách bot "ra trận" sau báo danh?**
→ **Khuyến nghị: gọi `ExecuteScript` chính hàm `tong_ratran`/`kim_ratran` của `task02.lua`** — dùng lại mã gốc, tự có `SetDeathScript` + cờ param + bất tử 3 giây, lệch ít nhất với người thật.

**Q7. Loại trừ bot bán sạp** (chủ game đã chốt: có)
→ **Khuyến nghị: thêm `PB_DangBanSap()` chốt `(nPlayerIdx, dwID)`.** Không dùng `m_BaiTan` hay `do_sit` — cả hai đều sai.

**Q8. `TIME_BD_TK = 1` (báo danh 60 giây) — cố ý hay sót từ lúc test?**
Chú thích ghi 10 phút; `TIME_KT_TK = 45` cũng lệch chú thích (ghi 70).
→ **Cần xác nhận.** 60 giây rất gấp để đưa 112 bot về 324 + báo danh. Nới lên 3-5 phút thì luồng dễ thở hơn nhiều.

**Q9. Sau trận bot về đâu?** Cơ chế **có sẵn** đưa bot về thành → Xa Phu → bãi đúng cấp (đúng ý chủ game), **nhưng không qua Thần Hành Phù** như mô tả.
→ **Khuyến nghị: dùng đường có sẵn, không viết gì thêm** — chỉ cần ép `b.nAi = PB_AI_FIGHT` khi gỡ cờ TK.

---

## PHỤ LỤC — Tệp cần đọc trước khi thi công

| Tệp | Vai trò |
|---|---|
| `…\script\tinhnang\tong_kim_tcap\lib_tktc.lua` (922 dòng) | Toàn bộ hằng số, bảng toạ độ, hàm sinh NPC/trap |
| `…\script\tinhnang\tong_kim_tcap\mobinhtk.lua` (452 dòng) | Báo danh: `go_tong`/`go_kim`/`common_*` |
| `…\script\timerserver.lua:672-739` | Mở trận, chọn thể thức, chọn vị trí trên/dưới |
| `…\script\timertask\task01/02/03.lua` | 3 pha; `task02` chứa `tong_ratran`/`kim_ratran` |
| `…\script\tinhnang\tong_kim_tcap\quany.lua` | Shop máu |
| `…\script\startgame.lua:80-98` | Đặt NPC báo danh trên map 324 |
| `D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp` | Hệ bot; điểm cắm cờ TK ở `pb_DriveBot` ~6880 |
| `…\KSubWorld.cpp:1704-1725` | Danh sách trắng lưới A\* — **phải sửa** |
| `…\KNpc.cpp:10068-10079` | Miễn trap cho bot |
| `…\KPlayer.cpp:371-382` | Lý do `SetTimer` chết với bot |
| `…\KNpcSet.cpp:128-165, 1674-1792` | Toàn bộ luật camp/PK |
