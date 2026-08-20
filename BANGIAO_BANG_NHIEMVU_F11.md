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
