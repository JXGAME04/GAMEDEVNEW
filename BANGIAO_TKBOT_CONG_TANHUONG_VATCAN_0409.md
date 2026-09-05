# BÀN GIAO 04/09 chiều — Bot Tống Kim: cổng "trận chưa bắt đầu" + chặng tản hướng; tắt "người + bot là vật cản"

Chủ (04/09 ~17:45): *"bỏ tính năng người chơi - bot là vật cản giúp tôi"* · *"fix lại bot tống kim khi chưa bắt đầu trận thì không được ra ngoài"* · *"cho bot tống kim khi ra ngoài doanh trại thì random nhiều tọa độ các hướng khác nhau để chia ra di chuyển nhiều đường hơn"*.

Nhánh `vatcan-tk-0409` (worktree `D:\GAMEDEVNEW_wt_phuve`), build từ `origin/main` fb200e15 + một commit, đã đẩy thẳng `origin/main` theo luật mới 17:25.

## 1. Tắt "người + bot là vật cản" — `KRegion.cpp:53` `g_nPbNpcChan = 0`

- Đảo lại đúng một giá trị mà commit `c1c11500` (04/09 03:18) đã bật; giữ `pb_DeChong` (bot đứng chồng ô thì tự bước sang ô kề) như chú thích của chính commit đó.
- 🔴 Biến này biên vào **cả hai phía** (`KRegion::GetBarrier` nhánh client cũng đọc) ⇒ **phải swap `CoreServer.dll` và `CoreClient.dll` cùng lúc**, nếu không client mô phỏng người khác bị chặn còn máy chủ cho chạy xuyên → "người xung quanh giật mạnh khi đông". Lệnh GM `PB_SetNpcChan` chỉ đổi phía máy chủ.

## 2. Bot Tống Kim không được ra ngoài khi trận chưa bắt đầu — `KPlayerBot.cpp`

**Đo (bot.log, trận 16:29 04/09):** 482 bot báo danh lúc 16:29:01; **236 con "đạp trap ra trại → DA RA TRAN" trong 16:29:15–16:29:59**, trong khi trận bắt đầu 16:30:00 (client auto: `[TK-CHO] thoai chan cong: tran chua bat dau, con 8 giay` lúc 16:29:51). Mọi trận trước đều cùng mẫu (báo danh :01 → ra :15–:17).

**Gốc:** `pb_TkRaTrai` đã có cổng chờ đúng (đồng hồ mission 1 `GetTimerRestTimer(1) > 0` = còn pha báo danh, cùng nguồn với `tong/kim_chancong.lua` chặn người chơi), nhưng cổng nằm **sau** bước "đi bộ tới đúng ô trap". Bot tới ô trap → `KNpc::CheckTrap` → `PB_TrapLog` chạy kịch bản `tongratrai/kimratrai.lua` như người chơi; kịch bản đó chỉ xét đồng hồ **cá nhân** 10 giây (`tongratrai.lua:21`), không biết trận bắt đầu chưa → `SetPos` ra ngoài. Người chơi thật thì bị 3 trap chân công ném về (`GetMSRestTime(MS_TONGKIM,1) > 0` → `SetPos` điểm chân công), còn bot miễn mọi trap.

**Sửa (hai lớp, cùng điều kiện với script chân công):**
- `pb_TkRaTrai`: chuyển khối cổng lên **trước** bước đi bộ tới ô trap (cổng đóng → đứng chờ tại chỗ trong trại, `nTkChoRa` rải 1..15 s như cũ để lúc mở cổng không ùa ra cùng giây).
- `PB_TrapLog`: bot pha 3 đạp trap ra trại mà mission timer 1 còn chạy → **không** chạy kịch bản, log `[BotTK] … tran CHUA BAT DAU (con N giay) -> khong qua` (tiết chế 10 s/con), `walk.Reset()`. Khai báo trước `pb_TkMission` ở đầu tệp.

## 3. Chặng "tản hướng" khi vừa ra trại — `pb_TkTanHuong` / `pb_TkBatDauTanHuong` (`KPlayerBot.cpp`)

