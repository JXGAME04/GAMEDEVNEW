# SẮP XẾP LẠI CÂY SCRIPT — kiểm kê 06/09/2026 và đề xuất (CHỜ CHỦ QUYẾT)

Chủ (06/09): *"thêm nhiệm vụ nữa: sắp xếp lại script — tôi đang thấy script hiện tại rất lộn xộn"*.
Tài liệu này là kết quả kiểm kê bằng công cụ trên cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`, chưa di chuyển/xoá gì. Công cụ: `ReverseTools\lua54\danhgia_0609\saplai_phantich.py`, `trap_ref2.py`; báo cáo thô `saplai_baocao.md`, `trap_ref_baocao.txt`.

## 1. Kiểm kê (3.216 tệp `script` + `scriptjx2`, 10,2 MB)

Nguồn tham chiếu đã quét: mọi chuỗi `.lua` trong script; 73 tệp `settings` có `.lua`; 104 tệp C++; **5.008 tệp `Region_S.dat`** (trap lưu **ID băm** `g_FileName2Id("\script\…")`, KFilePath.cpp:442, dạng `\script\...` chữ thường — đã đối chứng 28/28 ID khớp); `MapList.ini`.

| Nhóm | Số đo | Nhận xét |
|---|---|---|
| 9 thư mục tên Hán (GBK): 东北区, 江南区, 两湖区, 西北北区, 西北南区, 西南北区, 西南南区, 中原北区, 中原南区 (+ thư mục con Hán trong `global`, `skill`) | **1.513 tệp = 47 % số tệp**, chỉ 0,4 MB | Di sản JX gốc (hộp thư, rương, bảng thông báo, trap chuyển map). **Chỉ 10 tệp còn được trap map dùng** (`中原北区`, 407 ô trap toàn máy chủ chỉ trỏ 28 script). **1.503 tệp không ai gọi** nhưng engine vẫn tạo 1.503 `lua_State` lúc boot ≈ 190 MB RAM + ~1/3 thời gian nạp. |
| Không được tham chiếu từ đâu (chỉ autoload) | 1.746 tệp; ngoài Hán còn ~240: `skill/` 158, `scriptjx2/tong` 67 (bản cũ của `tong_vn`), `scriptjx2/lib` 37, `global` 38, `missions` 30, `task` 24, `maps` 19 | Có thể được gọi ĐỘNG (`g_GetScript` theo tên ghép chuỗi, skill theo cột `skills.txt` …) → không xoá máy móc, phải rà tay từng nhóm. |
| Test/admin/backup nằm trong cây chạy thật | 32 tệp: `test/` 7; `*_admin.lua` 10 (dùng thật qua vật phẩm GM); `item/event/copy of 30thang4/` 4; `startgame/monster/trainingmap - copy.lua`; `tmpbug90/6079/8089.lua`; `task_debug.lua`; `scriptjx2/lib/debug.lua`, `unittest.lua` | |
| Trùng nội dung (cùng sha1) | 67 nhóm, 101 tệp thừa, 218 KB: `npclevelscript/dragonboat_* ↔ randomtask_*`, `scriptjx2/tong ↔ tong_vn` (12 cặp), `script/lib ↔ scriptjx2/lib` 15 cặp cùng tên (10 giống hệt, **5 KHÁC**: awardtemplet, composeclass, getrectangle_point, objbuffer_head, progressbar) | C++ `sJX2RemapScriptPath` chỉ dùng bản jx2 khi `script/lib` THIẾU → hai bản sống song song, ai Include tên nào được bản đó. |
| Gốc `script/` | 10 tệp: `codenew.lua` 32.000 dòng, `giftcode_new/fancung.lua` 9.000 dòng/tệp, `timerserver`, `protocol`, `startgame`, `gmscript`, `gb_taskfuncs`, `timerserver_ctc` | dữ liệu khổng lồ trộn với mã điều khiển |
| Ngôn ngữ tên | 3 thứ tiếng ở 50 thư mục cấp 1: Hán (9), pinyin (`qianbaoku`, `yandibaozang`, `huoyuedu`, `leaguematch`, `npclevelscript`…), Việt không dấu (`tinhnang`, `cauhinh`, `npcthon`, `songbac`, `chienlenh`), Anh (`missions`, `event`, `global`, `battles`) | |
| Tham chiếu tới tệp KHÔNG tồn tại | `MapList.ini` 1.966 dòng → `maps/newworldscript.lua`; `item/noscript.lua` 334; `skill/7zhuanjineng.lua` 325; `skill/zhounianqing2012/…` 260; 105 dòng `Include`; tổng > 4.000 | phần lớn dữ liệu gốc chưa port, hỏng im lặng |
| Bản lưu lẫn trong `bin\server` | `script.lua4`, `scriptjx2.lua4`, `script_go_bo_3hd`, `_backup_*` ×5, `_moi_backup` (80 MB), `_x86_khong_nap_duoc_2008`, `_dara`, `_gobo_lien_dau_20260820`; `settings\item\magicscript.txt.truoc_*` ×20; `MapList.ini.truoc_*` ×3; `gamesetting.ini.truoc_*` ×3; `DumpInfo` 12 GB | |
| Gương git | `serverscript_jx2/` = 35 thư mục theo tính năng/ngày (`3hoatdong`, `tinsu_fix_2508`, `tong_fix_2508`, `lua54_toiuu`, `vantieu`, `taithiet_2908`…), 1.001 tệp = ảnh chụp từng đợt. **Cây live 10,2 MB không có trong git** → không diff/blame/lùi được, sửa nóng của nhiều phiên đè nhau không dấu vết. | rủi ro lớn nhất |

## 2. Vì sao không thể `mv` máy móc

1. `Include`/`dofile` = chuỗi đường dẫn tuyệt đối trong 2.839 dòng script.
2. Dữ liệu `settings` (npcs, items, skills, magicscript, task, MapList…) ghi đường dẫn; engine **băm đường dẫn thành ID** rồi tra `g_GetScript(ID)` → đổi đường dẫn = đổi ID → mọi chỗ tra ID cũ hỏng **im lặng**.
3. Trap trong 5.008 `Region_S.dat` lưu ID băm (nhị phân, không sửa tay) → đổi tên script trap = trap chết.
4. C++ ghi cứng: `\script\timerserver.lua`, `\script\lib\…`, `scriptjx2\tong_vn`, 20 tiền tố `g_IsJx2Script` (KSortScript.cpp:120-160, quyết định ngữ nghĩa hàm JX2), `sJX2RemapScriptPath`, `KSortScript` chỉ nạp `\script` + `\scriptjx2\tong_vn`.
5. Tên GBK phải giữ nguyên byte; thư mục có dấu cách (`copy of 30thang4`).

## 3. Lộ trình đề xuất (từ không rủi ro tới cần engine)

| Bước | Việc | Được gì | Rủi ro |
|---|---|---|---|
| **R0** | Đưa cây live vào git nguyên trạng: `serverscript_live\script`, `serverscript_live\scriptjx2` (robocopy /MIR từ `bin\server`), commit theo ngày; script `dongbo_script_live.py` 2 chiều có kiểm diff; đóng băng `serverscript_jx2` làm tư liệu | lịch sử, diff, lùi từng tệp; các phiên sửa nóng không đè nhau mù | 0 (không đụng máy chủ) |
| **R1** | Dọn ngoài cây script: chuyển `_backup_*`, `_moi_backup`, `_x86…`, `_dara`, `_gobo…`, `script_go_bo_3hd`, `settings\*.truoc_*`, `DumpInfo` cũ vào `bin\_luutru\<ngày>\`; `script.lua4`/`scriptjx2.lua4` giữ tới khi chủ bỏ đường lùi Lua 4 (`LuiLua4.bat`) | thư mục máy chủ đọc được | 0 logic (chỉ bat lùi Lua 4 phụ thuộc) |
| **R2** | **Bí danh đường dẫn trong engine** (C++ ≈ 60 dòng, 1 build): tệp `script\_duongdan_cu.txt` dòng `cũ=mới`; (a) `KSortScript` sau `LoadAllScript` đăng ký thêm node **ID(cũ) → cùng script index** (trap/npcs/settings/ID cũ vẫn chạy, không sửa nhị phân); (b) `LuaIncludeFile` khi tệp thiếu → tra bí danh (như remap JX2 hiện nay); (c) shim `dofile` tra cùng bảng (C++ đẩy vào state lúc `RegisterFunctions`) | mọi bước di chuyển sau đều lùi được bằng cách xoá dòng bí danh; không cần sửa dữ liệu | thấp, kiểm bằng `boot_gia.py` (tập lỗi `_ALERT` trước/sau phải giống) |
| **R3-1** | 1.503 tệp Hán không tham chiếu → `script\_disan_jx\` (KSortScript bỏ thư mục bắt đầu `_`, hoặc ra ngoài `script`); 10 tệp trap còn dùng đổi tên pinyin + bí danh | −1.503 state ≈ −190 MB RAM, boot nhanh thêm ~1/3 | thấp (0 tham chiếu từ 5 nguồn) |
| **R3-2** | Gộp `scriptjx2\lib` vào `script\lib` (5 tệp KHÁC hợp nhất tay), bỏ remap; `scriptjx2\tong` (bản cũ) → `_disan_jx`; xoá 101 tệp trùng | hết hai bản song song | trung bình (5 tệp khác nội dung) |
| **R3-3** | Bố cục cấp 1 theo miền, một ngôn ngữ (đề nghị Việt không dấu như `tinhnang/`, `cauhinh/`): `he_thong/` (timerserver, protocol, startgame, lib, header, class, script_protocol) · `nhan_vat/` (player, skill, petsys, partner) · `vat_pham/` (item, mail, auction_house, chienlenh) · `hoat_dong/` (missions, event, tinhnang, battles, leaguematch, songbac, vng_*, activitysys, nationalwar) · `nhiem_vu/` (task, dailogsys, npcthon, npcthanhthi, timertask) · `du_lieu/` (codenew, giftcode_*, npcpos, skills_table) · `admin_test/` (`*_admin`, `test/`) — mỗi đợt một miền, bí danh R2 giữ đường cũ, cập nhật 20 tiền tố `g_IsJx2Script` + C++ ghi cứng | cây đọc được, tìm được | trung bình; cần vài ngày + test từng miền |
| **R3-4** | Tách dữ liệu khỏi mã: `codenew.lua` 32.000 dòng, `giftcode_*` → bảng txt đọc bằng `ktabfile`/`KTabFile`; `npcpos.lua` 27.000 dòng → bảng | sửa dữ liệu không đụng mã | trung bình |

## 4. Câu hỏi cần chủ trả lời trước khi làm

1. **Ngôn ngữ tên thư mục** cho R3-3: Việt không dấu / tiếng Anh / giữ tên cũ, chỉ dọn (R0–R3-2)?
2. **1.503 tệp Hán không tham chiếu**: chuyển sang `_disan_jx` (không nạp, còn trên đĩa) hay xoá hẳn (có git R0 nên lùi được)?
3. **Bỏ đường lùi Lua 4** (`script.lua4`, `scriptjx2.lua4`, `LuiLua4.bat`) chưa? Bản `.lua4` đã lạc hậu 1 ngày sửa nóng.
4. **Đưa toàn bộ cây live vào git** (R0, +3.216 tệp / 10 MB vào repo GAMEDEVNEW hoặc repo riêng)?
5. **Mức tham vọng**: R0 + R1 + R2 + R3-1 (an toàn, ~1 buổi, lợi RAM/boot ngay) hay tới R3-3/R3-4 (đổi bố cục, vài ngày)?

Trả lời xong tôi làm R0 → R1 → R2 (một build CoreServer) → R3-1, mỗi bước kiểm bằng `boot_gia.py` + `kiem_54.py` và có `.moi`/bat lùi.

## 5. ĐÃ LÀM 06/09 15:30–16:05 (chủ chốt: tên Việt không dấu; xoá tệp Hán không dùng sau khi kiểm kỹ; bỏ đường lùi Lua 4; làm hết trong buổi)

| Bước | Trạng thái | Chi tiết |
|---|---|---|
| R0 git | **xong** | `serverscript_live/` = ảnh chụp nguyên trạng trước sắp xếp (commit 84675cc3, 3.396 tệp). Sau khi chạy bat bước cuối, gương được robocopy /MIR lại → tôi commit "sau sắp xếp". |
| R1 dọn | **xong** | 150 mục vào `bin\_luutru\0609\` (nhật ký `NHATKY_LUUTRU_0609.txt`): `_backup_*` ×5, `_moi_backup`, `_x86…`, `_dara`, `_gobo…`, `script_go_bo_3hd`, 124 tệp `settings/Maps *.truoc*/*.goc`; **đường lùi Lua 4 đã bỏ**: `script.lua4`, `scriptjx2.lua4`, `LuiLua4.bat`, `lualibdll.*`, `ChayGameServer.bat.truoc/.v2` (server) + `script.lua4`, `lualibdll.dll` (client) → `_luutru\0609\server\lua4`, `client\lua4`. `DumpInfo` 12 GB để nguyên. |
| R3-1 Hán chết | **xong** | Kiểm kỹ 5 nguồn (Include/dofile chuỗi, settings 73 tệp, C++ 104 tệp, trap ID băm 5.008 `Region_S.dat`, MapList): 1.507 tệp Hán → **136 còn tham chiếu GIỮ** (71 `skill/partner`, 26 `skill/npc`, 5 saolin, 2 emei, 2 chuiyang, 2 baiming = kỹ năng tên Hán trong `skills.txt`; 10 trap `中原北区`; 2 trap khác; 1 `global/…`), **1.371 tệp không ai gọi → `bin\_luutru\0609\disan_jx\`** (xoá khỏi cây chạy, còn trên đĩa + trong git R0; xoá hẳn khi chủ muốn). Cây còn **1.852 tệp** (từ 3.220). |
| R3-2 jx2 | **xong** | `scriptjx2/tong` (67 tệp bản cũ, 0 tham chiếu) → `_luutru\0609\server\scriptjx2_tong_cu`; 10 tệp `scriptjx2/lib` giống hệt `script/lib` → `_luutru\0609\server\scriptjx2_lib_trung` (remap chỉ dùng jx2 khi script/lib thiếu; 0 tham chiếu tường minh). 5 tệp KHÁC nội dung giữ lại (awardtemplet, composeclass, getrectangle_point, objbuffer_head, progressbar). |
| R2 bí danh | **xong, chờ restart** | `Lua54Dll` + `KSortScript`: tệp `script\_duongdan_cu.txt` (dòng `--@ cũ=mới`, cả tệp là chú thích Lua hợp lệ) → Include/dofile tra bí danh khi tệp gốc không có; ID băm cũ đăng ký trỏ cùng script (trap, settings, `g_GetScript` tên cũ); `g_IsJx2Script` xét cả tên cũ (cache theo state); engine bỏ qua tên bắt đầu `_`. Selftest + `test_alias.py` OK (bắt được 1 lỗi khoảng trắng đầu dòng). Commit origin/main **47569764**. `.moi` đã đặt: `CoreServer.dll.moi` **6ba06754**, `Lua54Dll.dll.moi` 7689d830 (server), `Lua54Dll.dll.moi` b9467bf6 (client). |
| R3-3 bố cục | **sẵn sàng, CHỜ CHỦ TẮT SERVER** | Kế hoạch 1.419 tệp (`tools\sapxep\r33_kehoach.txt`), 0 va chạm. Vì 30 tệp nằm trong chuỗi nạp nóng của `timerserver.lua` và `activitysys/functionlib` Include lúc chạy, **chỉ đổi chỗ khi server đã tắt**. |

### Bố cục mới (tiếng Việt không dấu, giữ tên thư mục con)

| Mới | Từ | Số tệp |
|---|---|---:|
| `nhanvat/kynang`, `nhanvat/nguoichoi`, `nhanvat/thucung`, `nhanvat/donghanh` | skill, player, petsys, partner | 367 + 23 + 28 + 5 |
| `vatpham/…` (lên thẳng) | item (trừ 11 tệp admin) | 245 |
| `giaodich/thu`, `giaodich/daugia` | mail, auction_house | 4 + 2 |
| `tinhnang/phuban`, `sukien`, `chientruong`, `liendau`, `songbac`, `vng_event`, `vng_feature`, `activitysys`, `nationalwar`, `huoyuedu`, `baucua`, `bonusvlmc`, `congthanh`, `chienlenh`, `trapcu/…` | missions, event, battles, leaguematch, songbac, vng_*, activitysys, nationalwar, huoyuedu, baucua, bonusvlmc, mission, chienlenh, 3 thư mục Hán còn trap | 309 + 93 + 9 + 9 + 5 + 8 + 4 + 35 + 1 + 1 + 1 + 1 + 2 + 4 + 12 |
| `nhiemvu/…` (lên thẳng), `nhiemvu/hoithoai`, `nhiemvu/tanthu` | task, dailogsys, tagnewplayer | 201 + 3 + 1 |
| `npc/…` (lên thẳng), `npc/thanhthi`, `npc/khac` | npcthon, npcthanhthi, npckhac | 18 + 3 + 1 |
| `dulieu/` | codenew.lua, giftcode_new.lua, giftcode_fancung.lua | 3 |
| `kiemthu/` | test/, 11 `item/*_admin.lua`, `partner_test_bdh`, `task_debug`, `copy of 30thang4` | 24 |
| **Giữ ở gốc (hệ thống, C++ nạp trực tiếp)** | lib, header, cauhinh, cauhinh_web, class, script_protocol, startgame, global, misc, maps, log_game, vng_lib, npclevelscript, timertask, timerserver*.lua, protocol.lua, startgame.lua, gmscript.lua, gb_taskfuncs.lua | — |

### Cách chạy bước cuối (chủ làm, ~1 phút)

1. Tắt GameServer.
2. Chạy `bin\server\SAPXEP_0609.bat`: tự thay `CoreServer.dll`/`Lua54Dll.dll` bằng `.moi` → đổi chỗ 1.419 tệp + sinh `script\_duongdan_cu.txt` → `kiem_duongdan_cu.py sua` (tệp phiên khác vừa ghi ở đường cũ được dời sang mới) → mô phỏng boot cây mới và so tập lỗi với trước (`so_boot.py`; phải "chi co SAU: 0") → robocopy gương git → mở GameServer.
3. Trên console phải thấy `[script] LoadAllScript: … tep, … ms` và **`[script] Bi danh duong dan: 1419 dong, dang ky ID cu 1419, ten moi chua nap 0, ten cu con ton tai 0`**. Kiểm nhanh: trap chuyển map hoạt động, NPC vật phẩm/kỹ năng chạy, `ScriptError.log` (nằm trong từng thư mục script) không có lỗi mới.
4. Client: `ChoiGame.bat` nhận `Lua54Dll.dll.moi` (cây script client không đổi).

**Lùi:** tắt server → `python tools\sapxep\r33_lui.py` (dời tệp về đường dẫn cũ theo bí danh) → mở lại. `.moi.truoc` của CoreServer/Lua54Dll vẫn giữ.

**Lưu ý cho các phiên khác:** từ giờ ghi tệp theo đường dẫn MỚI; ghi nhầm vào đường cũ vẫn chạy được (bí danh) nhưng tạo hai bản → chạy `kiem_duongdan_cu.py sua` trước mỗi lần restart. Muốn thêm bí danh tay: thêm dòng `--@ script\cu.lua=script\moi.lua` vào `script\_duongdan_cu.txt`.

### Hai lỗi đỏ lúc boot 15:39/15:44 (không do cache) — đã chặn
- `event\longmenbiaoju\tasknpc.lua:64`: `AutoFunctions` chỉ có trong state `global/autoexec_head.lua` (JX1 mỗi tệp một state) → chèn `if AutoFunctions ~= nil then … end`.
- `global\achievementsys\type\longmenbiaoju.lua:7`: Include `achievementsys\head.lua` không tồn tại ở JX1 (không có hệ thành tựu) → `if AchievementDetailBase == nil then return end`. Cả hai là tệp vận tiêu 13:51 của phiên khác; sửa chỉ chặn lỗi, không đổi logic.
