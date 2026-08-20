# BÀN GIAO: BẢNG "CHỈ NAM NHIỆM VỤ" GẮN PHÍM F11 (cửa sổ `NewTask`)

Ngày: 19/08/2026 tối · Commit: **`4618a2d0`** (đã push) · Phân tích nền: `PHANTICH_BANG_NHIEMVU_F12.md`
Yêu cầu gốc: *"thay 100% bản thông báo nhiệm vụ F11 ở dự án bằng bản tham khảo linux (bảng F12) — vẫn để phím F11; hiển thị giống bản linux và hiển thị thông tin nhiệm vụ cho Dã Tẩu hiện tại"*.

---

## 1 · Một phút nắm việc

- **F11** (và cả **F12** — autoexec sẵn có `Open([[NewTask]])`) giờ mở **bảng Chỉ nam nhiệm vụ** — lớp mới `KUiTaskGuide`, bố cục + ảnh SPR **y hệt bản tham chiếu** (`taskguide.ini` nguyên văn, ảnh nằm sẵn trong pak — không vẽ mới cái nào).
- Khung trái: danh mục từ `\Ui\uitasklist.ini` (nhóm "Nhiệm vụ hằng ngày" + "+Nhiệm vụ Dã Tẩu"). Khung phải: nội dung **Dã Tẩu port 1:1 từ `taskui_random.lua`** (6 loại nhiệm vụ, tiến độ 1025/1027, hiệu số danh vọng/phúc duyên/Tống Kim), thêm dòng cuối "Hôm nay đã làm X/40, đã hủy Y".
- Sổ tay cũ `KUiTaskNote` **không bị xóa**: chỉ gỡ khỏi phím; đường lưu `AddNote`→`MissionMemory.dat` vẫn chạy (WakeUp không tự mở cửa sổ nên không có cửa sổ ma).
- **Đồng bộ task id ≥256 xuống client** (nút thắt trong PHANTICH mục 7) giải bằng kênh **không đổi wire-format**: `PLAYER_SCRIPTACTION_SYNC` (biến độ dài, sẵn có) + UIId mới `UI_TASKVALUE`. **Không đổi kích thước/loại gói tin nào** — client cũ gặp UIId lạ thì rơi khỏi switch (không có default) ⇒ không bắt buộc nâng cấp đồng loạt.

## 2 · Trạng thái triển khai (19/08 ~18:47)

