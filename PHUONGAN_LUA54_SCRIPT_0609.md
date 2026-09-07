# PHƯƠNG ÁN ÁP DỤNG LUA 5.4 CHO SCRIPT VÀ HÀM C++ GẮN LUA (06/09/2026 19:30)

Chủ: *"tôi thấy có nhiều script có thể áp dụng lua 5.4 mới, kiểm tra kỹ hơn rồi phân tích phương án"* và *"các giới hạn ở lua cũ giờ fix hàm ở mã nguồn áp dụng lua 5.4 luôn được không? liệt kê các hàm có thể áp dụng lua mới và thay đổi có thể cải thiện được gì?"*

Công cụ: `ReverseTools\lua54\danhgia_0609\ptich_54.py` (tách token toàn cây, theo ngữ cảnh vòng lặp/hàm, chỉ mục tham chiếu chéo giữa 1.772 tệp + chuỗi trong C++ + settings). Báo cáo thô: `PTICH_54_baocao.md`, số đo: `bench_54.lua` (mục 14 `PHANTICH_NANG_LUA54_0509.md`).

## 1. Kết quả kiểm tra kỹ phía SCRIPT (1.772 tệp, 323.936 dòng)

| Mẫu viết cũ | Số chỗ | Tính năng 5.4 thay thế | Lợi đo được | Ghi chú |
|---|---:|---|---|---|
| `x = x .. y` **trong vòng lặp** | **116** (39 tệp) | `table.concat` | 23× ở 20.000 phần tử, O(n) thay O(n²) | đáng nhất ở `auction_manager` (618, 663, 665), `mailmanager` (667, 669), `leaguematch/officer.lua` (6 chỗ), `lib_ham.lua` (7), `lib/say.lua` jx2 (6); 25 chỗ trong `kiemthu/` (test) bỏ qua |
| `while … getn(t)` | **11** | `#t` | 25× (10 pt) → O(n²) thành O(n) | `auction_manager.lua` 105/177/1210, `seasonnpc.lua` 1255/1269 (Dã Tẩu), còn lại trong kiemthu |
| vòng lặp `strfind` tách chuỗi thủ công | **84** | `string.gmatch` / `gsub` | gọn 5–10 dòng → 1 dòng, tốc độ tương đương | `functionlib.lua` 5, `debug.lua` 6, `mailmanager` 4, `say.lua` 3, `auction_manager` 2 |
| `dostring(…)` biên dịch mỗi lần gọi | 14 | `load` một lần + cache | tránh biên dịch lặp | `functionlib.lua` 4 (OptionFunction đã vá số), `debug.lua` 2 |
| `call(f, {…}, "x")` | 49 | `pcall` / `xpcall` | 22×, có traceback | `auction_manager` 4, `timerserver` 3, `functionlib` 2 |
| `floor(a/b)` | 394 | `a // b` | 4,4× | rải rác, không chỗ nào nóng → chỉ cho code mới |
| tag method | 11 | metatable | – | lib JX2 `baseclass.lua`, đã chạy qua shim |
| **hàm toàn cục chỉ dùng trong chính tệp** (không engine/C++/settings/gọi theo tên chuỗi) | **1.588 / 8.680** | `local function` | gọi 1,3×; **hết va chạm tên giữa các tệp Include cùng state**; bảng toàn cục nhỏ hơn | top: `auction_manager` 53/70, `tong_mix.lua` 44/64, `lenhbaitanthu` 29/74, `battlehead` 26/43, `skills_table` 24/29, `mailmanager` 18/31, `setmeridian` 18/19 |
| **biến toàn cục cấp tệp chỉ dùng nội bộ** | **1.915** | `local` (upvalue) | đọc 2,9×; hết rò trạng thái tạm (đa số là biến tạm hội thoại) | `tong_mix` 57, `battlehead` 49, `test_phiphong_admin` 17 |
| hàm > 200 dòng | 40 | tách hàm | dễ hiểu | `skills_table.lua:1260` (258 dòng), `tasklink_award.lua:12` (267), `npclevelscript/animal*.lua` OnCreate 231–274 dòng |
| coroutine cho hội thoại NPC nhiều bước | 1.350 `Say(` | `SayWait` bằng coroutine (cần C++ hỗ trợ, mục 2.7) | code NPC ngắn 3–5×, không cần biến tạm toàn cục | chậm hơn 100× mỗi resume nhưng chỉ vài resume/hội thoại → không đáng kể |