- Gọi tại **cả hai cửa** pha 3 → 4 (đạp trap thấy mình ngoài trại; đi bộ tới cửa rồi bước qua trap).
- Bốc **một điểm kho** `s_tkKhoDiem` (264 điểm sinh từ script, chắc chắn trong đấu trường) trong vành **20..70 ô** quanh chỗ vừa ra, theo **một trong 8 hướng** (hướng ngẫu nhiên theo chỉ số bot + mốc thời gian + `g_Random`, xoay dần nếu hướng đó không có điểm), loại điểm trong hộp 45 ô quanh hậu doanh mình (pha 4 coi ≤ 40 ô là "trong trại"), kiểm ô đất `pb_ODuoc` + liên thông `pb_SapLoang` (BFS 27×27, trần 3 BFS/lần) + rải lệch ±12 ô như `pb_TkBocTrungGian`.
- Trường mới `PB_Bot::nTkTanRa` (khởi tạo ở khe tái dùng). Pha 4: sau `pb_Fight` và `if (b.nTargetNpc > 0) return;`, đang chặng tản thì đi tới điểm tản **trước**, chưa săn / chưa bốc doanh; gặp địch trên đường vẫn đánh (`pb_Fight` chạy trước); tới nơi / quá hạn `PB_TK_TRUNG_HAN` (40 s) / không đường → kết thúc chặng, `nTkDichTick = 0` để bốc đích ngay. `bTkDangDi` tính cả chặng tản để quét mục tiêu dày (3 nhịp).
- Log `[BotTanHuong] <tên> phe <p> ra trai o(x,y) -> tan huong <0..7> toi o(x,y)` (1 dòng/giây toàn cục; 0=Đông 1=ĐN 2=Nam 3=TN 4=Tây 5=TB 6=Bắc 7=ĐB).

## 4. Nhị phân — swap CẶP

| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **5af5166720dff887f982b980a9c9e94b** | 18.397.184 | tắt GameServer → `ChayGameServer.bat` |
| `bin\client\CoreClient.dll.moi` | **790fa976dc0ca7d95d8d71bc6e16ca29** | 2.552.320 | thoát Game + WAuto → `ChoiGame.bat` |

Kiểm nhị phân: máy chủ có đủ mọi nhãn `[…]` của bản live 467afa59 (`CL_Cong`, `st_ledger`, `AUC_MsgTong`, `[RoleChk2]`, `[S13-TELE-CU]`, `[DECHONG]`, `TKDich`, `SapNpc`…) + `[BotTanHuong]` + câu chặn cổng; client cùng bộ nhãn với bản live b73372be (chỉ khác `g_nPbNpcChan`), tương thích `Game.exe` 14:41 (không đổi `S3Client`/`CoreShell` sau e64bdb52). `Game.exe` giữ nguyên. **Không swap lẻ một bên** (mục 1).

## 5. Nghiệm thu (bot.log, trận kế)

- Trước giờ bắt đầu: **0** dòng `da o ngoai trai` / `qua cua trai RA TRAN` trong phút báo danh; có vài dòng `tran CHUA BAT DAU (con N giay) -> khong qua` là bình thường (bot lỡ đứng trên ô trap).
- Từ giây bắt đầu: `qua cua trai RA TRAN` rải trong ~15 s; kèm `[BotTanHuong] … tan huong k` với **k trải nhiều giá trị** (0..7), không dồn một hướng.
- Nhìn trận: hai đàn không còn thành hàng một trên cùng hành lang ngay sau cửa trại.
- Hồi quy phải soát: bot chờ trong trại quá `PB_TK_PHA_HAN` không bị coi là "kẹt" (cổng đóng làm tươi `nTkTick`, giữ nguyên); bot tản hướng vào ngõ cụt → hết 40 s tự bỏ; người/bot đứng chồng ô sau khi tắt vật cản là **chủ ý của chủ** (chỉ còn `pb_DeChong` cho bot đứng yên).

## 6. Liên quan
`BANGIAO_PHIEN_BOT_TK_3008.md` (máy trạng thái bot TK), `BANGIAO_GIATLUI_PHUVE_FPS_0309.md` mục 10.11 (S13l), memory `jx1-bot-dechong-o-0409`.
