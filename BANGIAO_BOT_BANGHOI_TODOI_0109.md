# BOT: XIN VÀO BANG HỘI THEO NHẮN MẬT + VÀO TỔ ĐỘI NGƯỜI CHƠI (01/09/2026)

> Chủ game giao 2 tính năng:
> 1. *"khi người chơi nhắn mật (lưu ý nhắn mật) với bot rủ vào bang hội thì bot sẽ về tại NPC môn phái
>    xuất sư và xin và chọn đúng bang hội của người rủ để xin vào bang hội ở bản bang hội"*
> 2. *"bot được người chơi mời vào party — nếu chưa có party thì sẽ vào party người chơi — khi người
>    chơi đổi map thì bot tự thoát party"*
>
> Vá: `ReverseTools/goi_va_bot_bang_nhom.py` (idempotent; 3 tệp: KPlayerBot.h 1 hunk, KPlayerTeam.cpp
> 2 hunk (CRLF, có TCVN3 — đi latin-1, high-byte 150 giữ nguyên), KPlayerBot.cpp 9 hunk).
> Server `CoreServer.dll.moi` = **`fb284d90`** (01/09 13:01) **CHỜ RESTART**. Không đổi giao thức,
> không deploy client (miếng KPlayerTeam.cpp nằm trong `#ifdef _SERVER`; Client Release|Win32 đã build
> kiểm sạch).

## 1. Bang hội — luồng thật

| Bước | Ở đâu | Cách làm |
|---|---|---|
| Nhận nhắn mật | `PB_WhisperReply` (KPlayerBot.cpp) | Hàm này CHỈ nhận whisper từ GameServer (`PB_WHISPER`) ⇒ đúng nghĩa "nhắn mật", chat kênh/gần không tới. Từ khoá ASCII **`bang`** sau hạ chữ thường ("vào bang", "gia nhập bang", "bang hội"…; "băng/bằng/bảng" mang byte TCVN3 khác nên không dính). Người rủ **phải đang có bang** (`m_cTong.m_nFlag` + `GetTongNameID()` tra được trong `g_TongJX2`). |
| Trả lời | cùng chỗ | Đã có bang → "Minh co bang hoi roi"; người rủ không có bang → "Ban chua co bang hoi ma"; đang đi → "dang tren duong"; nhận việc → "Ok! Minh ve NPC mon phai xin vao bang <tên> ngay day." Bỏ hạn "làm ngơ" 8-15s cho câu này (`nPmCamToi = 0`). |
| Đi về NPC | `pb_XinVaoBang` gọi trong `pb_DriveBot` (sau khối Tống Kim, trước bán sạp) | NPC = NPC môn phái **của bot** (`s_facNpc[b.nFaction]`, chưa rõ phái thì Thiếu Lâm — cả 10 NPC ở map 53 cạnh nhau). Khác map → rời nhóm + `ChangeWorld` cạnh NPC (pb_ODat ±12 ô, giãn 3s — khuôn Dã Tẩu DTB_TOI_NPC). Cùng map → `PB_WalkTo` tới NPC, thua A* 5 lần → `SetPos` cạnh NPC (khuôn PB_AI_GOTO_FACTION). Hết hạn **6 phút** / bot đã có bang / bang biến mất → huỷ + nhắn lại. |
| Nộp đơn | tới nơi | `g_TongJX2.DoClientOpBody(bot, {m_btOp=defTONG_JX2_COP_APPLY_JOIN, m_dwTarget=id bang người rủ})` = **cùng thân `sJX2_DoApplyJoin` với cửa sổ client** (KTongJX2.cpp:575): đủ ngưỡng tự nhận (field 65) → vào thẳng (mã 7); không → vào hàng đợi đơn ở relay (mã 0, bang chủ online được báo ngay `sJX2_NotifyApply`); dưới ngưỡng từ chối (field 66) → mã 11; đã nộp → 12; lỗi/đã có bang → 5. |
| Báo kết quả | cùng chỗ | Nhắn mật lại người rủ theo mã (vào thẳng / đã nộp đơn chờ duyệt / nộp trước rồi / cấp thấp / lỗi). Log `[BotBang]`. |
| Đang Tống Kim | | Nhận việc nhưng đợi xong trận (khối TK `return` trước); đồng hồ 6 phút chỉ chạy từ khi bắt đầu đi. |

Ghi chú: thực tế người chơi thật xin vào bang từ **cửa sổ bang hội của client** (`COP_APPLY_JOIN`), không
có NPC bắt buộc; việc "về NPC môn phái" là để bot nhìn giống người ("về tại NPC môn phái" như chủ mô tả).
Bot đi qua **đúng thân xử lý** của người thật nên mọi luật (ngưỡng cấp, hàng đợi, thông báo bang chủ) y hệt.