Rủi ro khi `local` hoá tự động (đã trừ khỏi 1.588): hàm engine gọi theo tên (`main`, `On*`, 20 tên trong C++ `ExecuteScript`), tên xuất hiện trong chuỗi cùng tệp (callback `Say("…/HamX")`), tên trong `settings`; **còn lại**: 18 `getglobal(`, 3 `setglobal(`, 15 `dostring`, 9 `load(` gọi theo tên động → tệp có các hàm này để rà tay. `local function` phải khai báo trước khi dùng → bộ chuyển phải sắp thứ tự hoặc dùng khai báo trước (`local A, B` rồi `function A() … end`).

## 2. Phía C++ — giới hạn Lua cũ còn "kẹt" trong mã nguồn và hàm có thể nâng

Kiểm kê: 566 hàm `Lua*` trong `ScriptFuns.cpp` (bảng 752 mục), 53 `CallFunction`, 161 `ExecuteScript/2`, `Lua_PushNumber` 1.523, `Lua_ValueToNumber` 1.310, `Lua_IsNumber` chỉ 376 → **331 hàm đọc đối số mà không kiểm kiểu** (sai kiểu → thành 0 im lặng, đúng hành vi Lua 4).

| # | Hàm / chỗ trong mã nguồn | Giới hạn Lua cũ đang mang | Sửa theo 5.4 | Cải thiện | Công |
|---|---|---|---|---|---|
| C1 | `KLuaScript::LoadBuffer` → `Lua_CompileBuffer(…, NULL)` | tên chunk `NULL` → mọi lỗi runtime ghi `[string "?"]:84:` không biết tệp nào | truyền `m_szScriptName` làm chunkname | **mọi ScriptError có `tệp:dòng`** thật (hiện phải đoán qua dòng "ScriptError 4 (…)" đi kèm) | 1 dòng, Engine |
| C2 | `KLuaScript::ExecuteCode` `if (state = Lua_Execute(...) != 0)` | ưu tiên toán tử → mã lỗi luôn `[1]` | tách 2 câu | log ghi đúng ERRRUN/ERRFILE/ERRSYNTAX | 1 dòng, Engine |
| C3 | `KLuaScript::CallFunction` định dạng `n d s N f v p`; `'t'` rỗng | không truyền được boolean, số nguyên 64 bit, bảng | thêm `'b'` (`lua_pushboolean`), `'i'` (`lua_pushinteger`), `'t'` (bảng qua hàm dựng) ; `GetValuesFromStack` đọc `'b'`/`'t'` | C++ ↔ Lua truyền dữ liệu có cấu trúc (thông tin vật phẩm, kết quả nhiều trường) thay vì 6–8 số rời | nửa ngày, Engine |
| C4 | 331 hàm `ScriptFuns` đọc `Lua_ValueToNumber/String` không kiểm | đối số sai kiểu thành 0/"" im lặng | (a) DLL: cờ `LUA54_KIEM_KIEU=1` → `lua4_tonumber/tostring` ghi cảnh báo 1 lần/(tệp,hàm) khi ép kiểu sai — **không sửa 331 hàm**; (b) macro `Lua_CheckNumber(L,i)` = `luaL_checknumber` dùng dần cho hàm mới/quan trọng | bắt lỗi script tiềm ẩn (gọi hàm thiếu đối số, truyền chuỗi thay số) | (a) 1 giờ DLL; (b) dần |
| C5 | `Lua_PushNumber(L, (double)n)` 1.523 chỗ | Lua 4 chỉ có double | **đã xong**: `lua4_pushnumber` đẩy `lua_pushinteger` khi số tròn (< 2^53) → script nhận số nguyên 64 bit, `%d` không tràn | không cần sửa | 0 |
| C6 | `LuaIncludeFile` chạy lại thân tệp mỗi Include (30.468 lần lặp trong 1 lần boot) | Lua 4 không có `require` | `IncludeOnce`: đánh dấu đã chạy theo (state, tệp); bật theo tệp bằng dòng đầu `-- @IncludeOnce` (an toàn) hoặc toàn cục | boot 2,98 s → ước < 1,5 s; bớt rác GC; hết reset bảng ngoài ý muốn (`X = {}` ở đầu lib) | 2 giờ DLL + kiểm parity `boot_gia` |
| C7 | Hội thoại NPC: `Say()` → callback theo tên hàm toàn cục | Lua 4 không có coroutine → mỗi bước một hàm + biến tạm toàn cục (1.915 biến) | `SayWait(text, {lựa chọn})` trong Lua = `coroutine.yield`; C++ khi người chơi chọn → `coroutine.resume` (5.4 cho phép yield xuyên C qua `lua_yieldk`/`lua_callk`); bảng coroutine theo `nPlayerIndex`, dọn khi thoát/timeout | script NPC tuyến tính, ngắn 3–5×, không rò trạng thái; viết nhiệm vụ mới nhanh hơn | 2–3 ngày (thử 1 NPC trước), Core + lib |
| C8 | Mỗi `.lua` = 1 `lua_State` (1.733 state, 127 KB/state, thư viện chép 30.000 lần) | Lua 4 không có `_ENV` | **một state + `_ENV` riêng từng tệp**: Include = `load` với env của tệp gọi; `g_GetScriptNameByState` → tra env; thư viện dùng chung nạp một lần; `require` khả dụng | RAM Lua ≈ 0,6 GB → ~20 MB; boot < 1 s; nạp nóng đơn giản; chia sẻ dữ liệu giữa tệp không cần Include | 1–2 tuần, rủi ro trung bình–cao (cách ly globals, 742 hàm gắn dùng `L` chung, client `MAX_SCRIPT_IN_SET=5`) |
| C9 | 30 chỗ `Lua_NewTable` + `Lua_SetTable` trả danh sách | – | `lua_createtable(n, 0)` định trước cỡ | vi mô | không đáng |
| C10 | `Lua_GetGlobal(name)` + `lua4_call` mỗi `CallFunction` | – | `luaL_ref` cache hàm nóng (OnTimer NPC) | vi mô (~50 ns) | không đáng |
| C11 | Không có đo theo hàm script | – | **Profiler lấy mẫu** bằng `lua_sethook(LUA_MASKCOUNT)` trong DLL khi `lua4_perf_set(1)`: mỗi 1.000 lệnh ghi `tệp:dòng` đang chạy vào histogram; Core lệnh GM / mỗi 10 phút ghi top-50 ra `logs/lua_prof.log` | biết chính xác script nào tốn 0,25–0,45 ms/tick và đỉnh 67 ms → tối ưu có địa chỉ | 1 ngày DLL + Core |
| C12 | `MAX_SCRIPT_IN_SET` 5000 (server) / 5 (client) | mảng tĩnh | giữ; client nạp lười 5 state | – | 0 |