| Tệp | Nơi | Ghi chú |
|---|---|---|
| `Game.exe` (1.245.696 B) | `TESTLOFFF_ONLINE\bin\client` | chứa KUiTaskGuide; bản cũ = `Game_cu_1908_tg.exe` |
| `CoreClient.dll` (2.181.120 B) | như trên | nhận UI_TASKVALUE + GDI mới; bản cũ = `CoreClient_cu_1908_tg.dll` |
| `Ui\Ui3\uitaskguide\taskguide.ini` | như trên | nguyên văn bản tham chiếu **+ `Selable=1` và `SelBgColor` vào `[TaskList_List]`** (bắt buộc — thiếu là click không chọn được dòng, `WndMessageListBox.cpp:309,676`) |
| `Ui\uitasklist.ini` | như trên | tối giản 2 mục; byte tên lấy nguyên văn từ bản tham chiếu |
| `settings\task\tasklink_*.txt` ×6 | như trên | **copy từ bảng server đang sống** (`bin\server\settings\task`) — đĩa thắng pak nên đè bản cũ trong `update01.pak` |
| Bản git của tài nguyên | `D:\GAMEDEVNEW\TaskGuideRes\` | đổi bảng server thì copy lại sang client + cập nhật đây |

`re_pe_crt.py`: CoreClient=CRT-TĨNH · Game.exe=UCRT-RELEASE — **PASS**. Game đang chạy phải **khởi động lại** mới nhận DLL/EXE mới.

## 3 · ⚠️ SERVER CHƯA MANG THAY ĐỔI — việc còn lại duy nhất

Build **Server Release x64** hiện **bị chặn bởi tệp của phiên làm việc song song** (bot): `KPlayerBot.cpp:6099` gọi `pb_FindRoamSpot` 6 đối số trong khi hàm nhận 7 — **sửa dở, không liên quan đợt này**; tôi không đụng vào tệp đó.

- Thay đổi server của đợt này (`KPlayer::SyncTaskValueToClient`, `LuaJX2_SyncTaskValue`, xóa-bảng lúc nạp nhân vật) **đã nằm trong cây D** ⇒ **lần build Server x64 kế tiếp tự mang theo**, không cần làm gì thêm ngoài build + thay binary + restart như quy trình bot vẫn làm.
- **Trước khi server được cập nhật**: bảng vẫn mở được, hiện "Bạn chưa tiếp nhận nhiệm vụ của Dã Tẩu…" + "0/40" (client không có dữ liệu id ≥256) — không lỗi, không crash.
- **Sau khi server cập nhật + restart**: đăng nhập lại một lần để nhận đủ trạng thái (đăng nhập đẩy toàn bộ task đã lưu; trong phiên thì mỗi `SetTask`/`SyncTaskValue` tự đẩy và bảng tự vẽ lại).

## 4 · Hướng dẫn test (theo thứ tự)

1. Khởi động lại Game.exe → đăng nhập → bấm **F11**: bảng 506×355 "Chỉ nam nhiệm vụ" hiện ra, khung trái có "+Nhiệm vụ Dã Tẩu", 3 nút dưới (Bỏ/Theo dõi/Hủy theo dõi) **mờ** — cố ý, xem mục 5.3.
2. Bấm **F12**: cùng bảng (bật/tắt).
3. Khi server CHƯA cập nhật: nội dung = dòng "chưa tiếp nhận…" + "Hôm nay đã làm 0/40…".
4. Sau khi server cập nhật + relogin: nhận nhiệm vụ Dã Tẩu ở NPC → mở F11 → thấy đúng loại nhiệm vụ (ví dụ loại 4: "Đến **map** tìm **N** quyển Địa đồ chỉ… đã thu thập được **X** tấm"); nhặt thêm cuộn → bảng **tự cập nhật số X** (không cần đóng mở).
5. Trả nhiệm vụ xong (course 2/3) → tiền tố đổi thành "[hoàn thành - hãy về gặp Dã Tẩu]".
6. Đổi nhân vật cùng client: bảng phải hiện số của nhân vật mới (server gửi lệnh xóa id=-1 lúc nạp).

## 5 · Cạm bẫy & quyết định thiết kế (đọc trước khi sửa tiếp)

1. **Cấm đổi `TASK_VALUE_SYNC`** (BYTE nTaskId, 6 byte pack(1)) — đó là lý do dùng kênh script-action. Ai định "sửa cho gọn" thành WORD/int sẽ làm **lệch stream với mọi client cũ** (BANGIAO_AUTO_DATAU §3.1 từng cấm).
2. **Hai thế hệ `taskui_*.lua` lệch 1 tham số** (bản pak client ta 2 tham số vs bản tham chiếu 1 tham số) — vì vậy KHÔNG port tầng Lua client; nội dung soạn trong C++ (`UiTaskGuide.cpp::BuildDaTauText`), chuỗi TCVN3 sinh tự động ở `UiTaskGuideStr.h` (đừng sửa tay — sửa `scratchpad/mine/gen_strings.py` sinh lại, hoặc sửa octal có chủ đích).
3. **3 nút dưới bị Enable(false) là trung thực với bản gốc**: nút Bỏ nhiệm vụ là mã chết cả hai đầu (enum `emSCRIPT_PROTOCAL_TaskGuide_QuitTask` không tồn tại trong bảng KE_SCRIPT_PROTOCOL), khung theo dõi (`tasktrace.ini`) bản gốc bị chú thích 100%. Muốn làm thật nút Bỏ: nối vào `tl_dealtask` (đã có từ đợt bot, hủy miễn phí) — việc mở, chưa làm.
4. **`Selable=1` trong `[TaskList_List]`** là bổ sung bắt buộc của ta (bản gốc dùng tree control riêng). Chép lại ini gốc mà quên dòng này = click chết.
5. Khác biệt trình bày so với bản tham chiếu (cố ý, ghi để khỏi tưởng lỗi): (a) bỏ **lặp tên map lần 2** trong câu loại 4 (bản gốc nối "quyển"+tên map+loại — lỗi trình bày); (b) thêm `<enter>` tách câu tiến độ; (c) tiền tố course tự đặt `[đang làm]`/`[hoàn thành…]` vì hằng `AEXP_TASKDESCWORKING/COMPLETE` không truy ra được định nghĩa gốc; (d) thêm dòng đếm ngày 2420/2797.
6. **Danh mục trái chỉ có Dã Tẩu** — các nhiệm vụ khác của bảng gốc thuộc hệ JX2 không tồn tại ở đây. Thêm mục mới: sửa `Ui\uitasklist.ini` (+ `TaskGuideRes`), rồi thêm nhánh dựng nội dung trong `ShowTask`/`BuildDaTauText`.
7. Bảng đọc `tasklink_*.txt` **từ đĩa client** (KPakFile: đĩa thắng pak). **Đổi bảng phía server thì phải copy sang client**, nếu không chữ hiển thị (tên map/số lượng) sẽ lệch dòng — dữ liệu vẫn đúng vì mọi số tiến độ lấy từ task value, chỉ phần tra bảng theo `1030` là lệch.
8. Ăn theo hạ tầng cũ: id <256 (151 phúc duyên, 747 TK…) đi gói cũ `s2c_taskvalue` — nay gói cũ cũng bắn `GDCNI_TASK_VALUE_UPDATE` để bảng vẽ lại.
9. Phím: F11 đi qua case 8 ("tasknote") của `l_WindowList`; **"NewTask" = chỉ số 30** — thêm cửa sổ mới vào danh sách này phải thêm cả case trong `LuaOpenWindow` VÀ `FindWndWindow` không cần (chỉ Open dùng).

## 6 · Bản đồ mã (file:line theo cây D sau commit `4618a2d0`)

| Việc | Nơi |
|---|---|
| Lớp bảng | `Sources/S3Client/Ui/UiCase/UiTaskGuide.{h,cpp}` (+`UiTaskGuideStr.h` sinh tự động) |
| Gắn F11/F12 | `ShortcutKey.cpp` case 8 + `l_WindowList[30]="NewTask"` + case 30 |
| Đóng/kiểm cửa sổ | `UiShell.cpp` (2 chỗ, cạnh KUiTaskNote) |
| Nhận notify vẽ lại | `GameSpaceChangedNotify.cpp` case `GDCNI_TASK_VALUE_UPDATE` |
| Kênh đồng bộ | `KPlayerTask.cpp::SyncTaskValueToClient` (server) · `KPlayer.cpp` case `UI_TASKVALUE` (client) · `KProtocolProcess.cpp::s2cTaskValueSync` (+notify) |
| Xóa bảng lúc nạp nhân vật | `KPlayerDBFuns.cpp` (chỗ `m_cTask.Release()`) |
| Lua server | `KTongJX2.cpp::LuaJX2_SyncTaskValue` (hết là hàm rỗng) |
| Enum mới (đều nối ĐUÔI) | `KPlayer.h::UI_TASKVALUE` · `CoreShell.h::GDCNI_TASK_VALUE_UPDATE`, `GDI_TASK_SAVE_VALUE`, `GDI_PLAYER_REPUTE_VALUE` |

---

## 7 · ĐỢT 2 (19/08 tối, commit này): màu danh mục + click-đi-Xa-Phu

1. **Màu tên nhiệm vụ**: `[TaskList_List]` bản gốc dùng khóa `Color=` (của tree control cũ) — `KWndMessageListBox` lại đọc `MsgColor`/`MsgBorderColor`, thiếu thì `GetColor("")=0` ⇒ **chữ đen**. Đã thêm `MsgColor=230,230,230` + `MsgBorderColor=9,9,9` vào cả 2 bản ini (chọn = màu vẫn vàng `SelColor` cũ).
2. **Click nhiệm vụ loại 4 → tự chạy đến Xa Phu** (chỉ Địa đồ chỉ / Mật chỉ, đang làm - course 1):
   - Kích hoạt: **bấm vào dòng nội dung** nhiệm vụ ở khung phải, hoặc **nhấp đúp** tên nhiệm vụ ở khung trái. Bảng tự thêm dòng gợi ý màu Cyan khi đủ điều kiện. Bấm lại khi đang chạy = **hủy**.
   - Cơ chế: `GOI_TASKGUIDE_GOTO_XAFU` (OperationRequest, enum nối đuôi) → bộ tick `TG_XaFu*` trong `CoreShell.cpp` (chạy từ `KCoreShell::Breathe`, **client-only, không cần WAuto**) — tái dùng nguyên đồ nghề đã chạy thật của engine Dã Tẩu: `g_MoveStation` (tọa độ bến Xa Phu theo map) → `DT_FindNpcName("xa phu")` → `DT_WalkTo` (tự lên ngựa, path 2,5s/lần) → tới ≤128 mps thì `DialogNpc` mở thoại; người chơi chỉ việc chọn mục "Đến nơi làm nhiệm vụ dã tẩu" (godatau — server chở thẳng map 1031, miễn phí).
   - An toàn: không bật khi auto Dã Tẩu WAuto đang giữ máy (`nDTEngaged`); tự tắt khi chuyển map (= Xa Phu đã chở đi), quá ~36 giây, hoặc thành không có bến; mọi thông báo qua khung chat prefix `[Chỉ nam]` (6 câu, TCVN3 octal).

## 8 · PHÂN TÍCH 3 NÚT Ở BẢN LINUX THAM CHIẾU (Bỏ / Theo dõi / Hủy theo dõi)

Nguồn: `taskguide.lua` (giải nén từ `Patch\data\slistcache.pak`, toàn văn 3.835 B), `protocol.lua` + `protocol_def_c.lua` cùng pak, chuỗi/xref trong `game_y_unpacked.bin`, `taskguide.txt` + `uitasklist.ini`.

### 8.1 Nút "Bỏ nhiệm vụ" (QuitTaskButton) — **CHẾT TOÀN CHUỖI ngay ở bản tham chiếu**
1. Nút sáng/mờ theo cột `Type` của `uitasklist.ini` (chú thích gốc dòng 7: 1=không bỏ được, 2=bỏ được) — Dã Tẩu `[7] Type=1` ⇒ mờ.
2. Bấm → C++ gọi Lua `taskguide_quittask(nId)` (chuỗi `taskguide_quittask` @VA `0x006A560C`, xref `0x00544F00`).
3. Lua (`taskguide.lua`): `OB_Create()` → `ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nId)` → `ScriptProtocol:SendData("emSCRIPT_PROTOCAL_TaskGuide_QuitTask", handle)` → `OB_Release`.
4. `SendData` tra `self[szEnum]` trong bảng `KE_SCRIPT_PROTOCOL` (33 mục, `protocol.lua`) — **mục này không tồn tại** (chú ý chính tả gốc `PROTOCAL`) ⇒ `type(...) ~= "number"` ⇒ **không gửi gì, im lặng**.
5. Phía máy chủ cũng không có người nhận (`grep TaskGuide_QuitTask` toàn cây `server1` = 0; thư mục `script_protocol` chỉ còn `protocol_def_gs.lua` + `protocol_def_c.lua--` đã vô hiệu).
⇒ Thế hệ trước (JX2 gốc TQ) hẳn có enum này; bản VN 2021 cắt enum khỏi `protocol.lua` nhưng giữ nguyên UI + Lua ⇒ nút thành vỏ rỗng. **Dự án ta để nút mờ là trung thực**; muốn làm thật thì nối sang `tl_dealtask` (hủy miễn phí có sẵn từ đợt bot) — không đi đường ScriptProtocol.

### 8.2 Nút "Theo dõi nhiệm vụ" (TraceButton) — hạ tầng ĐỦ nhưng bị TẮT bằng dữ liệu
Chuỗi đầy đủ ở bản gốc:
1. Nút sáng theo cột `Trace` của `uitasklist.ini` (1=theo dõi được, 2=không) — Dã Tẩu `[7] Trace=2` ⇒ **mờ ngay bản gốc**; chữ trạng thái đổi theo khóa `TraceText`/`NotTraceText` của `[Main]` ("Đã theo dõi"/"Chưa theo dõi").
2. Bấm → C++ gọi Lua `tasktrace_add(nId, nClass, nTraceType)` (khối chuỗi `tasktrace_add`+`Trace`+`TaskId` @`0x006A55EC`) → `DynamicExecute(tracescript, TraceTaskFunc, nId, nClass, nTraceType)` — chạy hàm CỦA TỪNG nhiệm vụ khai trong `taskguide.txt` cột 9 (vd ID 5 `tb150skillTask:TraceTask`, ID 14 `tbGuideTask:ShowTrace`). Hàm này gọi API C++ (`TraceTask`, `TraceTask_TextOut`) để đăng ký nhiệm vụ vào cửa sổ **KUiTaskTrace** — khung nhỏ 180×220 treo mép phải màn hình, cấu hình `uitaskguide\tasktrace.ini` (`MaxShow=8`), RTTI `.?AVKUiTaskTrace@@` @`0x006E92B0`.
3. Cập nhật sống: server đẩy task value → notify 84 → `playertaskchange(id, value)` → nếu id rơi vào `tbTaskVariable[2]` (chỉ khai cho **ID 5**: 2885 và **ID 14**: 4078, 1-10, 4001-4008) → `TraceTask_Update()` → C++ duyệt danh sách đang theo dõi → gọi lại Lua `tasktrace_udpate(nId)` (bản gốc gõ sai chính tả, binary lẫn lua khớp nhau) → `DynamicExecute(ShowReducedInfoFunc)` in bản RÚT GỌN vào khung. Có thêm `onridestatechange()` → cập nhật riêng ID 14.
4. Bấm vào một dòng trong khung theo dõi → C++ gọi `tasktrace_select(nId)` → `ClickTraceItemFunc` (vd `tbGuideTask:ClickTraceItem` — mở lại bảng chỉ nam đúng nhiệm vụ / dẫn đường).
5. **Vì sao bản tham chiếu không thấy gì:** (a) `tasktrace.ini` bị chú thích 100% ⇒ `Ini.Load` vẫn OK nhưng mọi control kích thước 0 ⇒ khung **vô hình** (đã kiểm nhánh mã: không bail, không treo); (b) dữ liệu chỉ cho phép ID 5/14 theo dõi (ID 14 thiếu cột `TaskTraceScriptFile` nhưng `load_data` có luật dự phòng `tracescript = guidescript`); (c) Dã Tẩu không khai hàm trace nào (5/9 cột) ⇒ `tasktrace_add/udpate/select` đều `return` sớm.
⇒ Muốn "Theo dõi" chạy thật cho Dã Tẩu phải: bỏ chú thích `tasktrace.ini` + dựng lớp `KUiTaskTrace` + viết hàm rút gọn cho Dã Tẩu — bản gốc chưa từng hỗ trợ Dã Tẩu ở khung này.

### 8.3 Nút "Hủy theo dõi" (CancelTraceButton) — thao tác THUẦN C++
Không tồn tại hàm Lua "tasktrace_remove" nào (đã rà toàn bộ `taskguide.lua` + bảng chuỗi binary) ⇒ bấm Hủy là C++ tự gỡ nhiệm vụ khỏi danh sách theo dõi nội bộ của `KUiTaskTrace` rồi vẽ lại khung + đổi chữ về `NotTraceText`. (Binary có thêm API Lua `SwitchTaskTrace` để script bật/tắt cả khung — nút Cancel không đi qua đường này.) *Mức tin: suy từ sự vắng mặt chuỗi Lua tương ứng + đủ bộ API C++; chưa dò asm riêng nút này.*

---

## 9 · ĐỢT 3 (19/08 khuya): bỏ Zalo/Facebook + 3 nút SỐNG THẬT + tự bấm mục Xa Phu

Deploy 21:30 — `Game.exe` 1.251.840 B (`Game_cu_2105.exe`), `CoreClient.dll` 2.184.192 B (`CoreClient_cu_2105.dll`), **`bin\server\CoreServer.dll` 17.859.072 B (`CoreServer_cu_1910.dll`)**. `re_pe_crt` PASS.

1. **Bỏ 2 nút Zalo/Facebook** góc phải: `UiPlayerBar.cpp` — comment 2 dòng `AddChild(&m_Zalo/&m_Fb)` (nút vẫn Init đọc ini nhưng không vào cây cửa sổ ⇒ không vẽ, không ăn click). Muốn bật lại: bỏ comment.
2. **Nút "Bỏ nhiệm vụ" SỐNG**: client gửi `UI_CMD nType=6, szFunc="tg_quit"` (`GOI_ADD_UI_CMD_SCRIPT`) → server `UiCommandScript` case 6 (đặt GIỮA case 4 và case 5 — case 5 vốn RƠI xuống default, đừng chen sau nó) → `ExecuteScript(seasonnpc.lua, "tg_quit")` → `tg_quit` (chèn CUỐI seasonnpc.lua server sống; snippet lưu `TaskGuideRes\script\`) đặt `1045=1` rồi gọi **`Task_CancelConfirm()`** ⇒ hiện đúng hộp xác nhận hủy CHUẨN của hệ (đủ luật: trừ lượt hủy, phạt hủy lậu >2 lần đầu chuỗi, nhánh 100 mảnh SHXT, tăng 2797). Nút chỉ sáng khi course==1. **⚠️ Cần RESTART GameServer** (bin\server đã có DLL mới nhưng tiến trình đang chạy giữ bản 19:10 — tôi không tự restart).
3. **Nút "Theo dõi / Hủy theo dõi" SỐNG**: lớp mới `KUiTaskTrace` (`UiTaskTrace.{h,cpp}`) — khung 180×220 mép phải (ảnh `任务追踪底板.spr` sẵn trong pak), nạp `uitaskguide\tasktrace.ini` **bản sống** (sinh lại từ bản comment tham chiếu + `MaxMsgCount=8`; đã deploy + `TaskGuideRes`). Hiện 2 dòng: tên (vàng) + bản rút gọn (`KUiTaskGuide::BuildBriefLine`: loại 4 = "Địa đồ chỉ: X/Y tấm", loại 6 = "Mảnh SHXT: X/Y", course 2/3 = "Về gặp Dã Tẩu trả nhiệm vụ!"…). Tự vẽ lại theo `GDCNI_TASK_VALUE_UPDATE`; **bấm vào dòng = mở bảng chính** (đúng vai `ClickTraceItemFunc` bản gốc); trạng thái theo dõi sống theo phiên, tắt khi logout (`UiShell` close-all).
4. **Tự bấm mục Xa Phu**: `TG_XaFu` lên 2 pha — pha 2 sau `DialogNpc` theo dõi `g_sDTCap.uDlgSeq`, thoại về thì `DT_Split` + `DT_FindAns(DTM_OPT_GODATAU)` + `DT_Answer` ⇒ **tự chọn "Đến nơi làm nhiệm vụ dã tẩu", người chơi không phải bấm gì** (đúng yêu cầu); 3,2s không có thoại thì gõ lại NPC, thoại không có mục thì báo chọn tay; bị kéo ra xa khi chờ thì quay lại pha đi. (Hook `g_sDTCap` nằm trong OnScriptAction — chạy vô điều kiện, không cần WAuto.)

Cạm bẫy mới ghi nhận: `DT_Answer` định nghĩa SAU khối TG trong `CoreShell.cpp` — đã thêm forward declaration; thêm hàm mới vào vùng đó nhớ khai báo trước.

## 10 · ĐỢT 4 (19/08 khuya): nút icon mép phải + hạ khung theo dõi khỏi minimap

- **`KUiTaskTraceIcon`** (trong `UiTaskTrace.{h,cpp}`): nút 25×25 dùng `opentracebtn.spr` của bộ tài nguyên tham chiếu (có sẵn trong `updatejx14.pak`, 2 frame Up/Down) — **luôn hiện khi vào game** (mở cùng HUD tại `UiShell.cpp` cạnh `KUiHeaderControlBar::OpenWindow`), neo `Wnd_GetScreenSize` mép phải ~2/5 chiều cao. Bấm = bật/tắt khung theo dõi (đồng bộ lại 2 nút của bảng chính qua `KUiTaskGuide::RefreshButtons`).
- **Khung theo dõi hết bị minimap che**: `KUiTaskTrace::Initialize` tự neo `(nSW - W - 2, nSH*2/5)` theo màn hình thật (ini `Top=300` chỉ còn là dự phòng 1024). Khung vẫn kéo thả được (`Moveable=1`).
- Section mới trong `tasktrace.ini`: `[OpenIcon]` + `[OpenIconBtn]` (sinh tự động, đường ảnh GBK lấy từ dòng Image mẫu).
- Deploy 21:44: `Game.exe` 1.253.376 B (bản cũ `Game_cu_2130.exe`), re_pe_crt PASS. CoreClient/CoreServer không đổi so với 21:30.

## 11 · ĐỢT 5 (19/08 khuya): nút theo dõi vào THANH PlayerBar — đúng chỗ Zalo cũ 100%

Đợt 4/4b đặt nút bằng cửa sổ nổi + `Wnd_GetScreenSize` — vị trí vẫn lệch so với Zalo cũ (Zalo là CHILD của PlayerBar, neo `1024-30` trong hệ tọa độ thanh). Sửa dứt điểm bằng cách **đi đúng con đường cũ**:
- `KUiPlayerBar` thêm `m_TraceBtn` (section **`[TraceBtn]`** mới trong `UiPlayerBar.ini` — Left/Top y hệt `[Zalo]` 765/185, ảnh `opentracebtn.spr`, Tip "Theo dõi nhiệm vụ"); Init cạnh Zalo, `AddChild` chỗ Zalo cũ, neo `SetPosition(1024-30, nY)` y hệt khối Zalo/Fb; click → `KUiTaskTrace::SetTraced(!IsTraced())`.
- Khung theo dõi mở **ngang hàng nút** qua `KUiPlayerBar::GetTraceBtnPos` (vị trí thanh + vị trí nút = tuyệt đối).
- Lớp `KUiTaskTraceIcon` (đợt 4) đã **xóa**; UiShell không còn mở/đóng icon riêng.
- `UiPlayerBar.ini` bản đã sửa lưu git tại `TaskGuideRes\Ui\Ui3\UiPlayerBar.ini`.
- Deploy 21:59: `Game.exe` 1.252.352 B (bản cũ `Game_cu_2150.exe`), re_pe_crt PASS.