## 2. Tổ đội — luồng thật

| Bước | Ở đâu | Cách làm |
|---|---|---|
| Người thật mời bot | `KPlayerTeam::InviteAdd(int, TEAM_INVITE_ADD_COMMAND*)` (server, KPlayerTeam.cpp) | Ngay sau khi tìm được `nTargetIdx`: `if (PB_IsBot(nTargetIdx) && PB_MoiVaoNhom(nIdx, nTargetIdx)) return;` — bot không có client để bấm "đồng ý" nên bot module tự ghi `m_nInviteList` + `GetInviteReply(...,1)` (khuôn `pb_GhepNhom`). Gói `s2c_teamaddmember` tới người mời đi như thường. |
| Điều kiện nhận | `PB_MoiVaoNhom` | Chỉ nhận lời mời của **người thật** (`m_nNetConnectIdx >= 0`); từ chối khi đang Tống Kim / bán sạp / chết; "nếu chưa có party": đang ở nhóm **có người thật khác** → từ chối; nhóm **toàn bot** → rời nhóm đó rồi sang (bot ghép nhóm nhau 100% nên nếu hiểu chữ "chưa có party" theo nghĩa đen thì tính năng gần như không bao giờ nổ — đã chọn cách này, chủ muốn khác thì đổi 1 điều kiện). Nhóm đầy → để đường cũ báo. |
| Canh đổi map | `pb_DriveBot`, ngay sau `pb_XuLyPmCho` (chạy trước mọi `return`) | Người mời **đổi map** (`m_SubWorldIndex` khác) → `pb_RoiNhom` + nhắn "Ban doi map roi, minh roi nhom nhe."; người mời thoát game / không còn chung nhóm (kick, giải tán) → thôi canh. |
| Quản lý nhóm bot | `pb_QuanLyNhom` | Nhóm có người thật → **không đụng** (trước đây thấy đội trưởng không phải bot là `pb_RoiNhom("doi truong khong con")` ngay nhịp 5s kế; và xáo nhóm 10 phút cũng sẽ đá). |
| Câu PM cũ | `PB_WhisperReply` | "Minh dang di mot minh, de khi khac nhe." → "Ban cu moi minh vao nhom di, minh dong y lien." |

Bot **không** theo sau người chơi (khối bám đội trưởng hiện tại chỉ bám đội trưởng là bot, :9819) — chủ
chưa yêu cầu; muốn thêm thì bỏ gác `PB_IsBot(nCap)` ở khối đó (1 dòng, cần chủ duyệt vì đổi hành vi bot ngoài bãi).

## 3. Trường mới trong `PB_Bot` (server-local, không dính giao thức)

`nNhomNguoiIdx/dwNhomNguoiID` (người thật đã mời) · `nBangPha` (0/1) · `dwBangID` · `nBangNguoiIdx/dwBangNguoiID`
· `nBangTick` (hết hạn) · `nBangNghiToi` (A* thua) · `nBangDoiMapTick` (giãn ChangeWorld) · `nBangThu`.
Khởi tạo tại khối reset lúc sinh bot (cạnh `nPmCamToi`).

## 4. Nghiệm thu (sau restart)

1. Nhắn mật cho bot: `vao bang minh di` (người nhắn phải có bang) → bot trả "Ok! Minh ve NPC…", bot.log
   `[BotBang] <bot> nhan PM ru vao bang <tên>` → `dich chuyen ve map 53` (nếu khác map) → `nop don … ket qua=0|7`
   → người nhắn nhận PM "da nop don…" / "da gia nhap…"; bang chủ mở trang Chiêu mộ thấy đơn (hoặc thấy
   thành viên mới nếu bang đặt ngưỡng tự nhận).
2. Nhắn từ người **không có bang** → "Ban chua co bang hoi ma"; nhắn bằng chat kênh/gần → không có gì (đúng ý "nhắn mật").
3. Mời bot vào tổ đội (bot đang luyện, không TK) → bot vào ngay, PM "Ok, minh vao nhom nhe!", log
   `[BotNhomNguoi] … vao nhom cua nguoi choi …`; đổi map → bot rời, PM "Ban doi map roi…"; log `[BotNhom] … roi nhom (nguoi choi da doi map)`.
4. Bot đang Tống Kim / bán sạp → từ chối (log `tu choi loi moi`); người mời thấy "không thể mời" như cũ.

```
grep -E "BotBang|BotNhomNguoi" bot.log | tail -50
```