Trả lời thẳng câu "fix hàm ở mã nguồn áp dụng 5.4 luôn được không?": **được, từng phần và không phá lớp tương thích**: C1, C2, C4(a), C6, C11 là những thay đổi nhỏ trong Engine/DLL không đụng 566 hàm gắn và không đổi hành vi script; C3, C7 mở khả năng mới cho script viết sau; C8 là dự án kiến trúc riêng. **Không nên** đổi kiểu trả về của 566 hàm sang boolean (1.752 chỗ script so `== 1`) hay đổi ngữ nghĩa `getn/format` (mục 12).

## 3. Phương án (chọn một hoặc xếp chồng)

| PA | Nội dung | Công | Được | Rủi ro / kiểm |
|---|---|---|---|---|
| **PA-0 Đo trước** | C11 profiler + C1/C2 (tên tệp trong lỗi, mã lỗi đúng) | 1 ngày | biết đúng script nóng; mọi lỗi có `tệp:dòng` | rất thấp; 1 lần build DLL+Engine+Core, `.moi` |
| **PA-1 Sửa đích danh** | 11 `while getn` → `#t`; ~25 `x = x ..` dài (auction, mail, leaguematch, lib_ham, say) → `table.concat`; 14 `dostring` → `load` cache; 49 `call` → `pcall`; 84 vòng `strfind` → `gmatch` khi chạm | 1 buổi | hết O(n²) ở đấu giá/thư/Dã Tẩu; lỗi có traceback | thấp: `kiem_54.py` + `boot_gia` parity từng tệp; sửa nóng được ở tệp trong chuỗi timerserver |
| **PA-2 `local` hoá tự động** | bộ chuyển: 1.588 hàm + 1.915 biến chỉ dùng nội bộ → `local` (khai báo trước, giữ tên engine/chuỗi/C++), bỏ qua tệp có `getglobal/dostring/load` (≈40 tệp) ; chạy theo thư mục, mỗi đợt parity `boot_gia` + kiểm 5.4 | 2–3 ngày | gọi 1,3×, đọc 2,9×; **hết va chạm tên giữa Include** (lớp lỗi có thật); bảng toàn cục nhỏ | trung bình: lỗi thứ tự khai báo (bắt được khi biên dịch/chạy thử), hàm gọi theo tên động sót (bắt bằng `LUA54_KIEM_KIEU` + log `attempt to call nil`) → làm theo đợt, có lùi git |
| **PA-3 C++ nhỏ chắc ăn** | C3 (`'b' 'i' 't'`), C4(a) cờ kiểm kiểu, C6 `IncludeOnce` theo tệp | 1 ngày | truyền dữ liệu có cấu trúc, bắt lỗi kiểu, boot nhanh thêm | thấp–trung bình (C6 cần parity) |
| **PA-4 Hội thoại coroutine** | C7 `SayWait` + lib `hoithoai.lua`; thử 1 NPC (vd Dã Tẩu `seasonnpc.lua` 53 hàm) rồi mở rộng cho NPC mới | 2–3 ngày | viết NPC nhanh, không biến tạm toàn cục | trung bình: timeout/thoát giữa hội thoại, GC coroutine treo |
| **PA-5 Một state + `_ENV`** | C8 | 1–2 tuần | RAM −0,5 GB, boot < 1 s, `require` | cao; cần môi trường thử riêng và bộ so hành vi (boot_gia mở rộng + test chức năng) |

