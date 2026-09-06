# PHÂN TÍCH NÂNG LUA 4.0.1 → LUA 5.4 (05/09/2026)

Chủ (05/09): *"phân tích kỹ trước khi fix — tôi muốn nâng một lần để dùng cho sau này nên cần tốt nhất"*.
Tài liệu này là kết quả giai đoạn 0 (phân tích + dựng công cụ + kiểm trên bản sao). **Chưa đụng cây chạy thật, chưa đổi engine.**

## 1. Hiện trạng đo được

| Chỉ số | Giá trị | Nguồn |
|---|---|---|
| Lua nhúng | **4.0.1** (2000), `Sources\Library\LuaLib` → `LuaLibDll.dll` (x64 máy chủ 265 KB, Win32 client 206 KB) | `lua.h:50`, `LuaLibDll.vcxproj` |
| Script máy chủ | **2.986 tệp**, 11,5 MB (trong `bin\server\script`) | `quet_lua4.py` |
| Script client | **449 tệp** (`bin\client\script`: ui, skill, tong, mail, auction_house…) | `quet_lua4.py` |
| Hàm C++ gắn cho Lua | **742** (bảng trong `ScriptFuns.cpp`; 4 hàm ngoài `#ifdef _SERVER` dùng chung client) | grep |
| Bề mặt API Lua mà engine/core gọi | ~120 tên `lua_*`/`Lua_*` (lớp macro `Engine\Include\lua.h` + `LuaLib.h`), 16 tệp gọi API thô, 742 hàm gắn chỉ dùng `Lua_GetTopIndex / IsNumber / IsString / ValueToNumber / ValueToString / PushNumber / PushString / PushNil / NewTable / SetTable` | grep |
| Kiến trúc | **mỗi tệp script = một `lua_State` riêng**, `lua_open(100)` (stack 100 ô), `Include` = thực thi tệp vào state đang gọi (2.546 dòng Include / 1.098 tệp → thư viện bị chép vào hàng trăm state); `KScriptList` ép `lua_setgcthreshold(200)` mỗi vòng cho step-script | `KLuaScript.cpp:23`, `KScriptList.cpp:44`, `ScriptFuns.cpp:2009` |
| API tự thêm trong DLL | `lua_compilebuffer/compilefile/execute/gettopindex` (ldo.c), `lua_setdebugout/outoutmsg/outerrmsg` (lua.c), `LuaExtend.c` (`Lua_SetTable_*`, `Lua_GetValuesFromStack`) | đọc mã |

### Hạn chế Lua 4 chứng minh bằng chạy thử (lua4.exe của dự án)

| Hạn chế | Bằng chứng | Ảnh hưởng thật |
|---|---|---|
| `%d` chỉ tới 2^31 | `format("%d", 3000000000)` = **-2147483648** | 868 chỗ `format(%d)`; exp/tiền đã vượt 2,1 tỷ |
| `random` chỉ 32.768 mức (`rand()%RAND_MAX/RAND_MAX`) | quay 200.000 lần trong 1..100.000 chỉ ra **32.691** giá trị khác nhau | tỉ lệ rớt có mẫu số > 32.767 không đúng cấu hình; 824 chỗ `random(` |
| Closure chụp **bản sao** biến ngoài, chỉ đọc, không đệ quy cục bộ | `%dem` = 0 trong khi biến ngoài = 5; hàm cục bộ tự gọi → "cannot access a variable in outer scope" | 500 chỗ `%x` / 66 tệp |
| Không boolean, không `#`, không coroutine, không metatable (tag method), không `table.concat`, vararg chỉ qua bảng `arg` | `type(1 == 1)` = number | 18 chỗ tag method (3 tệp), 113 chỗ `arg` (39 tệp) |
| Stack cố định | đệ quy 3.000 tầng → "stack Overflow" ngay cả bản 1024 ô; engine mở 100 ô | vòng Include 23/08 sập boot |
| GC dừng-toàn-bộ (`GCthreshold = 2×nblocks`) | `lgc.c:344` | cú giật 55–100 ms mỗi phút khi nạp lại 1 MB script |
| Xử lý lỗi bằng `call(f, args, "x")`, thiếu là timer chết | bài học AucWeb | 55 chỗ `call(` |

## 2. Quyết định phiên bản: **Lua 5.4.7**

- **Không chọn 5.5.0** (vcpkg đã có): 5.5 cấm gán lại biến điều khiển `for` → bộ quét đếm **2.652 chỗ gán lại biến for trong 629 tệp** (Lua 4 cho phép, nhiều script dùng `i = i + 1` trong vòng for) → phải sửa tay hàng nghìn chỗ; hệ sinh thái (luacheck, EmmyLua, tài liệu) chưa theo kịp. 5.4 → 5.5 sau này là bước rất nhỏ so với 4.0 → 5.4.
- **Không chọn LuaJIT**: ngữ nghĩa 5.1 (không số nguyên 64 bit thật, `%d` vẫn ép double), một người bảo trì, lag hiện tại nằm ở C++ (perf log 05/09: SW_ACTIVATE 50–60 ms/tick khi bot đánh Tống Kim) nên tốc độ Lua không phải nút thắt.
- 5.4.7: số nguyên 64 bit, GC thế hệ, closure thật, coroutine, metatable, `goto`, thư viện chuẩn đầy đủ, 5 năm ổn định.

## 3. Kiến trúc nâng cấp (giữ engine, thay lõi)

```
Engine/Core/Client (742 hàm gắn, ~150 chỗ gọi API)   -- KHÔNG SỬA
        │  gọi tên Lua 4:  lua_gettop, lua_tonumber, lua_call, lua_dostring, lua_newtag ...
        ▼
Engine\Include\LuaLib.h (viết lại): #define lua_gettop lua4_gettop ...   (macro → tên mới)
        ▼
Lua54Dll.dll  =  lõi Lua 5.4.7 (vá 1 dòng in số)  +  lua4compat.c (lua4_* với ĐÚNG chữ ký Lua 4)
                                                  +  lua4compat.lua (nhúng, nạp vào mỗi state)
```

