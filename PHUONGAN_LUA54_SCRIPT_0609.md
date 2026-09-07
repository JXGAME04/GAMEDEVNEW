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