**Đề xuất thứ tự:** PA-0 → PA-1 → PA-3 (một tuần rải) ; PA-2 làm theo thư mục sau khi profiler chỉ tệp nóng; PA-4 thử một NPC; PA-5 để sau khi các phiên khác xong vận tiêu/bot (đụng toàn engine).

## 4. Cần chủ chốt
1. Duyệt PA-0 + PA-1 + PA-3 làm ngay (chỉ cần 1 lần restart cho DLL/Engine/Core `.moi`)?
2. PA-2 `local` hoá tự động: làm toàn cây (2–3 ngày, theo đợt) hay chỉ 10 tệp đầu bảng (auction, tong_mix, lenhbaitanthu, battlehead, skills_table, mailmanager, setmeridian…)?
3. PA-4 coroutine: thử với NPC nào (Dã Tẩu `seasonnpc.lua` hay một NPC mới của vận tiêu)?
4. PA-5 một state: đưa vào kế hoạch tuần sau hay để hẳn?


## 5. ĐÃ THI CÔNG (06/09 tối, chủ: "ok hãy làm toàn bộ xong thì phản biện") — chờ swap `.moi`

| PA | Đã làm | Kiểm | Ghi chú |
|---|---|---|---|
| **PA-0** | C11 profiler lấy mẫu trong Lua54Dll (`lua4_prof_set/write`, hook đếm lệnh; máy chủ bật 2000 lệnh/mẫu qua KPerfTick, `LUA54_PROF=0` tắt) → `jx_lua_prof.log` mỗi 10 phút (top 40 dòng + top 20 tệp, xoá sau mỗi kỳ); C1 chunkname `@\script\x.lua` → lỗi runtime ghi `\script\x.lua:12:`; C2 mã lỗi ExecuteCode đúng | selftest, boot mô phỏng | client không lấy mẫu |
| **PA-1** | `lib_ham.lua` SayEx bỏ dostring (gọi Say thẳng), TaoBang/TaoBangNew bộ đệm `table.concat`; `common.lua` join; `mailmanager.lua` 664; `timerserver.lua` TS_GhiProf; `functionlib.lua` OptionFunction so sánh trực tiếp + `lib:ChayChuoi` cache chunk theo chuỗi (ParseAward/NumberParamTrans/StringParamTrans); `task_string.lua` TalkEx gọi Talk thẳng; `onkillnpc.lua` xpcall; `dailogsay.lua` gọi thẳng | kiem_54 0 lỗi; boot mô phỏng GIỐNG HỆT | **Đính chính:** 11 chỗ `while getn` KHÔNG phải O(n²): bảng đều dựng bằng `tinsert` (có `t.n`, `lua4_getn` O(1)) → không sửa |
| **PA-2** | `local_hoa2.py`: **193 tệp / 571 hàm** chỉ dùng trong tệp → `local` (dòng đầu tệp `-- [LOCAL54 ...]` + `local a, b, ...`). CHỈ hàm; loại: thư mục lib/activitysys/scriptjx2/script_protocol/kiemthu/cauhinh/header/bando/cauhinh_web, tệp gọi tên động, tên trong chuỗi/chú thích, tệp khác nhắc, không ai gọi (251), đọc trước định nghĩa (24), bị gán (2), điều kiện cấp 0 | kiem_54 0 lỗi; boot: lỗi giống hệt (chỉ lệch số dòng), `so_globals.py`: 0 khác thật (2.581 state; 428 mất đúng tên đã local của tệp Include) | **Lần 1 (1.326 tên gồm biến) đã LÙI**: biến chặn nạp lặp `WLLS_HEAD` (`if not WLLS_HEAD then WLLS_HEAD = 1`) thành local → mỗi Include lại nil → Include đệ quy vô hạn (treo 20 phút, không tới hook 20M lệnh vì mỗi tầng chỉ vài lệnh rồi vào C) |
| **PA-3** | C3 `CallFunction` 'b' boolean, 'i' int64, 't' bảng qua `KLuaTableFill(L, idx, ctx)`; `GetValuesFromStack` 'b'; C4 `LUA54_KIEM_KIEU=1` → `logs\lua_kieu.log` một lần/chỗ; C6 `@IncludeOnce` (Lua54Dll bỏ chạy lại thân tệp; `LUA54_INCLUDE_ONCE=0` tắt) đánh dấu **43 tệp toàn hàm/hằng** (`chon_includeonce.py`: an toàn đệ quy Include, tên không định nghĩa ở tệp khác) | boot mô phỏng: bỏ qua **760** lần Include | lib lớn (`lib_task` 600 lượt, `log` 317, `lib_ham` 214) có gán cấp tệp → không chứng minh được, không đánh dấu → lợi boot nhỏ |
| **PA-4** | `lib\hoithoai_54.lua`: `HoiThoai(f)`, `SayWait(text, {lựa chọn})` → chỉ số, `TalkWait({câu})`; callback `HT_Chon1..12` trong state script; `lib\lib_54.lua` (Thu/GhepChuoi/ChiaChuoi/Bit_*, @IncludeOnce). Thí điểm: **lệnh bài admin → "Thu nghiem Lua 5.4: SayWait coroutine, lib_54"** (`kiemthu\item\test_lua54_admin.lua`: hội thoại 3 bước trong một hàm, TalkWait, tiện ích, thông tin phiên bản). Core: `g_GetScriptNameByState` tra chủ coroutine qua `lua4_owner` | chưa test trong game | KHÔNG đổi NPC đang chạy (laodau.lua có timer/Talk, hiếm dùng) — chủ thử qua lệnh bài trước |
| **PA-5** | Lua54Dll chế độ **MỘT STATE** `LUA54_MOT_STATE=1` (mặc định TẮT): master + mỗi script một thread + bảng E (`__index` → bảng chủ, `E._G = E`); hàm C đăng ký vào bảng chủ một lần (trùng bỏ, khác → ghi đè trong E); chunk gắn `_ENV = E`; coroutine suy E từ upvalue `_ENV` (cache khoá yếu); shim getglobal/setglobal/globals/dostring/dofile/load/loadfile/call theo môi trường script (`L4_Env`); cache Include theo E; `lua4_owner/master` | selftest 0 lỗi; boot mô phỏng toàn cây: **lỗi giống hệt classic, 2,07 s (classic 3,5 s; tạo state 0,08 s thay 1,06 s)** | CHƯA chạy máy chủ thật; thử ở phiên vắng: thêm `set LUA54_MOT_STATE=1` trước lệnh chạy GameServer trong ChayGameServer.bat, theo dõi ScriptError + RAM |