- **`lua4compat.c`** (viết mới, ~600 dòng): mỗi hàm Lua 4 mà engine dùng có bản `lua4_*` với chữ ký cũ → không đụng 742 hàm gắn. Điểm phải xử lý khác biệt:
  - mã kiểu: Lua 4 `TNIL=1, TNUMBER=2, TSTRING=3, TTABLE=4, TFUNCTION=5, TUSERDATA=0` ≠ 5.4 → `lua4_type` ánh xạ; **boolean** (5.4 trả về từ so sánh) → `lua4_isnumber` = đúng, `lua4_tonumber` = 1/0 để hàm C cũ không hỏng.
  - `lua_call` (Lua 4 là **có bảo vệ**, trả mã lỗi) → `lua_pcall` + hàm nhận lỗi ghép traceback rồi gọi `_ALERT` (LuaGameAlert ghi ScriptError.log như hiện nay); `lua_rawcall` → `lua_call`.
  - `lua_dofile/dostring/dobuffer/compilebuffer/execute` → `luaL_loadX` + pcall; mã lỗi đổi về mã Lua 4 (ERRRUN=1, ERRFILE=2, ERRSYNTAX=3…).
  - tag: `lua_newtag/settag/tag/pushusertag/settagmethod/copytagmethods` → metatable trong registry; `pushusertag(ptr, tag)` = full userdata bọc con trỏ + metatable của tag (engine chỉ dùng cho tham số `p` của `CallFunction`).
  - `lua_ref/getref/unref` → `luaL_ref`; `lua_getn` → `luaL_len`; `lua_strlen` → `lua_rawlen`; `lua_stackspace` → hằng; `lua_getgcthreshold/getgccount/setgcthreshold` → `lua_gc` (đặt **GC thế hệ** lúc tạo state); `lua_error(L, msg)` → `luaL_error`; `lua_equal/lessthan` → `lua_compare`; `lua_open(n)` → `luaL_newstate` + openlibs + nạp shim.
  - Thư viện: `lua_baselibopen` = `luaL_openlibs` + chạy `lua4compat.lua`; `iolibopen/strlibopen/mathlibopen/dblibopen` = no-op.
  - Hàm nóng của shim đưa xuống C (đo ở mục 6): `getn, tinsert, tremove, format, random, mod, strsub, strfind, strlen` — port từ `lbaselib.c/lstrlib.c` 4.0.1 sang API 5.4 (giữ nguyên ngữ nghĩa, `%d` 64 bit).
- **Vá lõi 5.4** (1 chỗ, `lobject.c tostringbuff`): không thêm `.0` khi in số thực tròn → `10/2` in ra `5` như Lua 4 (5.4 gốc in `5.0`; ảnh hưởng mọi chỗ ghép chuỗi `..` và `print`). Định dạng vẫn `%.14g` như Lua 4.
- **Giữ mỗi tệp một state** ở giai đoạn 1 (ngữ nghĩa y hệt, rủi ro thấp). Gộp về một state + `_ENV` riêng từng tệp + cache bytecode (hết nạp trùng thư viện, `require`) là **giai đoạn 2**, làm sau khi giai đoạn 1 chạy ổn.

## 4. Bộ công cụ đã dựng (`ReverseTools\lua54\`)

| Tệp | Việc |
|---|---|
| `quet_lua4.py` | tách token Lua 4 thật (không đếm nhầm `%d` trong chuỗi), phân loại 17 nhóm khác biệt; báo cáo `quet_lua4_baocao.txt` |
| `chuyen_lua4_54.py` | bộ chuyển byte-safe (giữ TCVN3, CRLF): `%x`→`x`, `for k,v in t`→`pairs(t)`, thoát lạ `\c`→`c` (đúng như Lua 4 hiểu), `function(...)`→chèn `local arg`, số dính chữ `7then`→`7 then`, `[[ [[ ]] ]]`→`[=[ ]=]`; báo cáo từng dòng `chuyen_baocao.txt` |
| `lua4compat.lua` | lớp tương thích (getn/tinsert với `t.n` đúng như `lbaselib.c`, `call/dostring/dofile` đúng `passresults`, tag method→metatable, `format` ép nguyên, `random` kiểu Lua 4, I/O `openfile/read/write/closefile/readfrom/writeto`…) |
| `kiem_54.py` | kiểm cú pháp toàn cây bằng **Lua 5.4 thật** (`C:\Program Files\Wireshark\lua54.dll` qua ctypes, không cần tải gì) |
| `chay_54.py` | chạy một tệp trên 5.4 + shim, `print` ra tệp để diff với `lua4.exe` |
| `test\dump_cfg.lua`, `test\bench.lua` | đối chứng dữ liệu + đo tốc độ (viết bằng cú pháp Lua 4, chính bộ chuyển đổi chúng) |

## 5. Kết quả trên bản sao toàn bộ script

| Kiểm | Kết quả |
|---|---|
| Chuyển máy chủ | 2.986 tệp, **135 tệp có sửa** (UPVAL 415, FORIN 142, ESCAPE 17, VARARG 41, NUMKW 1), 2.851 tệp giữ nguyên từng byte |
| Chuyển client | 449 tệp, 35 tệp có sửa (UPVAL 85, FORIN 54, VARARG 31) |
| Biên dịch Lua 5.4 máy chủ | **2.986 / 2.986 OK** |
| Biên dịch Lua 5.4 client | 446 OK; 3 tệp lỗi (`lib\basic.lua`, `lib\say.lua`, `lib\string.lua`) dùng chú thích C `/* */` — **đã lỗi sẵn trên Lua 4** (tệp chết, client không nạp) |
| Đối chứng dữ liệu | nạp toàn bộ `ch_all.lua` + `cauhinh_hoatdong.lua` + `cfgw_meta.lua` (3.858 khoá, 4.162 dòng in) trên Lua 4 gốc và 5.4 + shim: **giống hệt**, chỉ 3 dòng khác đúng dự kiến: `10/2` in `5.0` (vá lõi sẽ hết), `date(..) ~= nil` in `true` thay vì `1` (boolean), `tonumber("0x10")` = 16 ở cả hai (đã sửa shim cho giống) |