## 5. Đợt b — phản biện 5 tác tử trên bản đã thi công (`goi_va_bot_bang_nhom_b.py`, 13 hunk KPlayerBot.cpp)

| Lỗ hổng tìm ra | Vá |
|---|---|
| Bot hồi sinh ở map khác → báo sai "Ban doi map roi" | So subworld HIỆN TẠI của người mời với subworld **lúc vào nhóm** (`nNhomNguoiSub`) — chỉ người chơi đổi map mới rời |
| Giờ Tống Kim kéo bot đang ở nhóm người thật đi (rời nhóm im lặng) | `pb_TkDuTuCach`: `nNhomNguoiIdx` → không gọi quân |
| `SetCanTeamFlag(TRUE)` ép qua map cấm tổ đội | Bỏ ép; cờ FALSE → để đường cũ báo "không thể mời" |
| Từ khoá `bang` quá rộng với người gõ không dấu ("cap bang nhau", "bang gia") | Phải kèm `vao` / `vo bang` / `gia nh` / `xin` / `moi` / `ru ` / `join` / `bang h` |
| Câu "Ok! Minh ve NPC…" bị nuốt khi bot đang có câu trả lời chờ (`nPmDenHan`) | Xoá cả `nPmDenHan` trong nhánh bang |
| Bot đang bán sạp / giữa nhiệm vụ Dã Tẩu bị ChangeWorld về map 53 bỏ dở | Trả lời lịch sự, không nhận việc (`nBanSap`, `nDtPha != DTB_NGHI`); `PB_MoiVaoNhom` cũng từ chối khi giữa Dã Tẩu / đang xin bang |
| Rời nhóm người thật để đi xin bang mà không báo | Nhắn "Minh roi nhom di xin vao bang, xong viec quay lai nhe." trước khi rời |
| ChangeWorld cả loạt cùng khung; log từ chối spam theo cú bấm | So le 3-5s theo chỉ số bot; log từ chối giãn 3s |

Phản biện xác nhận: `PackDataToClient(-1)` **không sập** trên cây Windows (GameServer nạp heaven.dll, guard `ulnClientID < m_nPlayerMaxCount`; chỉ sập nếu build `_STANDALONE` — cấm); blob bot **có** lưu `dwTongID` (`KPlayerDBFuns.cpp:954` chạy cả cho bot) nhưng bot không qua `KPlayerTong::Login` sau restart nên tên bang/camp trống — việc chờ chủ quyết.

**Chờ chủ quyết (chưa làm):** (a) bot có theo sau người chơi không; (b) các nhánh tự đổi map khác của bot (ra bãi / về thành / Dã Tẩu) khi đang ở nhóm người thật — hiện vẫn tự rời như trước, chỉ Tống Kim đã gác; (c) người rủ là bang chủ thì tự duyệt luôn?; (d) PM "cảm ơn" khi được duyệt (cần móc `SSOI_TONG_ADD`); (e) đồng bộ tên bang/camp cho bot sau restart (gửi `GET_LOGIN_DATA` lúc `PB_OnRoleData`).

## 6. Chuỗi tái áp

… → tkket3_moxe → **bot_bang_nhom** → **bot_bang_nhom_b** (KPlayerBot.cpp/.h + KPlayerTeam.cpp). Cả ba tệp đã commit cùng script (`90f13d30` + đợt b).

## 7. Đợt c (01/09 tối, phiên wauto-34) — chủ ĐÍNH CHÍNH ý + 2 lỗi Tống Kim — `ReverseTools/goi_va_bot_nhom_bang_tk_c.py` (36 hunk, CHỈ KPlayerBot.cpp), commit **`f58c1fdc`**, `CoreServer.dll.moi` = **`bf680d6aa3c2`** (19:01) **CHỜ RESTART**

