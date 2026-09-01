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