**Nhị phân (build từ origin/main 642eeaef, worktree `D:\GAMEDEVNEW_wt_lua54b`, `build_lua54c.bat` = Lua54Dll x64+Win32, Engine Server Release x64, Core Server Release x64):**

| tệp | md5 | kích thước |
|---|---|---|
| `bin\server\Lua54Dll.dll.moi` | 889d92e7 | 587.776 |
| `bin\server\engine.dll.moi` | 6457bc53 | 635.392 (có `strncpy` KLuaScript::Load) |
| `bin\server\CoreServer.dll.moi` | 7e41f1b7 | 18.461.696 (gồm mọi commit phiên khác đến 642eeaef) |
| `bin\client\Lua54Dll.dll.moi` | eb4aa618 | 512.000 |

Sau swap kiểm console: `[script] LoadAllScript ... `, `[script] IncludeOnce bo qua N lan; che do mot state: tat`, `[script] Bi danh duong dan: 2541 dong, dang ky ID cu 2541`; 10 phút sau có `jx_lua_prof.log`; lệnh bài → "Thu nghiem Lua 5.4".

**Sự cố cùng buổi (đã sửa):** (1) mất trap các map do 1.369 tệp Hán bị xếp nhầm "chết" (SAPXEP mục 9); (2) GameServer sập lúc boot 19:34 vì tên `bando\` dài ≥ 100 (SAPXEP mục 10) — máy chủ tắt 19:34–20:45.


**Cập nhật 20:57:** chủ đã swap cả 4 `.moi` (server 20:52: CoreServer 7e41f1b7, engine 6457bc53, Lua54Dll 889d92e7; client Lua54Dll eb4aa618), GameServer boot 20:57 với 1.000 bot, phiên BOTNOI xác nhận đủ nhãn của họ; phút đầu không có ScriptError mới, LUA_CALL 0,20 ms/tick, TICK 7,28 ms. `jx_lua_prof.log` xuất hiện sau 10 phút.


**Profiler ky dau (21:07, 655 s, 9.469 mau, moi 2.000 lenh; LUA_CALL 0,20 ms/tick = 2,7 % tick 7,28 ms):** duong "giet quai" chiem ~40 % thoi gian Lua:
`G_CFG` (cauhinh/ch_lib.lua:34-43) 8,9 % + `SKD_CFG` (lib/lib_sukien.lua:24-25) 7,3 % + `DropRate` (global/luanpcmonsters/droprate_normal.lua) ~7 % + `OnPlayerKillNpc` (onkillnpc.lua:34-35) 4 % + `CL_CongNhom` (cl_def.lua:43) 2 %.
Moi lan giet quai, DropRate goi SKD_CFG -> G_CFG -> CFGW_Get khoang 10 khoa DRQ_* rieng le. Ngoai ra: `tl_getMapInfo` quet tuyen tinh TL_MAPTRAPINDEX (map_index.lua:226-227) 2,6 %, `ScriptProtocol:_InitProtocolEnum` (protocol.lua:114-115) 2,5 % duoc goi lap, `Task_AwardLink` quet tuyen tinh (tasklink_award.lua:579-580) 1,8 %.
De xuat sua dich danh (chua lam, cho chu chot): (1) SKD_CFG/G_CFG cache gia tri theo khoa voi TTL vai giay hoac bo dem phien ban CFGW -> bo ~15 % thoi gian Lua; (2) TL_MAPTRAPINDEX va Task_AwardLink -> bang bam theo id; (3) _InitProtocolEnum chi chay mot lan. Tong loi toi da ~0,05 ms/tick.
**Loi da sua 21:03:** nhan menu lenh bai chua dau '/' ("Thu / GhepChuoi / ChiaChuoi / Bit") -> engine cat ten ham tai dau '/' dau tien -> "attempt to call a nil value"; doi nhan dung dau phay, SayWait/TalkWait tu thay '/' trong nhan bang '|'. Khong co ScriptError nao khac tu 20:57.


**Loi 21:18 + sua goc 21:29 (`CoreServer.dll.moi` 34a3fdd5 cho swap, thay 31f1537c cua BOTNOI, gom ca dot 5):** chu chon menu lenh bai trong map Tong Kim -> callback lua chon (SayEx menu, SayWait) roi vao state `tong_kim_tcap\kimtu.lua` (ScriptError: `Script Name kimtu.lua cFuncName L54_MenuThu ... attempt to call a nil value`). Goc engine: `ProcessPlayerSelectFromUI` goi `ExecuteScript(Npc[m_nIndex].m_ActionScriptID, ten ham)` = script chay GAN NHAT cho nguoi choi voi bGlobal; trap/timer/kimtu chay xen khi hop thoai dang mo -> ghi de. Sua: `KPlayer::m_dwDialogScriptID` ghi tai LuaSaySPR/LuaSay/SelectUI/LuaTalkUI (`sDialogScriptId(L)` = g_FileName2Id(g_GetScriptNameByState(L)), coroutine tra ve chu), dispatch dung no (fallback m_ActionScriptID neu 0/khong ton tai). Ap dung cho MOI hop thoai NPC, khong chi SayWait. Cach tam truoc swap: thu menu ngoai map su kien.

## 6. PHẢN BIỆN (tự soát, 06/09 tối)

1. **Hai sự cố sản xuất hôm nay là do tôi**, cùng một gốc: tin phân tích tĩnh hơn giới hạn thật của engine. (a) Xoá 1.369 tệp dù memory 30/08 đã cảnh báo pak là đường tham chiếu; (b) đặt tên dài không kiểm `m_szScriptName[100]`. Bộ mô phỏng boot không mô phỏng bộ đệm C++ nên không bắt được (b). Phòng ngừa đã làm: luật < 96 ký tự, `kiem_trap_map.py` quét pak, memory LUẬT. Việc còn thiếu: **đo động** (ghi script nào được `g_GetScript` chạm trong 1–2 tuần) trước khi dọn bất cứ gì.
2. **PA-2 lần 1 sai ngữ nghĩa** (biến giữ trạng thái qua Include). Lần 2 chỉ local hoá hàm, nhưng vẫn còn lỗ: hàm được gọi qua tên ghép trong C++ (`sprintf("%s_%d")`) hoặc `dostring` ở tệp khác với tên ghép mà không xuất hiện nguyên chữ ở đâu → localize sẽ làm nó `nil` im lặng. Rào chắn: hàm không ai gọi trong tệp thì để yên (251); hàm có tên trong chuỗi/chú thích để yên (278). Ước rủi ro thấp nhưng KHÔNG bằng 0; cách phát hiện: `ScriptError.log` "attempt to call a nil value" sau swap; lùi từng tệp = xoá 2 dòng đầu (`local_hoa_lui.py`).
3. **Lợi ích hiệu năng thật của PA-1/PA-2 nhỏ** (Lua ≈ 3,5 % tick): PA-1 chỉ bỏ vài biên dịch chunk/dựng chuỗi; `while getn` hoá ra O(1). Cái đáng nhất là profiler (PA-0) để biết chính xác 0,25–0,45 ms/tick nằm ở đâu — chưa có số vì máy chủ chưa swap.
4. **IncludeOnce** phủ 7 % lượt Include lặp (760/≈32.000) — boot lợi ~0,1 s. Muốn hơn phải xét tay `lib_task.lua`/`log.lua`/`lib_ham.lua` (có gán cấp tệp `X = ...` đơn giản, rất có thể idempotent nhưng công cụ không dám kết luận).
5. **Một state (PA-5) mới chỉ đúng trong mô phỏng**: mô phỏng không có `PlayerIndex` do C++ đặt, không có coroutine SayWait, không có ghi đè hàm C trong E, không có GC dài hạn một heap. Để mặc định TẮT là đúng; bật thử phải có người trực. Lợi dự kiến: RAM Lua −~200 MB (1.7k state × 127 KB), boot −1,5 s; không lợi tick.
6. **Profiler bật mặc định** trên máy chủ: chi phí ~0,1 % thời gian Lua, bảng ≤ 30.000 khoá, tệp log +~60 dòng/10 phút. Chấp nhận được, nhưng là thay đổi hành vi mặc định — nếu chủ không muốn: `set LUA54_PROF=0`.
7. **C1 đổi định dạng ScriptError.log** (`\script\x.lua:12:` thay `[string "?"]:12:`) — công cụ nào grep định dạng cũ (so_boot.py đã chuẩn hoá theo tên tệp) cần rà.
8. **Quy trình git**: gương `serverscript_live` kéo theo `.log` 386 MB → GitHub chặn push suốt 20 phút, phải viết lại commit; hai commit gương trước (c92cf394, 3a687cb4) đã đẩy vẫn còn blob `.log` nhỏ hơn (phình repo, không sửa lịch sử dùng chung). Đã thêm `.gitignore` + dời `.log` ra `_luutru`. Ngoài ra `git reset --soft` trên cây làm việc dùng chung với phiên khác là thao tác nguy hiểm — lần này an toàn vì họ không stage gì, nhưng không nên lặp lại; nên có clone riêng cho gương.
9. **Chưa test trong game**: SayWait/TalkWait (lệnh bài), CallFunction 'b'/'i'/'t' (chưa ai gọi), `LUA54_KIEM_KIEU`. Đều là opt-in, không ảnh hưởng đường cũ.
10. **Việc nên làm tiếp (theo lợi/rủi ro):** swap `.moi` ở phiên vắng → đọc `jx_lua_prof.log` sau 30 phút → sửa đích danh 3–5 script nóng nhất; sau đó cân nhắc bật một state ở khung giờ vắng có trực.