> Chủ: *"ở phiên trước đang bị hiểu sai ý tôi"* — 4 việc, nguyên văn ở đầu script. Không đổi giao thức, không đụng header,
> client KHÔNG cần deploy cho đợt này (KPlayerBot.cpp không biên dịch phía client — Core.vcxproj:798). `.moi` cũ `b71341e6`
> (DUNG LUYỆN phiên wauto-e7, 16:24) lưu ở `bin\server\_moi_backup\0109_b71341e6\`; bản mới là superset HEAD `2815d2d3`.
> ⚠️ Vì HEAD đã chứa DUNG LUYỆN server (chưa phản biện) mà `CoreClient.dll.moi`/`Game.exe.moi` 16:24 chưa có handler
> `s2c_syncfusion`, phiên wauto-e7 sẽ build lại và ghi đè cả 3 `.moi` (superset, có cả bot đợt c) — **restart sau khi họ đặt xong**;
> lệch chỉ bung khi có item Văn Cương/dung luyện.

### 7.1 Tổ đội với người chơi — luật mới

| Luật chủ giao | Làm ở đâu |
|---|---|
| Bot chưa có party (hoặc đang ở nhóm toàn bot) → tự vào party người mời, không cần PM | `PB_MoiVaoNhom` như cũ; **Dã Tẩu đang FARM (loại 4 trên map nhiệm vụ / loại 5-6 vừa luyện vừa làm) nay CŨNG nhận**; chỉ từ chối khi đang ĐI (tới NPC / thoại / lọc / Xa Phu / về trả / thưởng), TK, sạp, chết, đang xin bang. Cờ tổ đội mặc định của bot = 0 (Player[] mảng tĩnh, server không có ctor KPlayerTeam) → **bật `SetCanTeamFlag(TRUE)`** thay vì trả "không thể mời" oan. |
| Bot theo sau người chơi (đội trưởng) | Khối **bám theo đội trưởng** (`pb_DriveBot`, trước `pb_RaBai`) nhận cả đội trưởng NGƯỜI THẬT (`bTheoNguoi`): cách >200 MPS bỏ đánh chạy về, trong 200 đánh quanh; không cần "đúng bãi". Người chơi ở map khác (phù về thành) → bot luyện tại chỗ chờ. |
| Người chơi phù về thành/thôn → KHÔNG giải tán; lên map luyện công khác / map hoạt động → tự rời | Khối canh mỗi nhịp: subworld hiện tại của người mời ≠ lúc vào nhóm **và** map đó không phải thành/thôn (`pb_LaThanhThon` = 10 thành `s_dtNpc` + 6 thôn `THON_TT_MP_ARRAY` 20/53/99/100/101/121) → `pb_RoiNhomNguoi` (PM "Ban di map khac roi, minh roi nhom nhe."). Về thành rồi quay lại đúng map cũ → bám tiếp. |
| Bot không tự bỏ đi | `pb_RaBai`: đang ở đúng map nhóm → `return 1` (không chọn lại bãi, không đổi map); lúc vào nhóm đặt `nBaiIdx` = bãi của map nhóm (nếu có trong `s_bai`) để khi lạc (hồi sinh ở thành) đường Xa Phu đưa **về lại map nhóm mà không rời nhóm** (3 chỗ `pb_RoiNhom` trong pb_RaBai gác `!nNhomNguoiIdx`). Bot lạc khỏi map nhóm >5 phút mà người chơi vẫn ở map cũ → rời nhóm có PM. `PB_SetDaTau` / `PB_SetBanSap` không bốc bot đang ở nhóm người thật. |
| Đang làm Dã Tẩu → xong nhiệm vụ tự rời nhóm về trả | Loại 4: đủ cuốn (`DTB_FARM_NV` → `DTB_VE_TRA`) → PM "Minh xong nhiem vu Da Tau roi, roi nhom di tra nhiem vu nhe." rồi rời. Loại 5/6: tới kỳ 5 phút về NPC thử trả (= "xong" của bot) → rời có PM ở nhánh đổi map. Mọi nhánh tự đổi map của bot (Dã Tẩu tới NPC/Xa Phu, TK, về thành, xin bang) đi qua **một hàm `pb_RoiNhomNguoi`** — luôn PM trước khi rời. |

### 7.2 Bang hội — luật mới

1. Từ khoá (hạ chữ thường, ASCII): chứa **`vao bang`** / **`vo bang`** / **`tham gia bang`** (bao trọn 6 câu chủ liệt kê). Bỏ bộ khoá rộng đợt b.
2. Người nhắn **phải là BANG CHỦ**: `g_TongJX2.FindMember(bang, g_FileName2Id(tên))->btFigure == 0`; không phải → "Ban khong phai bang chu bang X, nho bang chu nhan cho minh nhe."
3. Thứ tự từng bước (`pb_XinVaoBang`): **B1** rời tổ đội (PM nếu nhóm người thật) → **B2** khác map thì dịch chuyển về map 53 cạnh NPC môn phái của bot (so le 3-5s) → **B3** đi bộ tới NPC → **B4 XUẤT SƯ**: nếu `GetSaveVal(4134)` (TASK_DUNGCHUNG2) == 0: cần cấp ≥60 (luật menu `thieulam.lua:19`), gọi `ExecuteScript(<script NPC môn phái>, "xuatsu")` = `factionhead.lua:32` (SetCamp(4)+SetCurCamp(4)+LeaveTeam()+SetTask(4134,1)), kiểm task = 1, nghỉ 2 giây → **B5** nộp đơn `DoClientOpBody(APPLY_JOIN)` như cũ + PM kết quả. Dã Tẩu đang làm vẫn nhận (xin bang xong máy trạng thái Dã Tẩu tự đồng bộ theo course).
4. ⚠️ **Hệ quả luật sẵn có của máy chủ**: xuất sư = **camp 4** → `mobinhtk.lua:100/:217` từ chối *"Chữ đỏ không thể vào Tống Kim"* và `pb_TkDuTuCach` chặn camp 4 → **bot đã xuất sư/vào bang sẽ KHÔNG được gọi đi Tống Kim nữa** (đúng luật đang áp cho người chơi thật). Chủ quyết: giữ luật hay mở camp 4 cho TK.

### 7.3 Bot Tống Kim đứng yên — GỐC đã tìm ra bằng log [TkKet3] + lưới thật, vá [TKKET4]

Đo trận 17:50–18:20 (server `3730dc63`, log TkKet3 sống từ restart 14:09): **81 bot kẹt, 100% trại Đông**, 6.160 dòng `[TkKet3-RA]` đều `nW=-1`, chỉ 5 đích: (1665,3106) (1666,3107) (1666,3109) (1667,3110) (1674,3108); **0 dòng `[BotA*]` trên map 379**; `[TkKet3-TRAP]/[TRAP2]/[CONG]/[QY]` = 0 ⇒ bot chết ở khâu `PB_WalkTo` nhưng A* không "lỗi" — rơi vào nhánh **ĐƯỜNG CỤT im lặng** (`FindPathServer` trả 2). Đọc `Maps\379_srv.fp` (magic SFP06, node 20 B, obs offset 16):

```
     x=1652 ..                                  1689