## 6. Tốc độ (cùng bench, cùng máy)

| Phép đo | Lua 4.0.1 | Lua 5.4 + shim Lua | Ghi chú |
|---|---|---|---|
| vòng lặp lồng 600×600 | 12 ms | **3 ms** | lõi 5.4 nhanh 4× |
| gọi hàm 2.000.000 lần | 74 ms | **29 ms** | 2,5× |
| tinsert 200.000 + đọc | 28 ms | 49 ms | shim viết bằng Lua chậm hơn → đưa xuống C |
| format + ghép 100.000 | 54 ms | 116 ms | shim `format` phân tích `%` bằng Lua → đưa xuống C |
| floor + random 1.000.000 | 64 ms | 159 ms | `random` bọc Lua → đưa xuống C |

Kết luận: lõi nhanh 2,5–4×; các hàm thư viện tên cũ **phải cài bằng C trong DLL** (mục 3) để không chậm hơn Lua 4 ở đường nóng (floor 6.154 chỗ, getn 1.833, format 1.604, random 824).

## 7. Khác biệt ngữ nghĩa phải RÀ TAY (đã liệt kê đích danh trong báo cáo quét)

| # | Việc | Số lượng | Xử lý |
|---|---|---|---|
| 1 | `%x` mà biến ngoài bị gán lại sau khi tạo closure (5.4 thấy giá trị mới) | **1** (`auction_house\auction_manager.lua:343 %nCount`, gán lại dòng 349) | sửa tay: chụp bản sao `local nCount_ = nCount` |
| 2 | `true`/`false` dùng như biến (Lua 4 = nil) | 4 (`timerserver.lua:399`, `lenhbaitanthu.lua:367`, `pubg.lua:28,63`) | rà từng chỗ: 5.4 thành boolean thật, luồng `if` có thể đổi |
| 3 | Chuỗi có thoát lạ (`"C:\server\dulieu"` Lua 4 đọc thành `C:serverdulieu`) | 17 (`timerserver.lua:383`, `hd3_admin.lua:129`, 12 tệp `npcmonphai\*.lua:7` `\F`) | bộ chuyển giữ đúng hành vi Lua 4 (bỏ `\`); dev quyết có sửa ý gốc không |
| 4 | Kết quả so sánh thành boolean thay vì 1/nil | không có chỗ gán/return trực tiếp; 1.752 chỗ `== 1` là so với số thật | C++: `lua4_tonumber(boolean)` = 1/0; Lua: chỉ lộ khi ghép chuỗi (`"x" .. (a==b)` sẽ lỗi ở 5.4) → chạy thử trên máy thử |
| 5 | Thứ tự duyệt bảng `for k,v in` khác giữa hai lõi | 197 vòng; 9 tệp vừa duyệt bảng vừa `Say(` (menu NPC có thể đổi thứ tự) | rà 9 tệp, đổi sang duyệt theo chỉ số |
| 6 | Nhân số nguyên vượt 2^63 quay vòng (Lua 4 là số thực) | hằng > 2^31: 37 chỗ (mốc ngày, 1e11 lượng) | không có tích nào tới 9,2e18; đánh dấu theo dõi |
| 7 | 3 tệp client chết (chú thích C) | 3 | xoá hoặc sửa, không ảnh hưởng |
| 8 | Lỗi runtime giờ báo kèm traceback đầy đủ | – | ScriptError.log rõ hơn, không cần làm gì |

## 8. Kế hoạch còn lại

| Giai đoạn | Việc | Nghiệm thu |
|---|---|---|
| 1a (cần nguồn 5.4.7) | dựng `Lua54Dll.vcxproj` (x64 + Win32) từ lua-5.4.7 + vá in số + `lua4compat.c` (hàm nóng bằng C) + nhúng shim; viết lại `Engine\Include\LuaLib.h`/`lua.h`; sửa `KLuaScript.cpp` (`lua_open` → tạo state kiểu mới), `KScriptList` (GC), 9 chỗ `lua_dostring`, `KCore.cpp:284 setdebugout` | Core (x64) + Engine/Client (Win32) build sạch, `lua4compat` self-test qua |
| 1b | máy chủ **thử** (bản sao `bin\server` + DB thử): boot với cây script đã chuyển, chạy bot 1000 con 2 giờ, so `ScriptError.log`, `jx_perf_server.log`, bot.log với bản 4.0 | không lỗi script mới, SCRIPT_TIME giảm, tick không tăng |
| 1c | client thử: đăng nhập, UI, kỹ năng, bang, thư, đấu giá (449 script client) | không lỗi `ScriptError.log` client |
| 1d | lên thật theo quy trình `.moi` (CoreServer + LuaLibDll máy chủ; Game.exe/CoreClient + LuaLibDll client cùng lúc) + cây script đã chuyển; giữ bản cũ để lùi trong 1 phút | 24 giờ đầu theo dõi log |
| 2 (sau) | một state + `_ENV` từng tệp + cache bytecode; `require` cho thư viện; bỏ nạp lại 1 MB/phút; coroutine cho hội thoại nhiều bước | perf log |

**Lùi lại (rollback):** DLL cũ + cây script cũ vẫn giữ nguyên trên đĩa (`.truoc`); đổi lại là chạy được ngay vì bộ chuyển không đụng cây thật cho tới 1d.

## 9. Việc cần chủ quyết

1. **Cho phép tải mã nguồn Lua 5.4.7** (`lua-5.4.7.tar.gz`, 374.097 byte, từ `https://www.lua.org/ftp/`) — máy đang có mạng, không cần gì khác. Đây là điều kiện để làm 1a.
2. Xác nhận chọn **5.4.7** (mục 2). Nếu chủ muốn 5.5.0 thì cộng thêm việc sửa 2.652 chỗ gán biến for.
3. Có máy chủ thử (hoặc cho phép chạy bản thử trên cùng máy với cổng khác) cho 1b.

## 10. Giai đoạn 1a ĐÃ LÀM (05/09 trưa) — chờ chủ khởi động lại

Chủ: *"oke hãy làm trên máy chủ hiện tại — làm xong nhớ chạy kiểm tra"*.

**Đã dựng:** `Sources\Library\Lua54` (nguồn 5.4.7 + 2 vá + `lua4compat.c` 84 hàm + shim nhúng), `Engine\Include\LuaLib.h` mới (742 hàm gắn, KLuaScript không sửa), 4 vcxproj trỏ `Lib\lua54\$(Platform)\Lua54Dll.lib`. Build sạch: Lua54Dll x64/Win32, CoreServer, engine.dll máy chủ, CoreClient, Engine.dll client, Game.exe.

**Kiểm trước khi lên (đã chạy):** `lua4_selftest` 25 mục = 0 lỗi; dump 3.858 khoá cấu hình qua API `lua4_*` giống Lua 4 (chỉ khác `1`→`true` cho biểu thức so sánh); bench qua DLL nhanh hơn Lua 4 ở mọi mục (2/27/12/26/68 ms so 12/75/29/53/64); 2.986 script máy chủ + 446 client biên dịch 5.4 OK.

**Đã đặt trong `bin\server`:** `CoreServer.dll.moi` 4081ce2d · `engine.dll.moi` 1f4f8c92 · `Lua54Dll.dll` 8ef0b246 (đặt thẳng, bản cũ không dùng tới) · `LUA54.moi` (dấu hiệu đổi cây script) · `tools\chuyen_lua4_54.py` · `ChayGameServer.bat` mới (bản cũ `.truoc`) · `LuiLua4.bat`.
**Đã đặt trong `bin\client`:** `Game.exe.moi` 41a0d7e3 · `CoreClient.dll.moi` f400b7ef · `Engine.dll.moi` 622b12f3 · `Lua54Dll.dll` e4f341cf · `LUA54.moi` · `tools\` · `ChoiGame.bat` mới (bản cũ `.truoc`) · `LuiLua4.bat`.

**Cách lên (chủ làm):** tắt GameServer → chạy `ChayGameServer.bat`: bat thay 3 file `.moi`, thấy `LUA54.moi` và `CoreServer.dll` đã dùng Lua54Dll thì chạy converter tại chỗ (`script` → `script54`, ~20 s, lấy đúng cây script hiện tại kể cả sửa đổi mới nhất), đổi `script`→`script.lua4`, `script54`→`script`, đổi `LUA54.moi`→`LUA54.da_doi`, mở GameServer. Client: thoát game → `ChoiGame.bat` (tương tự). Hai lớp bảo vệ: bat chỉ đổi script khi binary đã dùng Lua54Dll; Lua54Dll từ chối chạy (hộp thoại + ScriptError.log) nếu `script\LUA54_DA_CHUYEN.txt` thiếu.

**Lùi lại:** tắt → `LuiLua4.bat` (trả `.truoc` cho CoreServer/engine (Game/CoreClient/Engine), `script.lua4`→`script`, không xoá gì) → chạy bat thường.

**Kiểm sau khi lên (Claude làm khi chủ báo):** ScriptError.log không có lỗi mới kiểu `attempt to call global`/`unexpected symbol`; `jx_perf_server.log` SCRIPT_TIME giảm; bot.log vẫn có [BotTK]/[BotDT]; gcfg nạp bình thường; Tống Kim 17:50 chạy; client đăng nhập, UI, kỹ năng, bang, thư, đấu giá.

**Quy tắc viết script từ giờ (cây `script` = bản 5.4):** không dùng `%x` (viết `x` thẳng), duyệt bảng `for k, v in pairs(t) do`, hàm `...` tự khai `local arg = {n = select("#", ...), ...}`, chuỗi có `\` phải là thoát hợp lệ (`\`), số không dính chữ (`7 then`). Tên hàm cũ (getn, format, strfind, floor, date...) vẫn dùng được nhờ lớp tương thích. `script.lua4` là bản lưu, ĐỪNG sửa.

## 11. Toi uu RunTime timerserver.lua (05/09 22:30 - 06/09 00:05, sua nong, khong restart)

Do 7 gio 5.4 (`ReverseTools/lua54/so_sanh_perf.py`): SCRIPT_TIME max 39 ms MOI PHUT (568/568 phut > 20 ms), gio chan 90-126 ms.
C++ `CoreServerShell.cpp:1164` goi `RunTime` MOT lan moi phut (giay 0) -> moi thu trong do la viec theo phut.

| Nguyen nhan | Do | Sua |
|---|---|---|
| `dofile("script/timerserver.lua")` moi phut chay lai 33 Include = 47 tep 905 KB bien dich lai | 21-26 ms | `TS_CoThayDoi`: doc + so sanh noi dung (6 ms), chi dofile khi doi. Sua nong van an trong 1 phut |
| `HD3_DonNpcCu` 5 luot quet `Npc[1..MAX_NPC]` (KJx2WarInfra.cpp) | 14-34 ms (quet TEN 33 ms luc Tong Kim) | xoay vong 1 luot/phut (`HD3_DON_XOAYVONG=0` = nhu cu; boot/admin quet het) |
| `lib:OptionFunction` dostring("return a>b") moi lan kiem dieu kien hoat dong | nho, thuong xuyen | so truc tiep khi hai ve la so (hieu luc khi restart) |
| Gio chan: Vuot Ai `OnTrigger` (dong/mo mission cac tang, KickOutAll, ClearMapNpc) + PLD | 44 + 4 ms | viec that cua tinh nang, giu; giam nua phai sua C++ hoac lich `HD3_VA_GIO` |
| 00:00: `UpdateNgayMoiAllPlayer` 1001 nguoi | ~94 ms | mot lan/ngay, giu |

Ket qua: RunTime moi phut 36-43 -> 9-12 ms; TICK max 42-51 -> 20-25 ms; het rot khung theo phut.
Bo do: `TS_tProf` 13 khoi -> `GhiLog("PROF", ...)` trong `logs/hethong.log` khi >= 8 ms (`TS_PROF_NGUONG`); `HD3_Tick`/`HD3_DonNpcCu` co dong rieng.
Tep: `serverscript_jx2/lua54_toiuu/script/` (timerserver.lua, tinhnang/3hoatdong/hd3_driver.lua, activitysys/functionlib.lua) = ban dang chay.
Quy tac sua nong: python latin-1 giu CRLF + dem byte cao, `kiem_54.py`, chay thu bang Lua54Dll + engine gia, cp de; RunTime cu tu dofile ban moi phut ke.

## 12. Có nên viết lại TOÀN BỘ script sang Lua 5.4 thuần không? + phân tích hiệu năng (06/09/2026)

Chủ hỏi (06/09): *"có nên đổi từ Lua4 qua Lua5.4 hết toàn bộ script không? — và phân tích hiệu năng khi nâng toàn bộ script lên Lua5.4"*.

### 12.1 Hiện trạng: phần "nâng lên 5.4" đã xong 100 %, phần còn lại là "viết lại kiểu 5.4"

| Lớp | Trạng thái |
|---|---|
| Lõi thông dịch | Lua 5.4.7 (`Lua54Dll.dll`) ở máy chủ, client và relay từ 05/09 |
| Cú pháp script | 3 cây đã chuyển hết: máy chủ `script` 3.031 + `scriptjx2` 189 + client 450 tệp, biên dịch 5.4 sạch |
| Tên hàm thư viện kiểu Lua 4 | **12.990 lời gọi / 1.243 tệp** (máy chủ 8.177/897, scriptjx2 1.005/86, client 3.808/260) vẫn gọi `getn, format, floor, random, tinsert, strfind, date, call, dostring…` qua lớp tương thích |
| Lớp tương thích | 6 hàm nóng bằng C trong DLL (`getn, tinsert, tremove, format, random, randomseed`); ~30 tên chỉ là **alias** trỏ đúng hàm C của 5.4 (`floor = math.floor`, `strfind = string.find`, `date = os.date`…); còn lại là wrapper Lua (`mod, call, dostring, dofile, sort, foreach, tag*, I/O cũ`) |

Vậy câu hỏi thực chất là: **có nên bỏ lớp tương thích, đổi 12.990 chỗ sang `math.floor / string.format / #t / table.insert / pcall…` hay không.**

### 12.2 Khuyến nghị: KHÔNG viết lại hàng loạt — giữ lớp tương thích làm API chuẩn của game

1. **Hiệu năng gần như không đổi** (số đo mục 12.3–12.4): alias có tốc độ bằng hàm gốc, hàm nóng đã là C; Lua chỉ chiếm phần rất nhỏ của tick.
2. **Rủi ro nằm đúng ở những hàm không phải alias**, và là lỗi im lặng lúc chạy (như vụ gán biến `for` 05/09):
   - `format("%d", x)` với `x` số thực: Lua 4 ép nguyên, **5.4 `string.format` ném lỗi** "number has no integer representation" → **957 chỗ** `%d/%i` trong cây máy chủ phải bọc `math.floor` từng chỗ.
   - `random(a, b)` với đối số là biến (có thể lẻ): **278 chỗ**; `math.random` 5.4 cũng ném lỗi với số thực.
   - `getn`/`tinsert` với quy ước `t.n` và bảng có lỗ (`{1,nil,3}` = 3 ở Lua 4, `#t` không xác định); 12 chỗ đọc `.n` trực tiếp.
   - `mod(a,b)` = C `fmod` (dấu theo số bị chia) ≠ `%` 5.4 (dấu theo số chia) khi âm; `mod(x,0)` trả nan chứ không lỗi.
   - `call(f, {...}, "x")` / `dostring` có luồng bắt lỗi riêng (`_ERRORMESSAGE`), 76 + 20 chỗ.
3. **Không có bộ kiểm thử tự động** cho 315.000 dòng logic game → mọi khác biệt chỉ lộ khi người chơi chạm vào tính năng.
4. Khối lượng ~13.000 chỗ / 1.243 tệp + 272 chỗ `arg` + 152 dòng I/O cũ + 17 tag method — đổi máy móc được ~7.000 chỗ alias (vô nghĩa về hiệu năng), phần còn lại phải rà tay.
5. Đây là cách các engine game lớn vẫn làm: lớp API ổn định cho script cũ, script mới viết theo chuẩn mới.

**Nên làm thay vào đó (rẻ, không đụng script):**
- Quy tắc từ giờ: script **mới** viết 5.4 thuần (`math.*`, `string.*`, `#`, `table.insert`, `pcall`); tệp cũ chỉ đổi khi đã phải sửa nó.
- Đưa các wrapper Lua còn lại trong shim xuống C nếu thấy trên đường nóng (`mod`, `call`, `foreach`) — hiện không cần vì chưa có chỗ nào nóng.
- Sửa 10 chỗ `while … getn(t)` (O(n²), Lua 4 cũng vậy) thành `#t` khi tiện.
- **Lỗi có sẵn trong shim (chưa nổ vì 0 script gọi):** `lua4compat.lua:175` `_G.collectgarbage` và `gcinfo` gọi lại chính nó → tràn ngăn xếp. Sửa: giữ bản gốc `local cg = collectgarbage` trước khi gán. Cần rebuild Lua54Dll khi có dịp.

### 12.3 Đo trên máy chủ thật (`jx_perf_server.log`, 3.467 khối, cắt 16 lần chạy; chỉ lấy khối online ≥ 900)

| Lần chạy | Lõi | TICK tb / p95 / max (ms) | SCRIPT_TIME tb / max (ms) | SCRIPT chiếm % tick | SW_ACTIVATE tb |
|---|---|---|---|---|---|
| 04/09 10:00–13:13 | Lua 4 | 6,88 / 10,4 / 801 | 0,063 / 794 | 0,12 % | 4,31 |
| 04/09 13:17–15:00 | Lua 4 | 8,94 / 17,2 / 332 | 0,075 / 299 | 0,14 % | 5,58 |
| 04/09 17:38–19:28 | Lua 4 | 7,32 / 10,5 / 1.246 | 0,067 / 443 | 0,13 % | 4,64 |
| 05/09 00:00–12:20 | Lua 4 | 8,90 / 16,1 / 1.619 | 0,057 / 566 | 0,11 % | 6,31 |
| 05/09 13:02–23:59 | **5.4** | 6,61 / 8,8 / 684 | 0,032 / 677 | 0,09 % | 4,31 |
| 06/09 00:00–12:18 | **5.4** | 6,61 / 9,2 / 800 | 0,015 / 794 | 0,01 % | 4,30 |
| 06/09 13:32–14:37 | **5.4** + tối ưu mục 11 + C++ quét NPC theo vùng | 7,53 / 12,6 / 139 | 0,020 / **57** | 0,01 % | 4,87 |

- `SCRIPT_TIME` **chỉ đo `RunTime` của `timerserver.lua`** (C++ gọi 1 lần/phút, `KPerfTick.h:34`). Phần Lua chạy trong `SW_ACTIVATE` (hội thoại NPC, vật phẩm, kỹ năng, nhiệm vụ, hoạt động) **chưa có đồng hồ riêng** → không thể nói chính xác Lua chiếm bao nhiêu phần trăm tick; chỉ biết tick trung bình 6,6–7,5 ms trong đó `SW_ACTIVATE` (C++ + Lua sự kiện) 4,3–4,9 ms. Muốn biết chắc: thêm một `PERF_SCOPE` trong `KLuaScript::CallFunction` (5 dòng, chi phí một lần đọc đồng hồ).
- Sau nâng cấp, tick trung bình và p95 **không tăng** (6,9–8,9 → 6,6–7,5 ms) dù thay lõi; đỉnh `SCRIPT_TIME` từng phút hôm nay median 18 ms, max 57 ms (trước 300–880 ms).

### 12.4 Micro-bench: tên hàm cũ (lớp tương thích) vs 5.4 thuần — chạy trên đúng `Lua54Dll.dll` x64 của máy chủ, i7-13700K, best-of-3

| Phép đo | n | cũ (ms) | mới (ms) | cũ/mới | Ghi chú |
|---|---:|---:|---:|---:|---|
| `floor(x)` vs `math.floor(x)` | 2.000.000 | 36 | 40 | 0,90 | alias = cùng hàm C, tra 1 biến toàn cục nhanh hơn tra `math.floor`; `(x)//1` 13 ms |
| `strfind` / `strsub` / `date` vs `string.find` / `string.sub` / `os.date` | 500k / 500k / 100k | 18 / 17 / 35 | 20 / 18 / 35 | 0,9–1,0 | alias |
| `format("%d-%s")` vs `string.format` | 300.000 | 47 | 52 | 0,90 | bản C trong DLL; với `%d` số thực: 31 vs 39 ms (5.4 phải `math.floor` trước) |
| `tinsert(t,v)` vs `table.insert` | 200.000 | 11 | 8 | 1,38 | ghi thêm `t.n`; `t[#t+1]=v` 4 ms |
| `random(1,100)` vs `math.random` | 1.000.000 | 50 | 27 | 1,85 | +23 ns mỗi lần |
| `mod(a,b)` vs `math.fmod` vs `a % b` | 1.000.000 | 33 | 25 | 1,32 | wrapper Lua; `%` 5 ms |
| `getn(t)` có `t.n` vs `#t` | 1.000.000 | 28 | 6 | 4,7 | +22 ns mỗi lần |
| `getn(t)` không `t.n`, 10 / 100 / 1000 phần tử | 200k / 20k / 2k | 25 / 17 / 16 | 1 / <1 / <1 | ≥ 25 | quét O(n) đúng như `lua_getn` Lua 4 — chỉ đau ở `while getn` (10 chỗ) |
| `call(f,{a,b},"x")` vs `pcall` | 200.000 | 131 | 6 | 22 | wrapper Lua + tạo bảng; tuyệt đối 0,65 µs/lần, 76 chỗ |
| `dostring` vs `load()()` | 50.000 | 46 | 31 | 1,48 | 20 chỗ |
| `sort` vs `table.sort` (1.000 pt) | 200 | 39 | 31 | 1,26 | 9 chỗ |
| `foreachi` / `foreach` vs `ipairs` / `pairs` | 2.000 × 1.000 | 110 / 57 | 25 / 27 | 4,4 / 2,1 | 0 chỗ dùng trong 3 cây |
| gọi hàm toàn cục vs `local` | 3.000.000 | 52 | 39 | 1,33 | kiểu viết, không liên quan lớp tương thích |

Đọc số: nhóm chiếm 90 % lời gọi (floor 6.169, format 1.731, getn 2.006 chủ yếu `for i=1,getn(t)`, strfind/strsub/date) **không nhanh hơn khi viết lại**; nhóm chậm hơn thật (`random` +23 ns, `getn` +22 ns, `tinsert` +15 ns, `mod` +8 ns, `call` +0,6 µs) có tổng chi phí cận trên: kể cả 100.000 lời gọi/giây × 50 ns = **5 ms mỗi giây = 0,5 % một lõi**, tức dưới 0,03 ms trên tick 6,6 ms. Viết lại toàn bộ để đổi lấy tối đa ~0,4 % tick là không đáng.

### 12.5 Chỗ hiệu năng THẬT nằm ở kiến trúc nạp script, không ở phương ngữ — số đo cây live (`do_include.py`)

| Chỉ số | Số đo |
|---|---|
| Cây máy chủ | 3.220 tệp (`script` + `scriptjx2`), **10,2 MB** nguồn; 2.839 dòng `Include` trong 1.230 tệp |
| Mô hình hiện tại | mỗi tệp một `lua_State`; `Include` biên dịch lại tệp vào state đang gọi, **không khử trùng** (`LuaIncludeFile`, ScriptFuns.cpp:2010) |
| Byte phải biên dịch lúc boot | **338,6 MB = 33 lần cây nguồn** (42.055 lượt biên dịch tệp) |
| Nếu khử trùng Include trong từng state | 154,3 MB = 15 lần |
| Nếu một state chung + cache bytecode | 10,2 MB = 1 lần |
| State nặng nhất | `item/lenhbaiadmin.lua` 7,0 MB / 766 lượt biên dịch (duy nhất chỉ 1,36 MB / 117 tệp); `challengeoftime/npc_death.lua` 6,4 MB / 783 lượt |
| Tệp bị chép nhiều nhất | `task/tollgate/killbosshead.lua` 376 KB × 53 state = 19,9 MB; `global/itemset.lua` 67 KB × 286 state = 19,2 MB; `lib/lib_task.lua` × 495 state; `activitysys/playerfunlib.lua` × 285 |
| Tốc độ biên dịch 5.4 | 62 MB/s (10,2 MB = 164–178 ms) → boot ≈ **5,4 s biên dịch + 0,8 s tạo 3.220 state** (chưa kể chạy thân chunk) |
| Nạp lại từ bytecode (`string.dump` → `load`) | toàn cây 9,1 MB bytecode nạp trong **13 ms** = nhanh hơn biên dịch **14 lần** |
| Bộ nhớ proto (bytecode + hằng + debug) | 14,3 MB cho một bản cây (1,4 × nguồn); bản thứ hai trong cùng state +12,3 MB |
| Bộ nhớ mỗi state cơ bản | 68 KB (libs 5.4 + shim) + 59 KB (742 hàm C đăng ký) = **127 KB** → 3.220 state = **399 MB** |
| Ước tính heap Lua lúc boot | proto lặp ≈ 338,6 MB × 1,2–1,4 ≈ 410–470 MB + 399 MB base ≈ **0,8–0,9 GB** (≈ 11 % working set GameServer 7,3 GB; chưa tính bảng dữ liệu do chunk tạo) — so với ≈ 15 MB nếu một state |
| Include trỏ tới tệp không tồn tại | 56 tệp / 105 dòng (vd `npcthon/npcmonphaifactionhelper.lua` ×13, `missions/clearskill/testhole.lua` ×8) — cần rà, một phần có thể nằm trong chú thích khối |

Lộ trình giai đoạn 2 theo lợi ích / rủi ro:

| Bước | Việc (chỉ C++/DLL, không đụng script) | Được gì | Rủi ro |
|---|---|---|---|
| 2a | Khử trùng `Include` trong từng state (bảng đã-nạp trong registry, `LuaIncludeFile`) | biên dịch boot 338 → 154 MB, RAM proto −55 %, boot nhanh ~2,5 s | thấp: chỉ đổi hành vi khi một tệp bị Include 2 lần trong cùng state (định nghĩa lại hàm giống hệt) |
| 2b | Cache bytecode dùng chung: biên dịch mỗi tệp một lần, `lua_load` nhị phân vào từng state | biên dịch boot 5,4 s → ~0,4 s; nạp nóng nhanh | thấp; RAM không đổi (proto vẫn theo state) |
| 2c | Một `lua_State` chung, mỗi tệp một `_ENV` riêng (globals cách ly như hiện nay), thư viện dùng chung | RAM Lua 0,8 GB → ~20 MB; GC một chỗ; `Include` = `require` | trung bình–cao: sửa `KLuaScript/KScriptList/KSortScript`, 742 hàm gắn dùng `L` chung, phải giữ cách ly biến toàn cục giữa tệp; test dài |

Kết luận hiệu năng: **lõi 5.4 đã cho phần lợi thấy được (tick không tăng, đỉnh script từ 300–880 ms xuống 57 ms sau tối ưu mục 11); viết lại 13.000 lời gọi sang tên 5.4 thuần không đem lại gì đo được và mở ra hàng trăm điểm lỗi im lặng; tiền hiệu năng tiếp theo nằm ở 2a → 2b (rẻ) và ở C++ (`SW_ACTIVATE` 4,3 của 6,6 ms tick).**

Công cụ đo đợt này (`ReverseTools\lua54\danhgia_0609\`): `bench_compat.lua` + `bench_run.py` (micro-bench qua Lua54Dll), `do_include.py` (đồ thị Include, byte biên dịch, state cơ bản), `mem_proto.py` (heap proto qua GC step vì `collectgarbage` của shim lỗi), `perf_runs.py` (cắt perf log theo lần chạy).

## 13. Giai đoạn 2a + 2b ĐÃ THI CÔNG (06/09 15:00–15:25) — `.moi` đã đặt, chờ chủ restart

Chủ (06/09): *"hãy làm tiếp"* (sau mục 12). Chỉ đụng Lua54Dll + Core, **không đụng script**. Nhánh `lua54b-0609` (worktree `D:\GAMEDEVNEW_wt_lua54b`), đã gộp origin/main **1b90aee9**.

### 13.1 Làm gì

| # | Chỗ | Nội dung |
|---|---|---|
| 1 | `Lua54Dll` `lua4compat.c` `lua4_dofile` (đường `Include`) | **2a**: mỗi state giữ closure đã biên dịch theo đường dẫn (registry `lua4.inc.fn/st`); Include lần 2 trong cùng state KHÔNG biên dịch lại nhưng **vẫn chạy lại thân tệp** → ngữ nghĩa y hệt Lua 4. **2b**: bytecode dùng chung cả tiến trình (bảng băm đường dẫn chuẩn hoá → code + mtime + size, khoá CRITICAL_SECTION); state khác nạp nhị phân. Tệp đổi trên đĩa (mtime hoặc size khác) → biên dịch lại → sửa nóng và `ReLoadAllScript` vẫn ăn. Tắt: biến môi trường `LUA54_KHONG_CACHE=1` hoặc `lua4_inc_set(0)`. Thống kê qua `lua4_inc_stats`. |
| 2 | `lua4_call` | đồng hồ ở độ sâu 0 khi `lua4_perf_set(1)`; Core đọc + xoá mỗi tick qua `lua4_perf_read` → giai đoạn mới **`LUA_CALL`** trong `jx_perf_server.log` = tổng ms Lua mỗi tick (mọi lời gọi C++→Lua, kể cả Include/dofile). Không bật = không đọc đồng hồ. |
| 3 | `KPerfTick.h/.cpp` | `PERF_LUA_CALL` + `PerfLuaDoc()` (GetProcAddress → chạy được với DLL cũ) |
| 4 | `KSortScript.cpp g_IniScriptEngine` | dòng log `[script] LoadAllScript: N tep, M ms; cache Include: bien dich a, dung lai cung state b, bytecode chung c, bo qua phan tich d KB, bytecode giu e KB` (console GameServer + g_DebugLog) |
| 5 | `lua4compat.lua:175` | sửa lỗi có sẵn: `collectgarbage`/`gcinfo` gọi lại chính nó → tràn ngăn xếp (0 script gọi nên chưa nổ). Giữ `collectgarbage_54` gốc. |

### 13.2 Kiểm đã chạy

- `lua4_selftest` **0 lỗi** (thêm 2 mục: Include lần 2 phải chạy lại thân tệp + không biên dịch; tệp đổi phải biên dịch lại; đồng hồ đếm được).
- Mô phỏng boot thật (`boot_gia.py`: 3.098 state như `LoadAllScript`, `Include` thật qua `lua4_dofile` với remap JX2, hàm engine giả, sandbox I/O, cùng DLL x64 sẽ deploy):

| Chế độ | Tổng | Tạo state + shim | Chạy thân tệp + Include | State có lỗi `_ALERT` |
|---|---:|---:|---:|---:|
| Không cache | 6,37 s | 0,97 s | 5,25 s | 452 |
| Cache 2a+2b | **2,98 s** | 0,95 s | **1,89 s** | 452 |

Tập lỗi `_ALERT` (2.192 dòng, do hàm engine giả) **giống hệt từng byte** hai chế độ → cache không đổi hành vi. Thống kê: 466 lần biên dịch, 30.518 lần dùng lại closure cùng state, 11.861 lần nạp bytecode chung, bỏ qua phân tích **263 MB** nguồn, bytecode giữ 4,4 MB.
- Build: Lua54Dll x64/Win32 0 lỗi; CoreServer "Server Release x64" 105 tệp 0 lỗi, obj riêng trong worktree. Dấu hiệu tính năng trong DLL mới: CL_Cong, AUC_MsgTong, S13, GetBiaoChePos, RemoteExecute, TKDich, LoadAllScript:, LUA_CALL.

**Đính chính mục 12.5:** ước tính RAM "proto lặp 410–470 MB" quá cao — Include lại trong cùng state tạo proto mới nhưng bản cũ thành rác và GC dọn, nên phần bền vững chỉ là bản duy nhất theo từng state (~154 MB nguồn ≈ 200 MB) + 399 MB state cơ bản ≈ **0,6 GB**. Cache 2a giữ closure sống nên RAM không tăng; cái được là thời gian boot, rác GC và sau này là nạp lại nhanh.

### 13.3 Đã đặt (chờ chủ chạy bat) và cách kiểm sau restart

| Tệp | Hash | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | 4fcb02f9 | từ origin/main 1b90aee9 = **gồm cả bot nội/ngoại đợt 1+2 (4395bd89, ca4f1d03) của phiên khác**; bản `.moi` trước đó của phiên bot giữ ở `CoreServer.dll.moi.truoc_lua54b_1454` |
| `bin\server\Lua54Dll.dll.moi` | 2fe07c19 | x64; bat đã có `:capnhat Lua54Dll.dll` |
| `bin\client\Lua54Dll.dll.moi` | 2e8a2677 | Win32; `ChoiGame.bat` có `:capnhat Lua54Dll.dll` |
| PDB | `D:\GAMEDEVNEW_wt_lua54b\Sources\Core\x64\ServerRelease\CoreServer.pdb` | mổ dump |

`bin\multiserver` (relay) chưa phủ vì không có bat capnhat; DLL Win32 mới tương thích, đặt sau.

Kiểm sau restart: (1) console GameServer / g_DebugLog có `[script] LoadAllScript: 3098 tep, … ms` — kỳ vọng ≈ 2–3 s thay cho ≈ 6–7 s, `bo qua phan tich ≈ 260.000 KB`; (2) `jx_perf_server.log` có dòng `LUA_CALL n=1080 tb=… ms` → lần đầu biết Lua chiếm bao nhiêu tick; (3) `ScriptError.log` không có lỗi mới; (4) sửa nóng: ghi một tệp được Include (vd `cauhinh\ch_*.lua`) → phút sau `RunTime` đọc bản mới (mtime đổi → biên dịch lại).

**Lùi:** đặt `CoreServer.dll.moi.truoc_lua54b_1454` → `CoreServer.dll.moi` và bỏ `Lua54Dll.dll.moi`; hoặc chỉ tắt cache bằng `set LUA54_KHONG_CACHE=1` trong `ChayGameServer.bat` trước `start GameServer.exe` (DLL vẫn mới).

Công cụ: `ReverseTools\lua54\danhgia_0609\` (`boot_gia.py`, `selftest_dll.py`, `patch_lua54.py`, `patch_core.py`, `build_lua54b.bat`). Tiếp theo: xem [SAPXEP_SCRIPT_0609.md](SAPXEP_SCRIPT_0609.md) (sắp xếp lại cây script — chờ chủ quyết).
