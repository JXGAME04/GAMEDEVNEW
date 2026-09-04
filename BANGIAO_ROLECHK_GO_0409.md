# GỠ ROLECHK 7af20d82 + GỐC THẬT VỤ SẬP 15:05:38 — 04/09/2026 (phiên wauto-go-rolechk)

Chủ yêu cầu: *"gỡ đi 7af20d82 (làm cho không gọi bot được) và tìm ra nguyên nhân, fix"*.

## 0. TRẠNG THÁI NHỊ PHÂN (16:45)

| Tệp | Trạng thái | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll` **đang chạy** = `f576ac9e` (từ 16:20:08) | wauto-4a build: origin/main `9fb1f175` (gỡ RoleChk) + chienlenh `a18b5eb8` + DAUGIA-CHAT | **bot đã gọi được lại: 983/1000 lúc 16:25** |
| `bin\server\CoreServer.dll.moi` | **TRỐNG** (chủ đã chạy bat) | |
| `bin\server\CoreServer.dll.moi.rolechk2_99765285` | **ứng viên**, cạnh khe | = origin/main `d028ea41` + chienlenh `a18b5eb8` + rào `[RoleChk2]` (mục 4) — chủ muốn dùng thì đổi tên thành `CoreServer.dll.moi` rồi `ChayGameServer.bat` |
| `bin\server\rainbow.dll.moi` = `e8056284` (x64) | **ứng viên**, cạnh khe | **vá gốc** (mục 3). `ChayGameServer.bat` chưa có dòng `call :capnhat rainbow.dll` — thêm dòng đó (ngay dưới `call :capnhat heaven.dll`) rồi chạy bat; hoặc tắt GameServer, đổi tên tay |
| `bin\server\GameServer.exe.moi.recv_02b583ad` | **ứng viên**, cạnh khe | link DB 128 KB → 8 MB + bỏ gói role-data ghép sai độ dài. GameServer.exe đang chạy (27/08) build từ cây nào không rõ (md5 khác bản trong repo cùng giờ) → chủ cân nhắc |
| `bin\multiserver\heaven.dll.moi` = `ad7e6985` (x86) | **ứng viên**, cạnh khe | heaven.dll x86 mà **Goddess/Bishop** đang nạp là bản **19/04** chưa có vá `[SOCK 04/09]` (gửi thiếu thì vứt). Thay khi restart cụm |

Git: `origin/main` = `d028ea41` (gồm `9fb1f175` gỡ RoleChk + `d028ea41` RECV). Nhánh `rolechk2-0409` = `252d0268` (đã push, chưa gộp main — chờ chủ).

## 1. VÌ SAO 7af20d82 LÀM "KHÔNG GỌI BOT ĐƯỢC"

`bot.log`: **2000/2000 dòng** `[RoleChk] <bot>: blob HONG o 'Friend'` mỗi lượt gọi (15:33, 15:37, 15:43, 15:53, 16:03, 16:13) → `hethong.log` "bot 0/1000" suốt 15:30–16:20. Boot trước đó (8181a940, 15:08) nạp 969 bot bình thường.

Gốc: `TRoleData` có `dwFriendOffset`/`nFriendCount` (`Core/lib/s3dbinterface.h:86,92`) nhưng **engine Core không bao giờ ghi hai trường này**: `SavePlayerInfo` `memset(pRoleData, 0, …)` (`KPlayerDBFuns.cpp:986`) rồi chỉ đặt FSkill/SSkill/Task/Item/DataLen (:1087, :1230, :1209, :1307, :1194). Chỉ `Intercessor/PlayerCreator.cpp:195` và Sword3PaySys dùng. ⇒ `dwFriendOffset = 0` trong **mọi** blob → RoleChk kiểm `aOff[v] < nHdr` (0 < 745) ở vòng Friend → từ chối cả 1000 bot, kể cả blob hoàn toàn hợp lệ.

Đã gỡ nguyên commit: `9fb1f175` (chỉ `KPlayerBot.cpp`, 55 dòng). **Bài học: kiểm hợp lệ theo trường nào thì phải ĐO trường đó trên dữ liệu thật trước** — `pymysql` root/123456 `jx1_role.role`, 1003 blob, mất 10 giây (mục 4 có bảng đo).

## 2. GỐC THẬT VỤ SẬP 15:05:38 — CHỨNG MINH BẰNG DUMP ↔ MySQL

Phiên trước (wauto-75) đoán `CPackager::PackUp` ghép xen kẽ vì "gói cùng cID" — **sai** (Goddess `_GetRoleInfo` gửi trọn mọi gói của một thông điệp trong một vòng lặp, một luồng). Cách chứng minh thật:

1. `scratchpad\dump_ctx.py` (dựa `ReverseTools\crash\doc_dump.py`): đọc CONTEXT x64 → `rdx` = `m_SaveBuffer` (rbx+0xCC) của Player[785] LyMinh812, lấy 24 KB.
2. So từng byte với `jx1_role.role` và ảnh chụp `role_history` 14:36 (9070 B, đúng cỡ lúc sập): **pack 1 (blob 0..498) = LyMinh812** (chỉ lệch 4 byte toạ độ); **từ 0x1F3 = 499 = ranh giới gói 510 byte** (`GetHeadPack(512-2)`) là chuỗi bản ghi nhiệm vụ 8 byte.
3. So **giá trị** từng bản ghi (id, value) với 8 bot lân cận theo thứ tự xin trong `goddess_log.txt` 15:05:37: **PhanNghia813 (bot xin ngay sau LyMinh812) khớp 49/49**; LyMinh812 chỉ 13/49; các bot khác 9–35/49. Bản ghi 387 của PhanNghia813 nằm ở offset 1017 (ảnh 14:42) → lệch 518 ổn định qua 3 cửa sổ liên tiếp; không có header khung (`06 02 0A …`) nào lẫn trong bộ đệm.
   ⇒ Thông điệp ghép = **gói 1 của LyMinh812 + gói 3.. của PhanNghia813**; mất đúng L2..L18 + P1, P2 = **19 khung × 518 = 10.261 byte**, phần sau liền mạch.
4. Khoá XOR của link **không lăn**: `KSG_DecodeBuf/EncodeBuf` (Common/KSG_EncodeDecode.h) sao chép `*puKey` ra biến cục bộ rồi mới lăn → mất khung không làm rác phần sau → đúng quan sát.

### Điểm vứt duy nhất trên đường nhận của GameServer

`m_pDatabaseClient` = `CGameClient` trong **Rainbow.dll** (tạo qua `IClientFactory`, `KSOServer.cpp:686`; **không** phải `KSocketClient2` hay `zbuffer/IClient`). Một luồng `WSAEventSelect`, mỗi `WSARecv` ≤ 10 KB → `ReadCompleted` → `m_pRecvBuffer->AddData()`.
**`CIOBuffer::AddData` (`Common/IOBuffer.cpp:90-97`) thiếu chỗ thì `return` im lặng** (`throw` đã bị chú thích; `rainbow.dll` đang chạy 23/04 không có chuỗi "Not enough space"). Bộ đệm này **128 KB** (`SetEnvironment(1024*128)`, `KSOServer.cpp:688`) = 0,4 giây ở ~300 KB/s khi nạp 1000 bot (22 bot/s × 10–13 KB). Luồng chính (`MessageLoop`, 18 Hz, nạp bot đồng bộ trong `PB_OnRoleData`) kẹt hơn 0,4 s là **mất trọn 1–2 lần đọc mà kết nối vẫn sống** → `CPackager::PackUp` (`Buffer.cpp:432`) ghép Header của A với Middle của B → `dwFSkillOffset = 0x28000000` → `KPlayer::LoadDBPlayerInfo` đọc ngoài vùng → 0xC0000005. Không tất định, DB nguyên vẹn — đúng mọi dấu hiệu.

Phía gửi (Goddess) cũng có lỗ cùng loại nhưng cần điều kiện hiếm hơn: heaven.dll x86 **19/04** trong `bin\multiserver` chưa có vá `[SOCK 04/09]` (completion thiếu byte thì vứt), và `WSASend` lỗi tức thì → `CSocketServer::OnConnectionError` bản Release **rỗng** → rơi khung im lặng.

## 3. VÁ GỐC — commit `d028ea41` "RECV 04/09" (origin/main)

| Tệp | Sửa |
|---|---|
| `Rainbow/ClientStage.cpp` `ReadCompleted` | hết chỗ → **chờ** luồng chính lấy bớt (Sleep 1 ms, backpressure TCP về Goddess), **không bao giờ vứt**; kẹt quá 30 s → đóng kết nối + `printf` báo to (thay vì đọc rác) |
| `Rainbow/ClientStage.cpp` `CreateClientInterface` | sàn dưới **4 MB** cho bộ đệm nhận mọi client (~13 s đệm), không cần build GameServer.exe mới có hiệu lực |
| `GameServer/KSOServer.cpp:688` | link DB 128 KB → **8 MB** (ăn khi build GameServer.exe) |
| `GameServer/KSOServer.cpp` `DatabaseLargePackProcess` | gói role-data ghép xong mà độ dài ≠ `nDataLen` khai báo → **bỏ + printf**, không giao rác cho Core |

Build: `D:\GAMEDEVNEW_wt_s13\build_net.bat` (Common+Rainbow x64, Common+Heaven Win32, ~20 s). Bẫy: post-build của Common ghi đè `Lib\release\common.lib` là tệp **tracked** → `git checkout -- Lib/release/common.lib` sau khi build.

## 4. RÀO `[RoleChk2]` — nhánh `rolechk2-0409` (252d0268), CHỜ CHỦ QUYẾT

Đo trên **toàn bộ 1003 blob** `jx1_role.role` (16:40):

| Bất biến | Khớp |
|---|---|
| `dwDataLen == cỡ blob` (và `data_len`) | 1003/1003 |
| `dwFSkillOffset == sizeof(TRoleData)-1 == 745` | 1003/1003 |
| `745 ≤ dwSSkill ≤ dwTask ≤ dwItem ≤ dwDataLen` | 1003/1003 |
| `dwTask − dwSSkill == 8·nStateSkillCount` | 1003/1003 |
| `dwItem − dwTask == 8·nTaskCount` | 1003/1003 |
| `dwDataLen − dwItem − 233·nItemCount ∈ {0, 4}` (4 = có CRC) | 1003/1003 |
| `dwFriendOffset == 0` | 1003/1003 (nên **không kiểm**) |
| `dwSSkill − dwFSkill == 8·nFightSkillCount` | **0/1003** → trường này không phải số bản ghi, **không bao giờ kiểm** |

Rào chỉ dùng 6 dòng đầu; blob hỏng thì ghi `[RoleChk2] … blob HONG (lý do)` vào `bot.log`, bỏ đúng con bot đó. Vụ 15:05 bị chặn ngay điều kiện đầu (`dwDataLen` ≠ cỡ blob). Đã biên dịch sạch (ứng viên `CoreServer.dll.moi.rolechk2_99765285`). Tôi **không** tự đặt vào khe vì chủ vừa yêu cầu gỡ RoleChk — chủ chọn.

## 5. CÒN MỞ / CHỦ QUYẾT

1. Áp `rainbow.dll.moi` (vá gốc, phía GameServer) — cần thêm 1 dòng vào `ChayGameServer.bat` hoặc đổi tên tay khi server tắt.
2. Áp `GameServer.exe.moi.recv_02b583ad` (8 MB + kiểm độ dài) — kèm rủi ro nguồn gốc exe đang chạy.
3. Áp `multiserver\heaven.dll.moi` cho Goddess/Bishop khi restart cụm (vá SOCK phía gửi).
4. Gộp `rolechk2-0409` vào main + dùng `CoreServer.dll.moi.rolechk2_99765285` (hay không).
5. Lỗ còn lại chưa vá: `CSocketServer::OnConnectionError` (Common) khi `WSASend` lỗi tức thì → nên đóng socket thay vì rơi khung; `CPackager::PackUp` gặp Middle khi chưa có Header nên bỏ + log. Chỉ thêm kiểm, không đổi giao thức.
6. Gợi ý vận hành: `[BOTAUTO]` xin 1000 bot một lượt là điều kiện đủ để tràn 128 KB; sau khi áp Rainbow mới thì không còn mất dữ liệu, chỉ chậm lại.

## 6. CÔNG CỤ ĐỂ LẠI (scratchpad phiên)

`dump_ctx.py` (CONTEXT + đọc vùng nhớ tại rdx/rbx+0xCC), `quet_khung.py` (tìm khung 518 byte trong dump), `patch_recv.py` (vá latin-1 giữ CRLF, neo phải xuất hiện đúng 1 lần). Blob lấy từ MySQL: `role_history (role_name, saved_at, role_blob)` ảnh 30 phút/lần — rất quý để đối chiếu dump.