3098 .......##XT...................########      T = ô trap đi được   X = ô trap là VẬT CẢN
3102 .........####TT.................######      hành lang trap kimratrai (1661+i,3098+i) là KHE TƯỜNG
3103 .........#####XX##..............######      từ i>=6 các ô trap nằm trong tường
3106 ..............###XX#######..........##      (1665,3106) = đích pb_ODat chọn = phía NGOÀI tường
```

`pb_ODat` quét xoắn ốc quanh ô trap chỉ hỏi "engine trống + lưới không chặn" → chọn ô trống **bên kia tường** → A* dẫn tới block gần nhất rồi `-1` mỗi nhịp → bot đứng sát cửa, vùng-an-toàn làm tươi `nTkTick` nên 12 phút sau mới bị cắt "KẾT pha 3" và **rời trận luôn**. Kèm: 152 dòng `[BotTrap] id=1975C4C8` (kimratrai) + 224 `EA163CE9` (tongratrai) = bot **có đạp đúng trap ra trại** nhưng bị "miễn".

Vá (`pb_TkRaTrai` + `PB_TrapLog` + pha 3): (a) đứng trong **2 ô** quanh bất kỳ ô trap nào của vết (cả hàng nY và nY-1) = đã tới cửa → bước qua trap; (b) đích = **chính ô trap** có `CellObsSrv==0` **và** `FindPathServer==1` (nối được từ chỗ bot), xoay theo `b.nTkRaXoay`; (c) `PB_WalkTo` thua → xoay ô khác; (d) **bot pha 3 đạp trap ra trại → chạy kịch bản như người chơi** ngay trong `PB_TrapLog` (`ExecuteScript(id,"main",idx)` — đúng chỗ engine gọi cho người thật), pha 3 thấy mình ngoài hộp 40 ô quanh hậu doanh → pha 4 "RA TRAN". Log mới: `[BotTK] X dap trap ra trai ... DA RA TRAN|kich ban tu choi`, `[BotTK] X phe ... da o ngoai trai ... -> RA TRAN`, `[TkKet3-RA] ... xoay=%d noi=%d`.

### 7.4 "Bot TK xong đứng trong map báo danh" — GỐC + vá [TKCHET]

Sau 18:20:01 (TRẬN ĐÓNG) census `pha5=13` đứng im 17 phút; 13 con đó mỗi giây ghi `da chet (mau 0, doing 1) -> tu hoi sinh` ×991 lần. `doing 1 = do_stand` mà máu 0: task03 hết trận `NewWorld` cả đàn về 324 **đúng lúc bot đang chết** → `KNpc::ChangeWorld` gọi `DoStand()` (KNpc.cpp:10368) xoá do_death/do_revive; `KPlayer::Revive` (KPlayer.cpp:6944) đòi **đúng do_revive** mới hồi sinh → nuốt mãi = xác đứng thẳng giữa map báo danh. Vá ở khối tự hồi sinh của bot: máu ≤0 mà doing không phải do_death/do_revive → ép `m_Doing = do_revive` (đúng trạng thái OnDeath đặt ở KNpc.cpp:2316) rồi Revive như thường; log `[Bot] X XAC DUNG THANG (...) -> ep ve do_revive`.

### 7.5 Nghiệm thu sau restart

```
grep -E "BotNhomNguoi|BotBang|XAC DUNG THANG|dap trap ra trai|da o ngoai trai|TkKet3-RA" bot.log | tail -80
```
1. Mời bot (đang luyện / đang farm Dã Tẩu) vào tổ đội → vào ngay, PM "Ok, minh vao nhom nhe!", bot chạy theo khi cách >6 ô. Phù về thành (Thổ Địa Phù / Thần Hành Phù về Ba Lăng Huyện…) → bot **không** rời; sang map luyện khác / TK / hoạt động → bot rời + PM. Bot Dã Tẩu đủ cuốn → PM "xong nhiem vu ... di tra" rồi rời.
2. Bang chủ nhắn mật "vao bang" → bot PM "Ok! Minh ve Ba Lang Huyen gap NPC mon phai xuat su roi xin vao bang X ngay day." → log `[BotBang] ... da XUAT SU tai NPC ...` → `nop don ... ket qua=0|7`; người không phải bang chủ → "Ban khong phai bang chu...". Bot <60 → PM huỷ.
3. Trận TK kế: `[TkKet3-RA] nW=-1` phải về ~0, `[TkKet3]` "dung Ns" chỉ còn churn (<60-90s), xuất hiện `dap trap ra trai ... DA RA TRAN`; sau trận census `pha5` về 0 trong ~1 phút, không còn dòng `da chet (mau 0`.

**Chờ chủ quyết:** (a) bot xuất sư/vào bang bị luật camp 4 cấm Tống Kim — giữ hay mở; (b) bộ log `[TkKet3]` (6.000 dòng/trận) giữ thêm 1 trận để nghiệm thu rồi tắt; (c) 619 dòng `[TkKet3-MUA]` "không đường tới Quân Y" cùng gốc pb_ODat — bot chỉ bỏ mua thuốc, chưa vá (vá tương tự nếu chủ muốn).

## 8. Chuỗi tái áp

… → tkket3_moxe → bot_bang_nhom → bot_bang_nhom_b → **bot_nhom_bang_tk_c** (`--thu` = chỉ kiểm neo). Tất cả đã commit.

## 9. Đợt d (02/09 00:2x) — màu bang cho bot + ngưỡng cấp 90 + tắt nhắn mật tổ đội — `ReverseTools/goi_va_bot_bang_mau_d.py` (18 hunk, CHỈ KPlayerBot.cpp), commit **`7f041f8d`**, `CoreServer.dll.moi` = **`3bedd3ac7b3f`** (00:21) **CHỜ RESTART**

> Chủ giao thêm: *"Bot đã vào bang thành công sẽ có màu riêng theo bang (thêm giới hạn bot trên cấp 90 người chơi nhắn chat mật mới về xuất sư - xin vào bang)"* và *"khi bot vào paty và rời paty không cần nhắn chat mật cho người chơi — bạn hãy tắt cái đó đi"*.
> Đợt c đã **CHẠY THẬT** từ 00:19 (server `9d7ae996`, bản của phiên Hoa Sơn đã cuốn commit `f58c1fdc`).

### 9.1 Màu riêng theo bang

"Màu" của một người chơi = `KNpc::m_CurrentCamp`; bang hội đặt màu này = **camp của bang** (`KTongJX2Tong::btCamp` — 1 chính phái / 2 tà phái / 3 trung lập). Đường của người thật: relay báo về → `SSOI_TONG_ADD` → `KPlayerTong::AddTong` (KPlayerTong.cpp:571) gán `Npc.m_Camp` + `m_CurrentCamp`; đăng nhập lại thì `SGDI_TONG_LOGIN` → `KPlayerTong::Login` (:1364) làm y hệt. Camp 4 (tự do) là **điều kiện bắt buộc để xin vào bang** — chính là lý do phải xuất sư trước.

Bot vấp **hai lỗ hổng**, hàm mới `pb_DongBoBang` (10 giây/con, so le theo chỉ số bot) vá cả hai, không thêm gói tin, không đổi cấu trúc:

| Lỗ hổng | Vá |
|---|---|
| `AddTong` gán **thẳng** `m_CurrentCamp` nên **không phát gói** → người đang đứng cạnh bot không thấy đổi màu cho tới khi đồng bộ lại vùng | Gọi `KNpc::SetCurrentCamp` (KNpc.cpp:475) — đúng hàm **BROADCAST** `s2c_npcchgcurcamp` cho 9 vùng mà engine dùng cho mọi thay đổi màu khác |
| Sau **restart** bot không qua `KPlayerTong::Login` (không có bắt tay với relay): blob chỉ trả lại `dwTongID` (KPlayerDBFuns.cpp:357 `DBSetTongNameID`, có đặt `m_nFlag = 1`) còn **tên bang / camp / chức vụ đều trống** → bot mất màu bang và mất tên bang trên đầu | Đọc bản sao `g_TongJX2` (relay đồng bộ về): `FindTong(id)->btCamp/szName` + `FindMember(...)->btFigure` + tên bang chủ (thành viên `btFigure == 0`) → điền lại `m_cTong` |

Tên bang trên đầu tự có: `KNpc::SendSyncData` (KNpc.cpp:5966) lấy thẳng từ `Player[].m_cTong.GetTongName()` — bot là `KPlayer` **thật** nên điền `m_cTong` là đủ.

Không đụng vào màu khi: đang **Tống Kim** (script đặt camp phe Tống/Kim) và đang ở **tổ đội** (`AcceptTeam` ghi đè camp thành viên bằng camp đội trưởng — đúng luật `KPlayerTong::Login` chỉ đặt `CurrentCamp` khi `!m_cTeam.m_nFlag`). Bang giải tán / bot bị đuổi lúc offline → xoá hồ sơ bang, **cố ý không đụng vào camp** (đổi là đổi gameplay).

**🔴 ĐÍNH CHÍNH mục 7.2 điểm 4:** xuất sư đặt camp 4 chỉ là trạng thái **TẠM**. Vào bang xong `AddTong` đặt camp = camp của bang (1/2/3) nên bot **lại đủ tư cách đi Tống Kim** (`mobinhtk.lua:100` chỉ chặn camp 4). Chỉ bot đã xuất sư mà đơn **chưa được duyệt** mới tạm mang camp 4.

### 9.2 Ngưỡng cấp 90

`PB_CAP_VAOBANG 90`, kiểm ngay trong `PB_WhisperReply` — trước khi bot bỏ việc đang làm. Chặt hơn luật của chính game (client đòi cấp ≥ 60 để gia nhập, KPlayerTong.cpp:196). Bot dưới 90 trả lời *"Minh moi cap N, phai tu cap 90 tro len minh moi dam xuat su xin vao bang."*

### 9.3 Tắt nhắn mật khi vào / rời tổ đội

Bỏ câu "Ok, minh vao nhom nhe!" trong `PB_MoiVaoNhom` và bỏ tham số `szPm` của `pb_RoiNhomNguoi` (9 chỗ gọi: người chơi sang map khác, bot lạc 5 phút, Dã Tẩu tới NPC / Xa Phu / về trả / đủ cuốn, về thành, Tống Kim, đi xin bang). Lý do rời nhóm **chỉ còn vào `bot.log`**. Vẫn giữ câu **trả lời** khi người chơi hỏi trực tiếp (kể cả câu "Ban cu moi minh vao nhom di, minh dong y lien.").

### 9.4 Nghiệm thu (sau restart)

```
grep -E "BotBang|BotNhomNguoi" bot.log | tail -60
```
1. Mời bot vào tổ đội / bot rời nhóm → **không** còn tin nhắn riêng nào, chỉ có dòng log.
2. Bang chủ nhắn "vao bang" cho bot **dưới 90** → bot trả lời từ chối theo cấp; bot ≥ 90 → chạy chuỗi xuất sư → nộp đơn như mục 7.2.
3. Bot đã ở trong bang: đứng cạnh nó thấy **màu theo camp bang** + tên bang trên đầu; log `[BotBang] X doi mau theo bang 'Y': camp A -> B`. Restart server rồi vào lại: sau ~10 giây bot tự hiện lại màu/tên bang, log `[BotBang] X dong bo bang 'Y' (id ..., camp ..., chuc vu ...) tu ban sao relay`.

### 9.5 Chuỗi tái áp (cập nhật)

… → tkket3_moxe → bot_bang_nhom → bot_bang_nhom_b → bot_nhom_bang_tk_c → **bot_bang_mau_d** (`--thu` = chỉ kiểm neo). Tất cả đã commit + push.

## 10. Đợt e (02/09) — "vào party bot chưa hiện icon môn phái ở mini party" — `ReverseTools/goi_va_bot_phaidau_e.py` (3 hunk, CHỈ KPlayerBot.cpp), commit **`36901402`**

### 10.1 Lần ngược đường mã (không đoán một bước nào)

```
mini party  KUiTeamManager2::SetFactionIcon(m_pPlayersList[i].nFaction, …)   UiTeamManager2.cpp:186 + :82-129
              switch 0..12 -> icon_zd_sl/tw/tm/wd/em/cy/gb/tr/wu/kl/hsp/wht/xy.spr
              default:  icon_zd_new.spr   <-- ĐÚNG CÁI CHỦ THẤY (ô trống, không phải icon phái)
   <- nFaction  KTeam::GetMemberInfo (KPlayerTeam.cpp:871 và :890, khối #ifndef _SERVER)
                 pList[].nFaction = Npc[nNpcIdx].nFirstFaction;
   <- client   KProtocolProcess.cpp:2796 / :2894   Npc[nIdx].nFirstFaction = pPlaySync->nFirstFaction
   <- server   KNpc::SendSyncData (KNpc.cpp:6086) / SendSyncDataToNearRegion (:6303)
                 PlayerSync.nFirstFaction = (BYTE)Player[m_nPlayerIdx].GetFirstAddFaction()
   <-          KPlayer.h:849  ->  m_cFaction.m_nFirstAddFaction
```

**Gốc:** `m_nFirstAddFaction` chỉ được gán ở **đúng một chỗ** — `KPlayerFaction::AddFaction` (KPlayerFaction.cpp:88):

```cpp
m_nAddTimes++;
if (m_nAddTimes == 1)          // <-- chỉ lần gia nhập ĐẦU TIÊN của cả đời nhân vật
    m_nFirstAddFaction = nFactionID;
```

mà `m_nAddTimes` **nạp từ blob** (`KPlayerDBFuns.cpp:376` = `BaseInfo.ijoincount`, lưu lại ở `:1016`). Bot nhân bản từ **nhân vật mẫu đã từng vào phái** ⇒ `ijoincount ≥ 1` ngay khi nạp ⇒ lúc bot chạy `gianhapmonphai` → `SetFaction` → `KPlayer::AddFaction` (KPlayer.cpp:4249) thì `m_nAddTimes` thành 2/3/… **không bao giờ bằng 1** ⇒ `m_nFirstAddFaction` giữ nguyên giá trị của nhân vật mẫu (−1 = chưa vào phái) ⇒ gói sync mang `nFirstFaction = 255` ⇒ mini party rơi vào `default:` = ô trống.

### 10.2 Vá (chỉ chạm bot)

Ép `m_nFirstAddFaction` = **phái hiện tại** của bot, ở hai chỗ:
1. **Ngay khi vào phái xong** (nhánh `PB_AI_GOTO_FACTION` thành công, cạnh dòng log `[Bot] X da vao <phái>`).
2. **Nhịp 10 giây/con** `pb_SuaPhaiDau` (lệch pha với `pb_DongBoBang`) — cho bot đã nạp từ blob cũ, không cần đợi vào phái lại.

Log: `[BotPhai] X sua PHAI DAU (first faction) A -> B …`. Kèm giữ bất biến `m_nAddTimes ≥ 1`.

Đúng nghĩa với một nhân vật chỉ từng vào **một** phái, và **kéo theo đúng hướng** hai chỗ khác cùng đọc trường này: item `magic_requiremenpai` (KItemList.cpp:1204) và bảng thành viên bang hội JX2 (KTongJX2.cpp:2428 — icon phái trong cửa sổ bang).

**Hiển thị:** `KPlayer::SendFactionData` (KPlayer.cpp:1922) chỉ gửi cho **chính chủ** (`m_nNetConnectIdx`) — bot không có client nên là no-op; người chơi quanh đó nhận giá trị mới ở **lần đồng bộ vùng kế tiếp** (PLAYER_SYNC), y hệt người chơi thật đổi phái. Vì vậy vá sửa **sớm** (lúc nạp / vừa vào phái) và giá trị được lưu vào blob nên chỉ phải sửa một lần cho mỗi bot.

### 10.3 Nghiệm thu

```
grep "BotPhai" bot.log | head -20
```
Sau restart: mỗi bot in **một** dòng `[BotPhai] … sua PHAI DAU … -> N`. Mời bot vào tổ đội → mini party hiện **đúng icon phái của bot** (Thiếu Lâm / Thiên Vương / … / Hoa Sơn); bot trong bang cũng hiện đúng icon phái ở cửa sổ bang hội.
