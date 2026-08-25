# 16 — PHÊ BÌNH TÍNH ĐẦY ĐỦ của 3 báo cáo tính năng (01 / 02 / 03)

**Hướng quét 7.** Mục tiêu: **tìm cái CHƯA ĐƯỢC NHẮC TỚI**, không viết lại nội dung đã có.
Phương pháp: kiểm kê cơ học bằng script (định nghĩa hàm / lời gọi / bảng toàn cục / hằng số) trên
**đúng bộ tệp hạt giống** của từng tính năng, rồi đối chiếu từng tên với văn bản báo cáo
(`grep` chuỗi nguyên văn trong `01_satthu.md` / `02_phonglangdo.md` / `03_vuotai.md`).
Mọi mục dưới đây **đã được mở tệp gốc xác nhận lại bằng tay** — các trùng lặp với thân bài
hoặc với phụ lục phản biện của chính báo cáo đó **đã bị loại bỏ** trước khi ghi vào đây.

Script kiểm kê: `%TEMP%\claude\…\scratchpad\v7\{inv.py, cmp.py, badenc.py, mixenc.py, cmtcode.py, datebr.py}`
(tệp tạm, không ghi vào cây dự án).

Bộ tệp hạt giống dùng để đếm:

| Tính năng | Số tệp | Tổng dòng | `function` tìm thấy | Tên hàm được gọi | Bảng `x = {` | Hằng số | Dòng comment |
|---|---|---|---|---|---|---|---|
| 01 Sát Thủ | 7 | 3 998 | 32 (7 bị comment) | 71 | 12 | 11 | 3 242 |
| 02 Phong Lăng Độ | 13 | 832 | 34 | 103 | 10 | 15 | 40 |
| 03 Vượt Ải | 17 | 2 543 | 106 | 219 | 33 | 78 | 278 |

---

## PHẦN A — BÁO CÁO `01_satthu.md`: **10 mục bị bỏ sót**

| # | Mục bị bỏ sót | Bằng chứng | Mức độ quan trọng |
|---|---|---|---|
| A1 | **Cú pháp upvalue Lua 4.0 `%tên` — 3 dòng trong `shashou_mibao.lua`.** `local n_XuanTian_Chui = 6` (`:13`) rồi dùng `%n_XuanTian_Chui` ở `:17, :18, :21`. Báo cáo chỉ ghi "n_XuanTian_Chui = 6", **không nói tới toán tử `%`**. Đây KHÔNG phải lỗi cú pháp: `Sources\Library\LuaLib\src\lparser.c:229-242 pushupvalue()` chấp nhận cả biến **toàn cục** lẫn local của hàm bao ngoài; nhưng giá trị bị **đóng băng tại thời điểm nạp tệp** (`OP_PUSHUPVALUE` chạy khi `OP_CLOSURE` thực thi, `lparser.c:295-300`). ⇒ khi port, **thứ tự `Include` quyết định giá trị**, và gán lại biến sau đó **không có tác dụng**. | `shashou_mibao.lua:13,17,18,21`; `lparser.c:229-242, 295-300` | 🔴 **CAO** — cả 6 báo cáo (00–06) đều có **0 lần** nhắc chữ "upvalue" hay "`%tb`" |
| A2 | **`myChangeMember` là biến chết VÀ đặc tả trả về của `SetMemberTask` không được thực hiện.** Chú thích đầu tệp (`lib_killlevel.lua:8-13`) quy định 4 mã trả về: `0` (chỉ đổi biến người đánh đơn), `>=1` (số đồng đội được đổi), `80` (không ai được đổi), `110` (bất thường). Thân hàm `:21-68` **không có một lệnh `return` nào**; `myChangeMember` (`:26`) chỉ được `+1` ở `:57` rồi vứt. Báo cáo có ghi `Uworld1217` là mã chết nhưng **bỏ qua cả hai điểm này**. | `lib_killlevel.lua:8-13, :26, :57`, không có `return` trong `:21-68` | 🟡 TRUNG BÌNH — nếu port "theo đặc tả comment" sẽ viết sai ngữ nghĩa |
| A3 | **3 mã điểm thống kê `AddStatData` của `jiefangri_award` không được liệt kê.** Bảng `tbMaiDian` (`lib_killlevel.lua:134-138`) = `{"jiefangri_shashouchanchuzhangongjiangzhang", "jiefangri_shashouchanchuzhibi", "jiefangri_shashouchanchujianzhang"}`, gọi ở `:145` `AddStatData(tbMaiDian[i], tbshashou[i])` với số lượng `tbshashou = {2,1,2}` (`:123-127`). `grep "jiefangri_shashou"` trong `01_satthu.md` = **0**. | `lib_killlevel.lua:123-127, 134-138, :145` | 🟢 THẤP (thống kê) nhưng là **dữ liệu thiếu** khi dựng lại bảng |
| A4 | **Dòng `--EventSys:GetType("KillerBoss"):OnTeamEvent("OnFinish", …)` bị comment ở `kill_level.lua:73`.** Đây là **bằng chứng thiết kế gốc**: phần thưởng "OnFinish" (bảng 25 dòng + 10 000 000 exp, `kill_level.lua:89-119`) vốn dành cho **CẢ ĐỘI** (`OnTeamEvent`), đã bị thay bằng `OnPlayerEvent` (`:74`) ⇒ **chỉ người ra đòn cuối** ăn. `grep "OnTeamEvent"` trong `01_satthu.md` = **0**. Kết hợp với §8.2 (bản Linux **không có** `OnPlayerEvent`) ⇒ trong bản Linux, cả `OnTeamEvent` lẫn `OnPlayerEvent` đều không tồn tại. | `kill_level.lua:73` (comment) vs `:74` (đang chạy); `eventsys.lua` Linux chỉ có 6 hàm | 🔴 **CAO** — quyết định "thưởng cá nhân hay cả đội" khi port |
| A5 | **Tham số thứ 5 của `ITEM_DropRateItem` đứt chuỗi ở cấp 90.** 8 lời gọi dùng dãy tăng dần `4, 5, 6, 7, 8, 9, 10` cho `lev20…lev80` (`kill_level.lua:47-65`, đã comment) rồi **`10` một lần nữa** cho `lev90` (`:69`) — đáng lẽ phải là `11`. Báo cáo ghi chữ ký `(nNpcIdx, nCount, szIniPath, ?, ?, nSeries)` với 2 dấu `?` mà không nêu quy luật này. **CHƯA XÁC MINH** ý nghĩa tham số, nhưng dãy số là sự thật đọc được. | `kill_level.lua:47,50,53,56,59,62,65` (=4..10) vs `:69` (=10) | 🟡 TRUNG BÌNH — nghi lỗi sao-chép của bản gốc |
| A6 | **Bản giải mã `src_utf8` của chính dự án ĐANG SAI ở 3 dòng của hệ này.** `nieshichen.lua:21` (`ContentList[1]`, lời giới thiệu NPC, 292 byte), `nieshichen.lua:48` (`ContentList[28]`, **toàn bộ luật Vượt Ải**, 896 byte), `mibao_head.lua:21` (tên vật phẩm 6,1,1781). Byte gốc là **TCVN3** nhưng `gbktool` chọn nhánh GBK ⇒ ra chữ Hán vụn. Giải lại bằng `cp1258`/`latin-1` cho đúng tiếng Việt: `"NÕu ai cã ®ñ 160 s¸t thñ gi¶n…"`, `"Cø mçi giê hÖ thèng sÏ th«ng b¸o 1 lÇn…"`. Báo cáo có cảnh báo mã hoá chung (§9.6) nhưng **không biết bản trích xuất đang dùng đã hỏng ở đâu**. | Đọc byte thô: `nieshichen.lua` dòng 21/48, `mibao_head.lua` dòng 21; so 3 bảng mã | 🔴 **CAO** — ai port từ `src_utf8` sẽ chép nguyên văn bản rác |
| A7 | **Rò rỉ biến toàn cục ở 4 chỗ.** `givesword()` gán `series`, `i`, `j` **không có `local`** (`nieshichen.lua:202,203,205`); `exchange_token()` gán `itemgenre, detailtype, parttype, level, attribute` không `local` (`:167`); `main()` gán `UWorld1082` toàn cục (`:57`) mà `havetask()` (`:136`) đọc lại ⇒ giá trị **cũ** nếu gọi `havetask` từ đường khác; `add_killertasknpc/add_bossnpc/add_messengernpc` gán `Tid, TabValue4, TabValue5, newtasknpcindex` toàn cục (`killbosshead.lua:185-191, 3395-3401, 3408-3411`). | `nieshichen.lua:57,167,202,203,205`; `killbosshead.lua:185-191` | 🟡 TRUNG BÌNH — va chạm tên khi chạy chung một máy ảo Lua với hệ khác |
| A8 | **`killertabfile = new(KTabFile, …)` chạy ở CẤP TỆP, không nằm trong hàm** (`nieshichen.lua:54`). Nghĩa là bảng `\settings\task\tollgate\killer\killer.txt` bị **nạp ngay lúc `Include`**, mỗi lần Include lại tạo một đối tượng mới. Báo cáo có nêu tệp `killer.txt` và lớp `KTabFile` nhưng **không nêu thời điểm nạp** — điều quyết định khi port sang JX1 (nơi `Include` có bộ nhớ đệm khác). | `nieshichen.lua:54` (dòng lệnh top-level, ngoài mọi `function`) | 🟡 TRUNG BÌNH |
| A9 | **`tbChangeDestiny:completeMission_Killer()` được gọi KHÔNG THAM SỐ** (`kill_level.lua:68`) trong khi khai báo là `completeMission_Killer(tbPlayers)` (`event\change_destiny\mission.lua:80`). Thân hàm may mắn không dùng `tbPlayers` nên vô hại, nhưng §6.2 của báo cáo ghi chữ ký có tham số mà không nói điểm gọi bỏ trống. | `kill_level.lua:68` vs `change_destiny\mission.lua:80` | 🟢 THẤP |
| A10 | **🔴🔴 `killbosshead.lua` chỉ còn 243/3421 dòng SỐNG — §4.1 của báo cáo đánh dấu thiếu 4 bảng bị comment.** Đếm bằng máy (dòng không rỗng và không mở đầu bằng `--`): **243 dòng sống / 3 144 dòng comment / 3 421 tổng**. Bảng đối chiếu đúng: xem ngay dưới bảng này. Hệ quả nặng nhất: **`AddNpc_allbugbear` là bảng RỖNG** (`:365` `AddNpc_allbugbear=`, `:366` `{`, **2 239 dòng comment**, `:2606` `}`) ⇒ `addtollgatenpc.lua:19 add_messengernpc(AddNpc_allbugbear)` chạy `for i = 1, getn({}) = 0` ⇒ **sinh 0 NPC**. Tổng NPC mà `add_alltollgatenpc()` thực sự sinh = **9 + 9 + 0 = 18**, không phải "3 bảng NPC ải" như §6.1 gợi ý. | Quét dải sống/chết trên `killbosshead.lua`; đọc trực tiếp `:364-370`, `:2603-2612`, `:2918-2934`, `:3376-3392` (đều có dòng mốc `--只保留9个` = "chỉ giữ lại 9 cái") | 🔴 **RẤT CAO** |

**Bảng SỬA cho §4.1 của `01_satthu.md` (kèm A10)** — cột "Số dòng dữ liệu SỐNG" là kết quả đếm lại:

| Dòng | Bảng | §4.1 ghi | Số dòng dữ liệu SỐNG (đếm lại) | Nhận xét |
|---|---|---|---|---|
| 4–180 | `addkillertasknpc` | Hệ SÁT THỦ | **160 / 160** | ✅ đúng |
| 200–361 | `AddNpc_flyboss` | "comment hết" | **0** (162 dòng comment) | ✅ đúng — bảng **RỖNG** |
| 365–2606 | `AddNpc_allbugbear` | "Tín Sứ" | **0** (2 239 dòng comment) | 🔴 §4.1 **KHÔNG đánh dấu** — bảng **RỖNG** |
| 2608–2932 | `AddNpc_turesurebug` | "Tín Sứ (Thiên Bảo Khố)" | **9** (`:2922-2930`, 311 dòng comment) | 🔴 §4.1 **KHÔNG đánh dấu** — dòng mốc `--只保留9个` ở `:2921` |
| 2936–2953 | `AddNpc_flypoint6079` | "comment" | **0** | ✅ đúng |
| 2956–2974 | `AddNpc_flypoint8089` | "comment" | **0** | ✅ đúng |
| 2977–2995 | `AddNpc_flypoint90` | "comment" | **0** | ✅ đúng |
| 2999–3067 | `AddNpc_templeboss` | "Sơn Thần Miếu" | **0** (67 dòng comment) | 🔴 §4.1 **KHÔNG đánh dấu** — bảng **RỖNG** |
| 3071–3390 | `AddNpc_turesureboss` | "9 Bảo Rương ×3 cấp" | **9** (`:3380-3388`, 307 dòng comment) | 🔴 Thực tế **9 dòng**, KHÔNG phải 9×3 = 27 |

*(Bổ sung: `SHOUHUZHE_OFFSET = 2` — hằng ở `:1` mà §4.1 ghi "không dùng ở hệ sát thủ" — **có dùng thật**
ở đúng 9 dòng sống `:2922-2930`, dịch Bảo Khố Thủ Hộ Giả sang 2 ô so với Bảo Rương cùng chỉ số.)*

**Không phải bỏ sót (đã kiểm, báo cáo 01 CÓ):** `killer20..killer80` bị comment · lỗi `tinsert(tbDialog, 12, …)`
ngày sinh nhật · 7 nhánh `ITEM_DropRateItem` cấp 20-80 bị comment · `Uworld1217` mã chết · `tbParam={60}` của
vật phẩm 1781 · ladder 10119 xung đột · 1192/1193 bị `lenhbaitanthu.lua`/`baivip.lua` ghi đè.

---

## PHẦN B — BÁO CÁO `02_phonglangdo.md`: **10 mục bị bỏ sót**

Báo cáo 02 đã qua một vòng phản biện rất kỹ (15 mục B1-B15). Các mục dưới đây **không trùng** với phụ lục đó.

| # | Mục bị bỏ sót | Bằng chứng | Mức độ quan trọng |
|---|---|---|---|
| B1 | **`OnLeave` phát THÔNG BÁO TỬ VONG cho mọi người trong mission, mỗi lần BẤT KỲ AI rời mission.** `mission.lua:68` `Msg2MSAll(MISSIONID, GetName().."Bạn không may tử vong trong lúc đi thuyền.")`. `OnLeave` là callback engine gọi khi rời mission **vì bất cứ lý do gì** — kể cả `DelMSPlayer` lúc cập bến và lúc thoát game. §6.2 của báo cáo liệt kê đủ 6 lệnh khôi phục trạng thái của `OnLeave` nhưng **bỏ hẳn dòng `Msg2MSAll` này**. Cộng thêm: `fld_death.lua:4` đã `Msg2Player` rồi `:12 DelMSPlayer` ⇒ người chết nhận **2 thông báo**. | `mission.lua:65-77` (đặc biệt `:68`); `fld_death.lua:4,12` | 🔴 **CAO** — hành vi người chơi nhìn thấy, spam khi cập bến |
| B2 | **`SetTaskTemp(200, 0)` bị comment ở CẢ HAI chỗ khôi phục ⇒ cờ tạm 200 KHÔNG BAO GIỜ được xoá.** `mission.lua:71` `--	SetTaskTemp(200,0);` (trong `OnLeave`) và `mission.lua:116` `--		SetTaskTemp(200, 0)` (trong `Landing`). Trong khi `fld_head.lua:117` **có** đặt `SetTaskTemp(200,1)` ở khung giờ tốn phí. Báo cáo có nêu `SetTaskTemp(200,1)` 3 lần nhưng **không nêu đường xoá bị tắt**. Biến tạm 200 = "cho phép đổi phe, dùng cho PK" ⇒ người chơi giữ cờ này sau khi rời thuyền. | `fld_head.lua:117` (đặt) vs `mission.lua:71, :116` (xoá — comment) | 🔴 **CAO** — lỗ hổng trạng thái PK mang ra ngoài hoạt động |
| B3 | **`RunMission()` gọi `SetFightState(1)` và `PutMessage(...)` NGOÀI khối bảo vệ `if (pidx > 0)`.** `mission.lua:27-39`: `PlayerIndex = pidx` chỉ được gán khi `pidx > 0` (`:30-32`), nhưng `:34` `SetFightState(1)` và `:35 PutMessage(...)` chạy **vô điều kiện** ⇒ khi gặp một khe rỗng, lệnh áp lại lên **người chơi của vòng lặp trước** (nhận thông báo 2 lần, bật chiến đấu 2 lần). Ngoài ra `PlayerIndex` **không bao giờ được khôi phục** sau vòng lặp (khác `Landing()` cũng không khôi phục). `grep "pidx"` trong `02_phonglangdo.md` = **0**. | `mission.lua:25-41` | 🟡 TRUNG BÌNH |
| B4 | **`Landing()` dùng `tbPlayer[i] = pidx` có lỗ hổng chỉ số rồi duyệt bằng `getn(tbPlayer)`.** `mission.lua:91-102`: nếu một `pidx <= 0` xuất hiện giữa danh sách thì `tbPlayer` bị **thủng** ở chỉ số đó. **[đã sửa theo đối chất]** — cơ chế ghi ban đầu ("`getn()` dừng ở phần tử `nil` đầu tiên ⇒ mọi người phía sau lỗ hổng không được cập bến") là **SAI**: trong Lua 4.0 `lua_getn` trả `t.n` nếu là số, **ngược lại trả KHOÁ SỐ LỚN NHẤT** (`Sources\Library\LuaLib\src\lapi.c:458-476`; xác nhận trên chính ELF Linux tại `lua_getn` **0x08233070** — vòng quét node stride `0x1c` giữ `max`, hằng chuỗi `"n"` ở `0x8266214`), và hàm dựng `{}` **không hề đặt `n`** (`lparser.c:574-593`, `constructor_part` cho `cd->n = 0` nên không phát `OP_SETLIST`). ⇒ Hậu quả THẬT: vòng `:110` chạy đủ `1..chỉ-số-lớn-nhất`, **không ai bị bỏ qua**, nhưng đúng ở vòng lặp trúng lỗ hổng thì `PlayerIndex = nil` rồi vẫn gọi `GetCamp()/SetCurCamp()/SetCreateTeam()/NewWorld()`… với `PlayerIndex` không hợp lệ. Vẫn là lỗi cùng họ với B3, nhưng kiểu hỏng khác hẳn. | `mission.lua:94-96` (gán trong `if`) vs `:110` (`for i = 1, getn(tbPlayer)`); `lapi.c:458-476`; ELF `0x08233070` | 🟡 TRUNG BÌNH (hạ từ 🔴 CAO) |
| B5 | **Hai hàm script bắt buộc phải port mà bảng hàm §6.4 không có: `PlayerFunLib:CheckTaskDaily` và `PlayerFunLib:AddTaskDaily`.** `fld_head.lua:121-122` dùng cả hai để cấp Chân Nguyên Đơn (task 3070, trần 5/ngày). Báo cáo mô tả **hiệu ứng** (mục §5, task 3070) nhưng hai tên hàm này `grep` = **0** trong cả tệp ⇒ danh sách phụ thuộc hàm bị thiếu 2 mục. | `fld_head.lua:121, :122`; định nghĩa ở `activitysys\playerfunlib.lua` | 🟡 TRUNG BÌNH |
| B6 | **Cú pháp upvalue Lua 4.0 `%tbTalkDailyTask` ở `fld_smalltimer.lua:13`** — `%tbTalkDailyTask:AddTalkNpc(...)`, với `tbTalkDailyTask` là biến **toàn cục** của `activitysys\config\32\talkdailytask.lua`. Xem A1: hợp lệ nhưng **đóng băng giá trị lúc nạp `fld_smalltimer.lua`** ⇒ nếu `Include("…\config\32\talkdailytask.lua")` chưa chạy trước thì upvalue = `nil` **vĩnh viễn**, NPC nhiệm vụ ngày "Tiểu Chiêu" không bao giờ sinh. | `fld_smalltimer.lua:2` (Include) và `:13` (dùng `%`); `lparser.c:229-242` | 🔴 **CAO** |
| B7 | **`AddMSPlayer` dùng nhóm 1 nhưng `DelMSPlayer` dùng nhóm 0.** `fld_head.lua:138` `AddMSPlayer(MISSIONID, 1)`; `fld_death.lua:12` `DelMSPlayer(MISSIONID, 0)`. Báo cáo có bàn kỹ chuyện nhóm 0 = "tất cả" khi **duyệt**, nhưng **không đối chiếu cặp Add(1)/Del(0)** — nếu engine JX1 hiểu tham số 2 là nhóm cần khớp thì người chết **không bị gỡ khỏi mission**. | `fld_head.lua:138` vs `fld_death.lua:12`; đối chiếu `fld_haveroom` `:153` dùng nhóm **1** | 🟡 TRUNG BÌNH |
| B8 | **`shuizeideath.lua:6-8` giữ nguyên 3 dòng comment của công thức tỉ lệ CŨ** (`nRate = 0.01` với `random(10000000)` = 0,01 %) trước khi bị thay bằng `random(1,100) < 50` (≈ 49 %). Chênh **~5 000 lần**. Báo cáo ghi tỉ lệ đang chạy (49 %) nhưng không nêu bản cũ — mất manh mối "con số nào là chủ ý thiết kế". | `shuizeideath.lua:6-8` (comment) vs `:9-10` (đang chạy) | 🟢 THẤP |
| B9 | **`boss.lua` kéo theo 3 `IncludeLib` và 11 `Include` (tổng 14 dòng nạp) dù là mã chết** **[đã sửa theo đối chất — trước ghi "14 `Include`"; đếm lại `boss.lua` 83 dòng: `Include(` = 11, `IncludeLib(` = 3]**, trong đó `IncludeLib("LEAGUE")` và `IncludeLib("TASKSYS")` **không xuất hiện ở bất kỳ tệp sống nào khác của tính năng**. Báo cáo kết luận đúng "không port `boss.lua`" nhưng nếu ai đó port nhầm thì kéo theo `battlehead.lua`, `judgeoffline_limit.lua`, `composelistclass.lua`, `composeex.lua`, `droptemplet.lua`, `progressbar.lua`… | `boss.lua:2-15` | 🟢 THẤP |
| B10 | **`fld_smalltimer.lua` dùng biến toàn cục `t`, `posx`, `posy`** (`:5, :36, :43, :50, :59`) — không `local`. Ba thuyền chạy **cùng một máy ảo Lua**; `t` được đọc lại từ `GetMissionV` mỗi nhịp nên vô hại, nhưng `posx/posy` bị 3 thuyền ghi đè lẫn nhau trong cùng một nhịp `OnTimer`. Báo cáo có nêu "dữ liệu mission là per-subworld" nhưng **không nêu biến Lua toàn cục thì KHÔNG per-subworld**. | `fld_smalltimer.lua:5,36,43,50,59`; `mission.lua:11,21,25,89,104,112` (`worldid`, `posx`, `idx`, `camp` cũng toàn cục) | 🟡 TRUNG BÌNH |

**Không phải bỏ sót (đã kiểm, báo cáo 02 CÓ):** `boss.lua` là mã chết + 2 đường dẫn Include không tồn tại ·
`fld_landingpos` dùng `and` thay `or` · `fld_getadata` thiếu kiểm `totalcount==0` · `MS_STATE` ghi-không-đọc ·
`fenglingdu_main` vs `main` · `jf0904_shuizei_IsActtime` luôn đúng · thiếu `渡船刷怪点.txt` ·
`fld_TakeBoat` không khôi phục `SubWorld` ở 2 nhánh thoát sớm · vị trí 3 Thuyền phu.

---

## PHẦN C — BÁO CÁO `03_vuotai.md`: **14 mục bị bỏ sót**

| # | Mục bị bỏ sót | Bằng chứng | Mức độ quan trọng |
|---|---|---|---|
| C1 | **🔴🔴 Cấp 2 KIỂM Sát Thủ Giản trong RƯƠNG nhưng TRỪ trong TÚI.** Kiểm: `dragonboat_main.lua:65` `CalcEquiproomItemCount(6,1,400,90)` (rương/equiproom). Trừ: `dragonboat_main.lua:142` `ConsumeItem(3, 1, 6,1,400,90)` (**vị trí 3 = hành trang**). Cấp 1 thì nhất quán (`:59 CalcEquiproomItemCount` ↔ `:137 ConsumeEquiproomItem`). ⇒ Đội cấp cao để giản trong rương **qua được cửa kiểm** nhưng `nConsumed = 0` ⇒ `JoinMission` **không chạy**, người đó bị bỏ lại **im lặng, không thông báo gì**. Báo cáo ghi cả hai dòng như hai sự kiện trung lập, **không nhận ra mâu thuẫn**. | `dragonboat_main.lua:64-66` vs `:141-143`, `:145 if (nConsumed > 0)`, `:163 JoinMission` | 🔴 **RẤT CAO** — chặn cửa gameplay |
| C2 | **Hai bộ kiểm điều kiện lệch dải cấp Sát Thủ Giản.** `COT_CheckTeamRequirement` cấp 1 quét `for i=20,80,10` (`dragonboat_main.lua:58`) — **loại cấp 90**; còn `tbCOT_Party:CalcSword()` (bảng "kiểm trước cho vui") quét `for j=20,90,10` (`npcNhiepThiTran.lua:50`) — **có cấp 90**; và chính câu thoại `:69` ghi "Sát Thủ Giản … **cấp 90 trở xuống**". ⇒ người chỉ có giản cấp 90 được NPC báo "đủ điều kiện" rồi bị chặn khi báo danh thật. | `dragonboat_main.lua:58, :69` vs `npcNhiepThiTran.lua:50` | 🔴 **CAO** |
| C3 | **Toàn bộ hệ ĐIỂM NĂNG ĐỘNG (huoyuedu) của Vượt Ải không được nhắc.** `grep -i "huoyuedu\|năng động\|hoạt dược"` trong `03_vuotai.md` = **0**, trong khi báo cáo 01 và 02 đều có mục riêng. Đường thật: `award.lua:254` phát `G_ACTIVITY:OnMessage("Chuanguan", batch, …)` → `activitysys\config\36\config.lua:5-17` (`szMessageType="Chuanguan"`, `tbMessageParam={"28"}`) → `tbHuoYueDu:AddHuoYueDu("shijiandetiaozhan")` → `huoyuedu.lua:21 ["shijiandetiaozhan"] = 3` → `settings\huoyuedu\huoyuedu.txt` dòng 4: `3	时间的挑战	2864	2	5	5` ⇒ **task 2864, tối đa 2 lần/ngày, +5 rồi +5 điểm**. | `award.lua:254`; `activitysys\config\36\config.lua:5-17`; `huoyuedu.lua:21`; `huoyuedu.txt:4` | 🔴 **CAO** — thiếu 1 task id (2864) + 1 tệp cấu hình + 1 config activitysys trong danh sách port |
| C4 | **`map_random_awards` bị GIẢI MÃ SAI trong `src_utf8` — cả HAI bản sao.** `award.lua` dòng 9-53 và `npc.lua` dòng 260-303 (bản trùng lặp mà §10.9 đã phát hiện) là các dòng **trộn TCVN3 (tên vật phẩm tiếng Việt) + GBK (chú thích chữ Hán) trong CÙNG MỘT DÒNG**; `gbktool` chọn nhánh GBK ⇒ tên vật phẩm ra chữ Hán vụn: `"Tinh H錸g B秓 Th筩h"` (đúng: *Tinh Hồng Bảo Thạch*), `"V� L﹎ M藅 T辌h"` (*Võ Lâm Mật Tịch*), `"L謓h b礽 Phong L╪g ч"` (*Lệnh bài Phong Lăng Độ*), `"мnh Qu鑓 …"` (*Định Quốc …*). Quét toàn cây `src_utf8`: **104 dòng chứa ký tự thay thế U+FFFD trên 16 tệp**, nặng nhất là `award.lua` (36 dòng) và `npc.lua` (36 dòng). **[đã sửa theo đối chất]** — con số 104 là **số dòng có U+FFFD**, KHÔNG phải số dòng giải mã sai: U+FFFD chỉ xuất hiện khi byte TCVN3 rơi vào khe trống của GBK, còn phần lớn dòng hỏng lại ra **chữ Hán hợp lệ nên không có dấu hiệu cảnh báo nào**. Đếm lại theo tên vật phẩm (chuỗi trong ngoặc kép đầu dòng): `award.lua:9-53` có **45/45 dòng sai tên** nhưng chỉ **35** dòng mang U+FFFD; `npc.lua:260-303` là **44/44 sai** nhưng chỉ **34** mang U+FFFD. ⇒ riêng hai bảng này đã là **89 dòng hỏng**, không phải 72. Con số toàn cây (≥104) **chưa đo được** bằng chỉ dấu U+FFFD. | `badenc.py` trên `src_utf8`; giải lại dòng `award.lua:13` bằng cp1258 = `"Vâ L©m MËt T̃ch"` (TCVN3 chuẩn) | 🔴 **RẤT CAO** — bảng thưởng ải ẩn là dữ liệu **cốt lõi**, ai port từ `src_utf8` sẽ chép nguyên rác |
| C5 | **24 dòng dùng cú pháp upvalue Lua 4.0 `%tên`, trong đó 17 dòng tham chiếu biến TOÀN CỤC nằm ở tệp khác.** Danh sách đầy đủ: `award.lua:103,104,105,109,110` (`%tbAward_batch`, `%tbPro` — cả hai `local` cùng tệp, an toàn); `doubleexp.lua:10,11` (`%tbDouble`, local, an toàn); `chuangguanbaoxiang.lua:27,33,39,45,51,77,83,89,95,101,107` (`%tbvng_ChestExpAward` — **toàn cục** của `vng_event\change_request_baoruong\exp_award.lua`), `:165` (`%TransLife6`, `%TASK_ID_BOX` — **toàn cục** của `task\metempsychosis\translife_6.lua`); `npc.lua:492` (`%tbTalkDailyTask`); `dragonboat_main.lua:154,155,157,160` (**`%TSK_Longxuewan_Date`, `%TSK_Longxuewan_avail`** — toàn cục khai ở `include.lua:117,119`). Giá trị bị đóng băng lúc nạp tệp ⇒ **thứ tự `Include` là điều kiện đúng/sai**. Nếu `include.lua` chưa chạy khi `dragonboat_main.lua` được nạp thì `SetTask(nil, …)` ⇒ **hệ Long Huyết Hoàn hỏng câm**. | `lparser.c:229-242` + 24 dòng nêu trên (đếm bằng `verify.py`); cả 3 tính năng cộng lại = **28 dòng** (satthu 3 · PLĐ 1 · Vượt Ải 24) | 🔴 **CAO** |
| C6 | **`nPrisonId = 208` — hằng chặn "vượt ngục" không được liệt kê là hằng/phụ thuộc bản đồ.** Báo cáo có câu "Chặn nếu đang ở map 208 (đại lao)" nhưng `grep "nPrisonId"` = **0** và **map 208 không có trong §9.1 "Bản đồ" lẫn §11 danh sách việc phải làm** — trong khi đó là bản đồ thứ 33 mà hệ này chạm tới. | `dragonboat_main.lua:128-131` | 🟡 TRUNG BÌNH |
| C7 | **`rank_award()` có CỬA GIỜ viết sai.** `rank_perday.lua:16-20`: `ntime = tonumber(GetLocalDate("%H%M"))` (dạng HHMM) rồi `if (ntime < 4 or ntime > 2300) then` chặn. Ý đồ hiển nhiên là "từ 04:00 đến 23:00" nhưng `4` ở dạng HHMM = **00:04**. ⇒ cửa thật là **00:04 → 23:00**, tức mở gần như cả ngày; chỉ 4 phút đầu ngày và sau 23:00 là đóng. Báo cáo có chép con số 2300 nhưng **không phát hiện lệch đơn vị**. | `rank_perday.lua:16-17` | 🟡 TRUNG BÌNH — sai lệch luật chơi |
| C8 | **`get_top5team()`: nhánh `break` khi hết dữ liệu bị comment ⇒ luôn in đủ 5 dòng.** `rank_perday.lua:86-88` `--		if (RoleName == "") then / --			break / --		end`. Vòng `:79` chạy cứng `for i = 1, 5`; chỉ có `:82` chặn khi **dòng 1** rỗng. ⇒ bảng xếp hạng ngày mới có 2 đội vẫn hiện "Hạng 3/4/5: (trống) Thành tích: 0 phút 0 giây". Thêm: `tbRoleName` (`:78`), `RoleName`, `value` (`:80`) đều là **biến toàn cục**. | `rank_perday.lua:78-93` | 🟡 TRUNG BÌNH |
| C9 | **Thông báo MỞ BÁO DANH bị comment ⇒ hoạt động không tự rao.** `mission_match.lua:22-24`: `--由relay通告报名开始，2次` / `--local min = floor(LIMIT_SIGNUP / 60);` / `--broadcast("<#>时间的挑战任务报名开始了…")`. Mâu thuẫn trực tiếp với chính câu thoại đang chạy `dragonboat_main.lua:120` "*…Cách mỗi giờ báo danh 1 lần. **Xin chú ý hệ thống thông báo!***". Chỉ còn thông báo **KHAI TRẬN** (`timer_match.lua:26`). Chú thích gốc nói việc rao được chuyển cho **Relay** — tức phụ thuộc **ngoài cây script**. | `mission_match.lua:22-24` vs `dragonboat_main.lua:120`, `timer_match.lua:26` | 🔴 **CAO** — port xong sẽ "không ai biết mà vào" |
| C10 | **`GetMatchPlayerList()` đếm theo NHÓM 1 nhưng duyệt theo NHÓM 0.** `include.lua:176` `for i=1, GetMSPlayerCount(MISSION_MATCH, **1**) do` … `:177 index, player = GetNextPlayer(MISSION_MATCH, index, **0**);`. Đây là hàm gom người chơi mà **toàn bộ đường phát thưởng** (`award_batch_extend`, `award_hidden_mission`, `award_batch`, `kickout`, `save_player_info`) đều dựa vào. `grep "GetMatchPlayerList"` trong báo cáo = **1** (chỉ liệt kê tên hàm ở §7). | `include.lua:169-186`, cụ thể **`:176` (nhóm 1) vs `:177` (nhóm 0)** | 🟡 TRUNG BÌNH |
| C11 | **`map_posfiles` được ĐỌC ở `include.lua:384` nhưng ĐỊNH NGHĨA ở `npc.lua:11`.** `npc.lua` `Include` `include.lua` chứ không ngược lại ⇒ mọi đường nạp **chỉ có `include.lua`** (ví dụ `timer_close.lua:5`, `timer_board.lua:5`, `npcNhiepThiTran` qua `nieshichen.lua:5`) sẽ để `map_posfiles = nil`, và `func_npc_getpos` ném lỗi khi bảng NPC dùng dạng hàm. Báo cáo liệt kê 7 tệp `lineup*.txt` phải chép nhưng **không nêu ràng buộc thứ tự nạp này**. | `include.lua:384` vs `npc.lua:11-19`; `npc.lua:6` Include include.lua | 🟡 TRUNG BÌNH |
| C12 | **`_Message` trong `chuangguanbaoxiang.lua:9-15` là mã chết ⇒ KHÔNG có thông báo toàn server khi mở Bảo Rương Vượt Ải.** `grep "_Message\|CallBack"` trong tệp 171 dòng = **đúng 1 hit** (chính dòng định nghĩa). Bảng thưởng dùng `pFun`, không dùng `CallBack`. Báo cáo có đánh dấu `AnnounceLuckyAward` (`chuangguang30.lua:29`) là hàm không ai gọi nhưng **bỏ sót `_Message`**. Kéo theo `OB_Create/ObjBuffer:PushObject/RemoteExecute/OB_Release` chỉ còn là phụ thuộc "chết". | `chuangguanbaoxiang.lua:9-15`; grep trong tệp | 🟡 TRUNG BÌNH |
| C13 | **11 closure `pFun` của bảng thưởng rương không được liệt kê ở §7.** `chuangguanbaoxiang.lua` dòng 26, 32, 38, 44, 50 (chìa khoá như ý: 1/2/3/4/5 triệu exp, tỉ lệ 52/30/10/5/3) và 76, 82, 88, 94, 100, 106 (chìa khoá vàng: 2/4/5/6/8/10 triệu exp). §7 chỉ ghi 3 hàm (`main/Oncancel/VnCOTBoxNewAward`). Kèm theo: 3 hằng **toàn cục tên rất chung** `nWidth=1, nHeight=1, nFreeItemCellLimit=1` (`:114-116`) trong một script vật phẩm — nguy cơ va tên. | `chuangguanbaoxiang.lua:26-107, :114-116` | 🟡 TRUNG BÌNH |
| C14 | **8 tên bảng/hằng của tính năng chưa từng xuất hiện trong báo cáo** (grep = 0 cho từng tên) **[đã sửa theo đối chất — trước ghi 9 tên và có kể `tbHidenNpc`; thực tế `grep "tbHidenNpc" 03_vuotai.md` = **5** (dòng 225, 407, 494, 1045, 1143) nên phải loại tên này khỏi danh sách]**: `tbFightNpc` (`npc.lua:25` — bảng gốc chứa cả 2 cấp), `tbNormalNpc` (`npc.lua:27, :132`), `map_series` (`include.lua:143`), `tbMapList` + `tbFreeMaps` (`dragonboat_main.lua:83, :85`), `tbRegist` (`chuangguang30.lua:16`), `tbCOT_Key_Require` (`chuangguanbaoxiang.lua:17`), `tbDouble` (`doubleexp.lua:6`), `tbRoleName` (`rank_perday.lua:78`); và 2 tên hằng `tsk_rank_lastscore` (=2637) / `tsk_rank_2thscore` (=2639) — **giá trị số thì có trong báo cáo, tên hằng thì không**; `BOAT_POSY`(3226), `VARV_SIGNUP_POSY`(7), `SEX_BOY/SEX_GIRL` cũng chỉ có "anh em POSX/SEX_RENYAO" được nêu. | `inv.json` mục `vuotai.tables/consts` đối chiếu `cmp.py vuotai tables|consts` | 🟢 THẤP — nhưng làm bảng tra cứu khi port bị hụt |

**Không phải bỏ sót (đã kiểm, báo cáo 03 CÓ):** cửa giờ `transfer.lua:38` không bao giờ đúng · `nMinute >= 24` ·
`cancel_match()` rỗng · `get_random_npc_id(2)` trả nil · `AWARD_COUNT=10` mã chết · `map_random_awards` khai 2 lần ·
`chuangguang30_playerdeath.lua` mã chết · `bfind` khai `local` trong `if` · điều kiện lọc NPC Mật Phòng sai ·
`JoinMission` thiếu ở JX1 · `Pay(10000)` vs "10 vạn" · `ClearMapNpc(world,1)` bị JX1 bỏ tham số 2.
Ngoài ra: `Say(msg, n, tbTable)` (dạng truyền **bảng**, dùng ở `chuangguanbaoxiang.lua:141` và `fld_head.lua:75`)
**KHÔNG phải lỗi** — JX1 hỗ trợ sẵn dạng này (`ScriptFuns.cpp:583` `Say(nMainInfo, nSelCount, SelTab)`).

---

## PHẦN D — LIÊN HỆ GIỮA 3 TÍNH NĂNG mà cả 3 báo cáo chưa nói hết

| # | Liên hệ | Bằng chứng | Mức độ |
|---|---|---|---|
| D1 | **🔴 MỘT nhiệm vụ "Đổi Mệnh" (Thần Nông Lão Gia) trói CẢ BA tính năng vào một chuỗi.** `event\change_destiny\mission.lua` có đúng 4 nhiệm vụ con: `completeMission_Battle` (Tống Kim cao cấp 3000 điểm, `:4`), **`completeMission_NieShiChen`** (Vượt Ải cao cấp — duyệt `MISSION_MATCH`, gọi từ `challengeoftime\npc_death.lua:149`, `:30`), **`completeMission_WaterThief`** (Phong Lăng Độ — gọi từ `fengling_ferry\bossdeath.lua:47`, `:62`), **`completeMission_Killer`** (Sát Thủ, 4 lượt cấp 90 — gọi từ `killer\kill_level.lua:68`, `:80`). Mỗi báo cáo chỉ thấy mảnh của mình; **không báo cáo nào nói 3 tính năng này là 3/4 chân của cùng một chuỗi nhiệm vụ**. ⇒ Port lẻ 1 tính năng thì Đổi Mệnh **không thể hoàn thành**. | `change_destiny\mission.lua:4, 30, 62, 80`; 3 điểm gọi nêu trên | 🔴 **RẤT CAO** |
| D2 | **Vượt Ải là NGUỒN SINH vé vào Phong Lăng Độ.** Bảng thưởng ải ẩn `map_random_awards` có dòng `{0.05, {"Lệnh bài Phong Lăng Độ", **489**}}` (`award.lua:22`, và bản sao `npc.lua:273` — cả hai dòng **đang bị giải mã sai** trong `src_utf8`, xem C4) — chính là event-item `4,489` mà `fld_head.lua:269` kiểm để cho lên thuyền. Báo cáo 03 có ghi "event 489" trong bảng; **báo cáo 02 hoàn toàn không biết vé của mình từ đâu ra** (`grep -i "vượt ải\|challengeoftime\|chuangguan"` trong `02_phonglangdo.md` = **0**). | `award.lua:22` ↔ `fld_head.lua:269` | 🔴 **CAO** — port PLĐ mà không port Vượt Ải là cắt nguồn vé |
| D3 | **Rương của Phong Lăng Độ và rương của Vượt Ải dùng CHUNG 2 chìa khoá.** `activitysys\config\17\vnshuizeibaoxiang.lua:14-17` (`tbPirate_Box_Key_Require`) và `challengeoftime\item\chuangguanbaoxiang.lua:17-20` (`tbCOT_Key_Require`) **giống hệt**: `chiakhoanhuy = {6,1,2744}`, `chiakhoavang = {6,1,30191}`. Hai tệp còn là bản sao gần như từng dòng (cùng `_Message` chết, cùng thang exp `pFun`, cùng `%tbvng_ChestExpAward`). Rương: PLĐ = `6,1,2743`, Vượt Ải = `6,1,2742` (liền số). ⇒ **kinh tế chìa khoá là chung**, port một nửa sẽ lệch cung/cầu. | `config\17\vnshuizeibaoxiang.lua:6-17` ↔ `chuangguanbaoxiang.lua:9-20`; `bossdeath`/`Landing` phát 2743, `award_batch_extend` phát 2742 | 🔴 **CAO** |
| D4 | **`activitysys\config\17` cũng là nguồn Huyền Thiên Chùy của Sát Thủ Bí Bảo** (báo cáo 01 §2.8 đã ghi). ⇒ **config 17 là điểm giao của cả 3 tính năng** (chùy cho hệ Sát Thủ, rương cho PLĐ, cùng bộ chìa khoá với Vượt Ải). Không báo cáo nào gọi tên nó là điểm giao. | `01_satthu.md` §2.8 + D3 | 🟡 TRUNG BÌNH |
| D5 | **Ba tính năng dùng CHUNG một NPC (id 769 "Nhiếp Thí Trần") làm cửa vào, và cùng một tệp `killbosshead.lua`.** Tệp head 3 421 dòng: 4-180 = Sát Thủ (160 boss); phần còn lại ≈ 95 % mang danh **Tín Sứ / ải** nhưng theo A10 thì **hầu hết là mã chết** — chỉ còn 18 dòng dữ liệu sống. NPC 769 ở 7 thành mở menu vừa nhận nhiệm vụ Sát Thủ, vừa báo danh Vượt Ải (`want_playboat`), vừa nhận thưởng hạng ngày Vượt Ải (`rank_award`, `get_top5team`). Vé vào Vượt Ải (`6,1,400` Sát Thủ Giản) **do chính hệ Sát Thủ sinh ra**. Báo cáo 01 §8.6 nêu quan hệ này nhưng chỉ theo hướng Sát Thủ→Vượt Ải; **báo cáo 03 không nêu ngược lại rằng hệ mình PHỤ THUỘC toàn bộ vào hệ Sát Thủ để có vé**. | `nieshichen.lua:19,5,11`; `dragonboat_main.lua:29,59,65,136,142`; `killbosshead.lua` bố cục | 🟡 TRUNG BÌNH |
| D6 | **`EventSys` có 3 "kiểu" riêng cho 3 tính năng, cùng dựa vào `OnPlayerEvent` — hàm KHÔNG TỒN TẠI trong bản Linux.** `KillerBoss` (`kill_level.lua:72,74`), `FengLingDu` (`fld_head.lua:140`, `shuizei.lua:79`), `ChuanGuan` (`mission_match.lua:98`). Báo cáo 01 §8.2 chứng minh `OnPlayerEvent` = 0 định nghĩa Lua / 82 điểm gọi / không có chuỗi trong ELF; **báo cáo 02 và 03 không kế thừa kết luận này** dù cả hai đều có điểm gọi `OnPlayerEvent`. ⇒ Cùng một rủi ro, phải xử lý một lần cho cả 3. | `01_satthu.md` §8.2; `fld_head.lua:140`; `mission_match.lua:98` | 🔴 **CAO** |
| D7 | **Bậc điểm năng động của 3 tính năng lệch nhau và Vượt Ải bị bỏ quên.** `huoyuedu.txt`: dòng 3 `2 风陵渡船 2863 2 5 5` (PLĐ: +5/+5, tối đa 2) · dòng 4 `3 时间的挑战 **2864** 2 5 5` (Vượt Ải) · dòng 12 `11 杀手任务 2871 2 **3 3**` (Sát Thủ: +3/+3). Báo cáo 01 và 02 có; **báo cáo 03 = 0 lần nhắc** (xem C3). | `settings\huoyuedu\huoyuedu.txt:3,4,12`; `huoyuedu.lua:21,22,26` | 🟡 TRUNG BÌNH |

---

## PHẦN E — ĐẾM LẠI CÁC CON SỐ THEN CHỐT

Tất cả đếm lại bằng script trên tệp gốc, **không chép từ báo cáo**.

### E.1 Báo cáo 01 — "160 boss / 8 nhóm / 40 map / 39 hàm script / 36 hàm engine"

| Con số báo cáo | Đếm lại của tôi | Khớp? | Ghi chú |
|---|---|---|---|
| 160 boss | **160** dòng dữ liệu 10 cột trong `killbosshead.lua` vùng dòng 4→180 | ✅ | Chỉ số nhiệm vụ (cột 10) = `1..160` **liên tục, không trùng, không đứt** |
| 8 nhóm | **8** dòng comment mốc (`:5, 27, 49, 71, 93, 115, 137, 159`), cấp NPC `{25,35,45,55,65,75,85,95}` mỗi cấp **đúng 20 dòng** | ✅ | |
| 40 map | **40** map id phân biệt: `4,5,10,12,14,22,23,24,42,43,56,66,71,72,73,75,76,77,83,91,93,94,117,123,135,141,143,148,164,168,181,193,194,196,201,202,225,319,321,340` | ✅ | Mỗi nhóm đúng 5 map × 4 boss |
| (bổ sung) id mẫu NPC | 59 id phân biệt, dải 761→820, **thiếu đúng `769`**; cột 6 luôn `0`; cột 8 luôn `1`; cột 9 luôn `kill_level.lua` | ✅ | Trùng khớp §4.2 |
| 39 hàm script (26 + 13) | Trong 7 tệp hạt giống có **32 tên `function`**, trong đó **7 bị comment** (`killer20`…`killer80`) ⇒ **25 tên sống**; tên `main` có **2** định nghĩa (`nieshichen.lua:56` + `shashou_mibao.lua:15`) ⇒ **26 mục** đúng bằng §6.1 | ✅ | §6.2 (13 hàm ngoài) không đếm cơ học được — nằm ngoài bộ hạt giống |
| 36 hàm engine | Bộ hạt giống gọi **71 tên** khác nhau: **13** tên tự định nghĩa, **58** tên ngoài. Trong 58 tên ngoài: **2** rác regex (`Hoàng`, `boss应使用add_messengernpc`), **10** lời gọi phương thức trên bảng script (`EventSys:GetType`, `G_ACTIVITY:OnMessage`, `tbAwardTemplet:Give/GiveAwardByList`, `killertabfile:getCell`, `PlayerFunLib:CheckTotalLevel`, `tbBirthday0905:IsActDate`, `tbCOT_Party:CheckCondition`, `tbChangeDestiny:completeMission_Killer`, `tbLog:PlayerActionLog`), **6** hàm thư viện chuẩn Lua (`format/getn/random/tinsert/tonumber/unpack`), **9** hàm do script khác định nghĩa (`nt_getTask`, `nt_setTask`, `tongaward_killer`, `new`, `fnCallback`, `AddExp_Skill_Extend`, `Reg`, `OnPlayerEvent`, `OnTeamEvent`) ⇒ còn **31 tên hàm engine gọi TRỰC TIẾP**. §7 có 36 dòng vì cộng thêm 5 hàm gọi **gián tiếp** (`SetTask`, `SyncTaskValue`, `TabFile_Load`, `TabFile_GetCell`, `Add120SkillExp`) và gộp thư viện chuẩn vào 1 dòng. **Không hàm engine nào bị bỏ sót.** | ✅ | |

### E.2 Báo cáo 02 — "mission id 15 / 3 map thuyền / sức chứa 100 / 30 quái / 3 boss"

| Con số báo cáo | Đếm lại của tôi | Khớp? |
|---|---|---|
| mission id 15 | `fld_head.lua:10 MISSIONID = 15` | ✅ |
| 3 map thuyền | `fld_head.lua:12 boatMAPS = {337, 338, 339}` | ✅ |
| sức chứa 100 | `fld_head.lua:153 GetMSPlayerCount(MISSIONID, 1) >= 100` | ✅ |
| 30 quái | `mission.lua:20 for i=1, 30 do … AddNpc(724, 95, …)` — **số 30 viết cứng**, hằng `TNPC_THIEF_COUNT = 30` (`fld_head.lua:17`) là hằng **chết** | ✅ |
| **3 boss** | ⚠️ **KHÔNG ĐỦ.** Đếm lại: **3 lần** sinh NPC **725** (`fld_smalltimer.lua:37, :44, :60`) **+ 2 lần** sinh NPC **1692** ở khung giờ tốn phí (`:49-54`, vòng `for i=1,2`) ⇒ **tối đa 5 boss/lượt, 2 loại NPC**. Con số "3 boss" chỉ đúng cho **khung giờ thường**. Thân bài §4 có ghi đủ cả 1692 nhưng phần TÓM TẮT lại rút gọn thành "3 đợt Boss" | ⚠️ **một phần** |
| (bổ sung) số hàm | **34 tên `function` phân biệt** trong 13 tệp hạt giống, **0 hàm bị comment**. §6 của báo cáo: §6.1 = 15 dòng, §6.2 = 6 dòng, §6.3 = 13 dòng bảng nhưng **kể 19 tên** (dòng `boss.lua` gói 8 hàm) ⇒ 40 lượt nhắc, **34 tên phân biệt** sau khi trừ trùng (`OnTimer` ×3, `OnDeath` ×4, `main` ×2). Khớp | ✅ |

### E.3 Báo cáo 03 — "28 ải / 2 cấp độ / 32 map / hai chế độ"

| Con số báo cáo | Đếm lại của tôi | Khớp? |
|---|---|---|
| 28 ải | Đếm mốc `-- N` trong `tbFightNpc`: **cấp 1 = 28 ải (1..28 liên tục)**, **cấp 2 = 28 ải (1..28 liên tục)** | ✅ |
| 2 cấp độ | `include.lua:83-88 tbLevels` = 2 mục `{50,90}` và `{90,201}` | ✅ |
| 32 map | `include.lua:90-103 tbLevelMaps`: cấp 1 = 464..479 (**16**), cấp 2 = 480..495 (**16**) = **32** | ✅ |
| hai chế độ | `VARV_BATCH_MODEL` 0/1, bật ở `dragonboat_main.lua:195-200`; 4 bảng con `tbNormalNpc`/`tbHidenNpc` × 2 cấp (`npc.lua:27,123,132,228`) | ✅ |
| (bổ sung) số hàm | **106 tên `function`** trong 17 tệp hạt giống (đếm mỗi tên 1 lần; 11 closure `pFun` gộp thành 1). §7 của báo cáo liệt kê ~110 mục ⇒ **không lệch thật**, chênh do §7 tách `pFun`/tên trùng ở nhiều tệp | ✅ |
| (bổ sung) map thứ 33 | **957** (Mật Phòng) — báo cáo có; **map 208 (đại lao)** thì không được tính vào danh sách bản đồ (xem C6) | ⚠️ |

---

## PHẦN F — PHẠM VI ĐÃ QUÉT (để giới hạn khẳng định)

**Đã làm:** kiểm kê cơ học 37 tệp hạt giống (7 + 13 + 17) · đọc lại **toàn văn** 33/37 tệp (bỏ 4 tệp bảng dữ liệu
thuần: `killbosshead.lua`, `npc.lua` phần bảng, `award.lua` phần bảng, `chuangguang30.lua` phần bảng) ·
liệt kê **mọi** dòng code bị comment (satthu **77** — gồm cả `killbosshead.lua`; phonglangdo **20**; vuotai **19**) ·
liệt kê **mọi** dòng chạm NGÀY/GIỜ/SỰ KIỆN (**93 dòng**) · quét toàn cây `src_utf8` tìm dòng giải mã hỏng ·
đếm lại 13 con số then chốt · dò 21 điểm nối chéo giữa 3 tính năng.

**Chưa làm (nên là hướng quét tiếp):**
- Chưa đọc hết **`killbosshead.lua` vùng 200-3390** (≈ 3 200 dòng, thuộc hệ Tín Sứ) — có thể còn bảng dùng chung.
- Chưa đối chiếu **`.bak`** ở mức byte cho `fengling_ferry` (báo cáo 02 §9 đã làm, tôi chỉ xác nhận đúng 3 tệp `.bak`
  tồn tại trong toàn bộ `server1\script`: `loginprize.lua.bak`, `fld_smalltimer.lua.bak`, `mission.lua.bak`
  — **không có `.bak` nào cho Sát Thủ và Vượt Ải**).
- Chưa xác minh ngữ nghĩa **tham số 4 và 5 của `ITEM_DropRateItem`** trên ELF (mục A5 vẫn là **CHƯA XÁC MINH**).
- Chưa kiểm hết **467 tệp settings chỉ có ở `Patch`** cho 3 tính năng này — đã xác nhận `D:\ServerLinux\Patch\script`
  **KHÔNG chứa** bất kỳ tệp nào của 3 tính năng (`Patch\script` chỉ có 9 thư mục:
  `activitysys, event, global, item, skill, task, tong, ui, vng_event`; không có `missions`, không có `task\tollgate`).

**Một điểm lệch dữ liệu phát hiện thêm (không thuộc 3 báo cáo):**
`closure3.json` khoá `vuotai` **KHÔNG chứa** `vng_feature\challengeoftime\npcnhiepthitran.lua`
(nó chỉ nằm trong khoá `satthu`), nên `src_utf8\vuotai\vng_feature\` **thiếu tệp này** —
trong khi §7 và §1.3 của `03_vuotai.md` đều trích dẫn nó. Ai làm việc chỉ trong `src_utf8\vuotai\`
sẽ **không tìm thấy** tệp quy định điều kiện báo danh của tính năng.

---

### Tổng kết số mục bỏ sót

| Báo cáo | Số mục bỏ sót tìm được |
|---|---|
| `01_satthu.md` | **10** |
| `02_phonglangdo.md` | **10** |
| `03_vuotai.md` | **14** |
| Liên hệ chéo 3 tính năng | **7** |
| **Tổng** | **41** |

### 10 mục quan trọng nhất (xếp theo mức nguy hiểm khi port)

1. **C1** — Vượt Ải cấp 2: **kiểm Sát Thủ Giản trong RƯƠNG nhưng trừ trong TÚI** (`dragonboat_main.lua:65` vs `:142`) ⇒ người chơi qua cửa kiểm rồi **không vào được ải**, im lặng không báo lỗi.
2. **A10** — `killbosshead.lua` chỉ **243/3421 dòng sống**; `AddNpc_allbugbear` là bảng **RỖNG** ⇒ `add_alltollgatenpc()` sinh **18 NPC**, không phải hàng nghìn. §4.1 đánh dấu thiếu 4 bảng.
3. **C4 + A6** — Bản giải mã `src_utf8` hỏng ở **ít nhất 104 dòng / 16 tệp** (104 = số dòng có U+FFFD; **[đã sửa theo đối chất]** đây là **cận DƯỚI**, vì dòng hỏng thường ra chữ Hán hợp lệ không có U+FFFD — riêng `award.lua:9-53` + `npc.lua:260-303` đã là **89 dòng sai tên vật phẩm** so với 72 dòng có U+FFFD), nặng nhất là **cả hai bản sao của `map_random_awards`** (bảng thưởng ải ẩn, `award.lua:9-53` và `npc.lua:260-303`) và 2 chuỗi luật của NPC Sát Thủ. Port từ `src_utf8` = chép rác.
4. **D1** — Nhiệm vụ **Đổi Mệnh (Thần Nông Lão Gia)** trói **cả 3 tính năng** vào một chuỗi 4 bước (`change_destiny\mission.lua:4/30/62/80`). Port lẻ 1 tính năng ⇒ chuỗi này không hoàn thành được.
5. **A1 / B6 / C5** — **28 dòng** dùng upvalue Lua 4.0 `%tên` (đã dò độc lập trên 4 thư mục gốc: khớp đúng 3 + 1 + 24), **18 dòng trỏ biến toàn cục ở tệp khác** **[đã sửa theo đối chất — trước ghi 17, đó là con số riêng của Vượt Ải; cộng `fld_smalltimer.lua:13` (`%tbTalkDailyTask`) của Phong Lăng Độ thì là 18]**; giá trị bị **đóng băng lúc nạp tệp** (`lparser.c:229-242`) ⇒ **thứ tự `Include` quyết định đúng/sai**. Cả 6 báo cáo 00–06 = 0 lần nhắc.
6. **B2** — `SetTaskTemp(200, 0)` **bị comment ở cả hai đường khôi phục** (`mission.lua:71` và `:116`) ⇒ cờ "cho phép đổi phe / PK" **không bao giờ được xoá** sau Phong Lăng Độ.
7. **D3 + D2** — Vượt Ải và Phong Lăng Độ **dùng chung 2 chìa khoá** (`6,1,2744` / `6,1,30191`) và Vượt Ải là **nguồn sinh vé** `4,489` của Phong Lăng Độ (`award.lua:22`). Port một nửa là vỡ kinh tế.
8. **B4 + B3** — `Landing()` gom `tbPlayer` có **lỗ hổng chỉ số** rồi duyệt bằng `getn` ⇒ **[đã sửa theo đối chất]** `lua_getn` Lua 4.0 trả **khoá số lớn nhất** (`lapi.c:458-476`; ELF `0x08233070`), nên **không ai bị bỏ qua** — mà đúng vòng lặp trúng lỗ hổng sẽ chạy với `PlayerIndex = nil`; `RunMission()` chạy `SetFightState`/`PutMessage` **ngoài** khối `if (pidx > 0)`.
9. **C3** — Toàn bộ **điểm năng động của Vượt Ải** (task **2864**, `activitysys\config\36`, +5/+5, 2 lần/ngày) **không có trong báo cáo 03**, dù báo cáo 01 và 02 đều có mục này cho tính năng của mình.
10. **A4 + D6** — `OnTeamEvent` bị comment (`kill_level.lua:73`) chứng minh thưởng "OnFinish" vốn là **của cả đội**; và cả 3 tính năng đều gọi `OnPlayerEvent` — hàm **không tồn tại trong bản Linux** (kết luận của §8.2 báo cáo 01 chưa được báo cáo 02/03 kế thừa).


---

## ĐỐI CHẤT (tác tử độc lập)

**Ai làm:** một tác tử **khác** với tác tử viết mục 16, chạy sau, **mặc định coi mọi khẳng định là SAI cho tới khi tệp gốc / binary chứng minh ngược lại**.
**Đã kiểm:** 41 khẳng định (10 mục Phần A, 10 mục Phần B, 14 mục Phần C, 7 mục Phần D, cùng toàn bộ Phần E và Phần F).
**Không đụng:** `D:\GAMEDEVNEW\Sources`, `D:\GAMEDEVNEW\bin`, `E:\SourceTuanLe`, và các tệp `00_`–`09_`.
Mọi con số dưới đây được đếm lại **trực tiếp trên byte tệp gốc** (`D:\ServerLinux\server1`, `D:\ServerLinux\Patch`) hoặc **tháo mã ELF** `jx_linux_y`, không chép lại từ mục 16.

### Bảng đối chất

| Khẳng định | Bằng chứng gốc (đã tự kiểm) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|
| **A10** `killbosshead.lua` chỉ còn 243/3421 dòng sống; `AddNpc_allbugbear` rỗng; `turesurebug`/`turesureboss` mỗi bảng 9 dòng; `add_alltollgatenpc()` sinh 18 NPC | Đếm byte: **3 421 tổng / 243 sống / 3 144 comment / 34 trống**. Dải sống: `4..180` → 162 dòng (= `{`, `}` + **160** hàng dữ liệu); `200..361` → 0 hàng; `365..2606` → **0 hàng** (chỉ `AddNpc_allbugbear=`, `{`, `}`); `2608..2932` → **9 hàng** `:2922-2930` (mốc `--只保留9个` ở `:2921`); `2999..3067` → **0 hàng**; `3071..3390` → **9 hàng** `:3380-3388` (mốc ở `:3379`). `addtollgatenpc.lua:16,17` gọi `turesureboss`+`turesurebug`, `:19` gọi `add_messengernpc(AddNpc_allbugbear)`; `:12-15,:18` **đều bị comment** ⇒ 9+9+0 = **18** | ✅ **ĐÚNG** (khớp từng con số) | giữ nguyên — bổ sung: `AddNpc_templeboss` **cũng đã bị comment ở CHÍNH ĐIỂM GỌI** (`addtollgatenpc.lua:15`), nên bảng rỗng của nó là vô hại; `SHOUHUZHE_OFFSET = 2` ở `:1` được dùng đúng 9 lần `:2922-2930` ✅ |
| **A1 / B6 / C5** 28 dòng upvalue Lua 4.0 `%tên` trong 3 tính năng; `lparser.c:229-242` cho phép biến toàn cục; giá trị đóng băng lúc nạp tệp | Dò độc lập (bỏ chuỗi & comment) trên 4 thư mục `task\tollgate`, `missions\fengling_ferry`, `missions\challengeoftime`, `vng_feature\challengeoftime`: **đúng 28 dòng thuộc 3 tính năng**, trùng khít danh sách của C5 (satthu 3 · PLĐ 1 · vuotai 24); 12 dòng còn lại nằm ở `task\tollgate\messenger\*` = **hệ Tín Sứ, ngoài phạm vi**. `lparser.c:229-242` đúng là `pushupvalue()` với nhánh `level == -1 /* global? */`; `lparser.c:291-301` là `pushclosure()` — đẩy upvalue lên stack **trước** `OP_CLOSURE` | ✅ **ĐÚNG** | Sửa **tóm tắt mục 5**: "17 dòng trỏ biến toàn cục ở tệp khác" → **18** (17 của Vượt Ải + `fld_smalltimer.lua:13`). Bổ sung: `pushupvalue` còn **báo lỗi cú pháp** `"cannot access upvalue in main"` nếu `%tên` nằm ở thân chunk (`lparser.c:234-235`) — cả 28 dòng đều nằm trong hàm nên hợp lệ |
| **A4** `kill_level.lua:73` là `OnTeamEvent("OnFinish")` bị comment, `:74` là `OnPlayerEvent("OnFinish")` đang chạy | Đọc byte `kill_level.lua`: `:72 …OnPlayerEvent("OnKillBoss"…)`, `:73 --EventSys:GetType("KillerBoss"):OnTeamEvent("OnFinish"…)`, `:74 …OnPlayerEvent("OnFinish"…)`. `grep "OnTeamEvent" 01_satthu.md` = **0** | ✅ **ĐÚNG** | giữ nguyên |
| **D6** `OnPlayerEvent` **không tồn tại** trong bản Linux (0 định nghĩa / 82 điểm gọi / 0 chuỗi ELF); `eventsys.lua` chỉ có 6 hàm | `grep -rn "function.*OnPlayerEvent" server1\script` = **0**; `grep -rn "OnPlayerEvent\s*="` = **0**; tổng điểm gọi = **82**. `eventsys.lua` có đúng **6** `function`: `NewType/GetType/Reg/UnReg/GetProcParam/OnEvent`; `type\func.lua:107-108` chỉ `NewType(key)` cho 20 kiểu, **không thêm phương thức nào**. Đếm chuỗi thô trong ELF: `b"OnPlayerEvent"` = **0**, `b"OnTeamEvent"` = **0** | ✅ **ĐÚNG** (kiểm cả gốc B: `grep "OnPlayerEvent" D:\ServerLinux\Patch` = 0 ⇒ **không phải bẫy hai gốc**) | giữ nguyên. Bổ sung: `EventSys:GetType()` trả `self` khi không tìm thấy kiểu (`eventsys.lua:24`) nên lời gọi hỏng là **`attempt to call a nil value`**, không phải index nil |
| **C1** cấp 2 kiểm Sát Thủ Giản trong RƯƠNG (`:65`) nhưng trừ trong TÚI (`:142`); cấp 1 nhất quán | `missions\challengeoftime\npc\dragonboat_main.lua`: `:59 CalcEquiproomItemCount(6,1,400,i)` ↔ `:137 ConsumeEquiproomItem(1,6,1,400,i)` (cấp 1, **nhất quán**); `:65 CalcEquiproomItemCount(6,1,400,90)` ↔ `:142 ConsumeItem(3, 1, 6,1,400,90)` (cấp 2, **lệch**); `:145 if (nConsumed > 0)` bao trọn `:163 JoinMission(MISSION_MATCH, 1)` | ✅ **ĐÚNG** — mục nguy hiểm nhất và **có thật** | giữ nguyên, **thêm bằng chứng nội tại** (xem "Bỏ sót" §1): `:29 local nTaskItem = CallPlayerFunction(nPlayerIndex, CalcItemCount, 3, 6,1,400,90)` là **biến chết** (đúng 1 lần xuất hiện trong tệp) — chính là cửa kiểm TÚI cũ bị bỏ quên |
| **C3 / D7** hệ điểm năng động Vượt Ải: task **2864**, `activitysys\config\36`, +5/+5, 2 lần/ngày; `03_vuotai.md` = 0 lần nhắc | `award.lua:254 G_ACTIVITY:OnMessage("Chuanguan", …)`; `config\36\config.lua:5 szMessageType="Chuanguan"`, `:9 tbMessageParam={"28"}`, `:12 lib:Include huoyuedu.lua`, `:16 tbHuoYueDu:AddHuoYueDu {"shijiandetiaozhan"}`; `huoyuedu\huoyuedu.lua:21 ["shijiandetiaozhan"] = 3`; `settings\huoyuedu\huoyuedu.txt` dòng 4 = `3 时间的挑战 2864 2 5 5`, dòng 3 = `2 风陵渡船 2863 2 5 5`, dòng 12 = `11 杀手任务 2871 2 3 3`. `grep -i huoyuedu 03_vuotai.md` = **0**, `grep 2864` = **0**; `01` = 14 hit, `02` = 11 hit | ✅ **ĐÚNG** | giữ nguyên. Bổ sung chống bẫy hai gốc: `huoyuedu.txt` **giống hệt từng byte** ở cả `server1` và `Patch` (1 865 B, 43 dòng) |
| **D1** nhiệm vụ Đổi Mệnh có 4 chân, 3 chân là 3 tính năng này | `event\change_destiny\mission.lua`: `:4 completeMission_Battle(tbPlayers)`, `:30 completeMission_NieShiChen(nNpcSettingIdx)`, `:62 completeMission_WaterThief()`, `:80 completeMission_Killer(tbPlayers)`. `grep -rn "completeMission_"` toàn cây = **7 hit**: 4 định nghĩa + `challengeoftime\npc_death.lua:149`, `fengling_ferry\bossdeath.lua:47`, `killer\kill_level.lua:68` | ✅ **ĐÚNG** — 3 điểm gọi đúng số dòng | giữ nguyên |
| **D2 / D3** vé `4,489` sinh từ Vượt Ải; hai rương dùng chung 2 chìa `6,1,2744` / `6,1,30191` | `challengeoftime\award.lua:22 {0.05, {"LÖnh bµi Phong L¨ng §é", 489}}` (byte TCVN3) ↔ `fengling_ferry\fld_head.lua:269 if (itemgenre ~= 4 or detailtype ~= 489)`. `chuangguanbaoxiang.lua:17-20 tbCOT_Key_Require` = `{6,1,2744}` / `{6,1,30191}`; `activitysys\config\17\vnshuizeibaoxiang.lua:14-17 tbPirate_Box_Key_Require` = **y hệt**; hai tệp cùng có `_Message` chết (`:9-15` vs `:6-12`) và cùng dùng `%tbvng_ChestExpAward`. `grep -i "vượt ải\|challengeoftime\|chuangguan" 02_phonglangdo.md` = **0** | ✅ **ĐÚNG** | giữ nguyên |
| **B2** `SetTaskTemp(200,0)` bị comment ở **cả hai** đường khôi phục | `fengling_ferry\mission.lua:71` `--	SetTaskTemp(200,0);` (trong `OnLeave`) và `:116` `--		SetTaskTemp(200, 0)` (trong `Landing`); đường **đặt** `fld_head.lua:117 SetTaskTemp(200,1);` (còn `:133` là bản đặt cũ đã comment) | ✅ **ĐÚNG** | giữ nguyên |
| **B3** `RunMission()` chạy `SetFightState(1)`/`PutMessage` ngoài `if (pidx > 0)` | `mission.lua:30-32` gán `PlayerIndex` trong `if`; `:34 SetFightState(1)`, `:35 PutMessage(…)` ở mức thụt lề ngoài `if`, trước `:36 if (idx == 0) then break` | ✅ **ĐÚNG** | giữ nguyên |
| **B4** `Landing()` thủng chỉ số ⇒ **người sau lỗ hổng không được cập bến**, vì `getn` Lua 4.0 dừng ở `nil` đầu tiên | **Bác bỏ cơ chế.** `Sources\Library\LuaLib\src\lapi.c:458-476` — `lua_getn` trả `t.n` nếu là số, **ngược lại quét toàn bảng băm lấy KHOÁ SỐ LỚN NHẤT**. Xác nhận trên chính ELF Linux: `luaB_getn` `0x0823D2C0` → `lua_getn` **`0x08233070`**; tại đó `luaH_getstr(h,"n")` (hằng chuỗi `"n"` @ `0x8266214`), nếu không phải số thì vào vòng `0x082330D0-0x082330F5` bước node `0x1c`, `fucomi` giữ `max` rồi `fistp` trả về. Và `{}` **không đặt `n`**: `lparser.c:546-593` — `constructor_part` gặp `}` ngay thì `cd->n = 0`, **không phát `OP_SETLIST`**; `lvm.c` `case OP_SETLIST` chỉ `luaH_setint`, không ghi `n` | ❌ **SAI** (số dòng đúng, **cơ chế và hậu quả sai**) | "…`getn(tbPlayer)` = **chỉ số lớn nhất đã gán**, nên vòng `:110` vẫn duyệt hết mọi người; hỏng ở chỗ **đúng vòng lặp trúng lỗ hổng chạy với `PlayerIndex = nil`** rồi vẫn gọi `GetCamp/SetCurCamp/SetCreateTeam/NewWorld`." Mức độ hạ 🔴 CAO → 🟡 TRUNG BÌNH. **Đã sửa trong thân bài** |
| **B7** `AddMSPlayer(…,1)` vs `DelMSPlayer(…,0)` | `fld_head.lua:138 AddMSPlayer(MISSIONID,1)`; `fld_death.lua:12 DelMSPlayer(MISSIONID, 0)`; `fld_head.lua:153 GetMSPlayerCount(MISSIONID, 1)` | ✅ **ĐÚNG** | giữ nguyên |
| **B9** `boss.lua` kéo theo **3 `IncludeLib` và 14 `Include`** | `boss.lua` 83 dòng: `grep -c "Include("` = **11** (dòng 2,3,4,7,8,10,11,12,13,14,15); `grep -c "IncludeLib"` = **3** (dòng 5 `ITEM`, 6 `LEAGUE`, 9 `TASKSYS`) | ❌ **SAI (số)** — 11 chứ không phải 14 | "3 `IncludeLib` và **11** `Include` (**tổng 14 dòng nạp**)". **Đã sửa trong thân bài** |
| **C14** 9 tên bảng/hằng "grep = 0" trong `03_vuotai.md` | Chạy lại từng tên: `tbFightNpc`=0, `tbNormalNpc`=0, **`tbHidenNpc`=5**, `map_series`=0, `tbMapList`=0, `tbFreeMaps`=0, `tbRegist`=0, `tbCOT_Key_Require`=0, `tbDouble`=0, `tbRoleName`=0, `tsk_rank_lastscore`=0, `tsk_rank_2thscore`=0, `BOAT_POSY`=0, `VARV_SIGNUP_POSY`=0, `SEX_BOY`=0 | ❌ **SAI một phần** — `tbHidenNpc` có mặt 5 lần (`03_vuotai.md` dòng 225, 407, 494, 1045, 1143, trong đó có cả phụ lục phản biện) | "**8** tên … (bỏ `tbHidenNpc`)". **Đã sửa trong thân bài** |
| **A6 / C4** bản `src_utf8` "đang sai **104 dòng / 16 tệp**" | Quét lại U+FFFD toàn cây `src_utf8`: **đúng 104 dòng / 16 tệp**, phân bố khớp (award 36, npc 36, battleinfo 6, 4× `task_head` 3, chuangguan 3, `yandibaozang\head` 3, 2× dragonboat_main 2, nieshichen **2**, mibao_head 1, 4× tong 1). **Nhưng** đếm theo tên vật phẩm: `award.lua:9-53` = **45/45 dòng sai tên** mà chỉ **35** có U+FFFD; `npc.lua:260-303` = **44/44 sai** mà chỉ **34** có U+FFFD. Ví dụ `award.lua:9` và `:22` **không có U+FFFD** nhưng vẫn ra `"Tinh H錸g B秓 Th筩h"` / `"L謓h b礽 Phong L╪g ч"` (byte gốc đọc cp1258 = `"Tinh Hång B¶o Th¹ch"` / `"LÖnh bµi Phong L¨ng §é"`) | ⚠️ **ĐÚNG SỐ, SAI PHẠM VI** — 104 là **cận dưới**, không phải tổng số dòng hỏng | "≥104 dòng; U+FFFD **không phải** chỉ dấu đủ — riêng 2 bảng `map_random_awards` đã là **89** dòng hỏng thay vì 72". **Đã sửa trong thân bài** |
| **E.1** 160 boss / 8 nhóm / 40 map / 26 hàm lõi / 31 hàm engine trực tiếp | Bóc bảng `killbosshead.lua:5-180`: **160 hàng**; cột 10 = **1..160 liên tục, không trùng**; **40** map id — **trùng khít danh sách** mục 16; **59** id mẫu NPC, dải 761→820, **không có 769** ✅; cột 6 luôn `0`, cột 8 luôn `1`, cột 9 luôn `kill_level.lua`; 8 dòng mốc comment ở `:5, 27, 49, 71, 93, 115, 137, 159` ✅ | ✅ **ĐÚNG** (khớp 100 %) | giữ nguyên |
| **E.2** "3 boss" là thiếu — thật ra 3 × NPC 725 + 2 × NPC 1692 | `fld_smalltimer.lua:37, :44, :60` mỗi dòng `AddNpc(725, 85, …)`; `:49-54` vòng `for i = 1, 2 do … AddNpc(1692, 85, …)` trong `if (check_new_shuizeitask() == 1)`. `MISSIONID` qua `fld_head.lua:10 = 15` ✅; `:12 boatMAPS = {337,338,339}` ✅; `:153 >= 100` ✅; `mission.lua:20 for i=1, 30` + `:22 AddNpc(724, 95, …)` ✅, `TNPC_THIEF_COUNT = 30` (`fld_head.lua:17`) đúng là hằng chết | ✅ **ĐÚNG** | giữ nguyên |
| **E.3** 28 ải × 2 cấp / 32 map / 2 chế độ | `npc.lua`: mốc `-- N` trong `tbNormalNpc` cấp 1 (`:27-122`) = **28 mốc 1..28**, cấp 2 (`:132-227`) = **28 mốc 1..28**; `include.lua:83-88 tbLevels` = `{50,90}` / `{90,201}`; `:90-103 tbLevelMaps` = 464-479 (16) + 480-495 (16) = **32**; `tbFightNpc` `:25`, `tbHidenNpc` `:123` và `:228` | ✅ **ĐÚNG** | giữ nguyên |
| **§F** chỉ có **3** tệp `.bak` trong `server1\script`; `Patch\script` chỉ có 9 thư mục, không có `missions`, không có `task\tollgate` | `find server1\script -name "*.bak"` = **đúng 3** (`event\prize\loginprize.lua.bak`, `fengling_ferry\fld_smalltimer.lua.bak`, `fengling_ferry\mission.lua.bak`). `Patch\script` = đúng 9 thư mục đã nêu; `Patch\script\task` chỉ chứa `guide\{head,npc}.lua` ⇒ **không có** `task\tollgate` ✅ | ✅ **ĐÚNG** | giữ nguyên — nhưng xem "Bỏ sót" §2: `Patch` còn **hai nhánh khác** chưa ai mở |
| **closure3.json** khoá `vuotai` thiếu `vng_feature\challengeoftime\npcNhiepThiTran.lua` | `closure3.json`: `satthu` 67 mục — **có** `{"rel":"vng_feature/challengeoftime/npcNhiepThiTran.lua","seed":false,…}`; `phonglangdo` 82 mục và `vuotai` **103 mục — không có**. Trên đĩa: `src_utf8\satthu\vng_feature\challengeoftime\npcNhiepThiTran.lua` **có**, `src_utf8\vuotai\vng_feature\` chỉ có `forbiditem\` | ✅ **ĐÚNG** | giữ nguyên |
| **A2 / A3 / A5 / A7 / A8 / A9** (nhóm nhỏ hệ Sát Thủ) | A2: `lib_killlevel.lua:8-13` đúng là đặc tả 4 mã trả về, thân `:21-68` **không có `return`**, `myChangeMember` `:26` → `:57` rồi vứt ✅. A3: `tbMaiDian` `:134-138` đúng 3 chuỗi, `:145 AddStatData(tbMaiDian[i], tbshashou[i])`, `tbshashou = {2,1,2}` `:123-127` ✅ (`grep jiefangri_shashou 01_satthu.md` = 0). A5: `kill_level.lua:47,50,53,56,59,62,65` = **4,5,6,7,8,9,10** (đều comment) và `:69` = **10** ✅. A7: `nieshichen.lua:57 UWorld1082` (toàn cục), `:167` gán 5 biến không `local`, `:202,203,205` gán `series,i,j` không `local` ✅. A8: `nieshichen.lua:54 killertabfile = new(KTabFile, …)` đúng là lệnh **mức tệp** ✅. A9: `kill_level.lua:68 completeMission_Killer()` không tham số vs khai báo `(tbPlayers)` ✅ | ✅ **ĐÚNG** cả 6 | A5 vẫn **CHƯA XÁC MINH** ý nghĩa tham số, nhưng đã tiến thêm một bước trên ELF (xem "Bỏ sót" §4). A8: đường dẫn thật viết bằng **gạch chéo xuôi** `"/settings/task/tollgate/killer/killer.txt"`, không phải `\settings\…` như mục 16 chép lại — chi tiết này quan trọng khi port sang JX1 |
| **B1 / B5 / B8 / B10** (nhóm nhỏ Phong Lăng Độ) | B1: `mission.lua:68 Msg2MSAll(MISSIONID, GetName().."…tử vong…")` nằm trong `OnLeave` ✅; `fld_death.lua:4 Msg2Player` + `:12 DelMSPlayer` ✅; §6.2 của `02` (dòng 312) đúng là chỉ liệt kê **6** lệnh khôi phục, **không có** `Msg2MSAll` ✅. B5: `fld_head.lua:121 PlayerFunLib:CheckTaskDaily(3070, 5, "nomsg", "<")`, `:122 AddTaskDaily(3070, 1)`, `grep` trong `02` = 0 ✅. B8: `shuizeideath.lua:6-8` comment `nRate = 0.01` + `random(10000000)`, `:9-10 random(1,100) < 50` ✅. B10: `fld_smalltimer.lua:5 t=`, `:36/:43/:50/:59 posx, posy =` — đều không `local` ✅ | ✅ **ĐÚNG** cả 4 | B1: nói rõ thêm — `02_phonglangdo.md` **có** trích `mission.lua:68` ở bảng hàm engine (dòng 368, `Msg2MSAll` `0x08134280`); cái thiếu là **phân tích hành vi**, không phải thiếu dòng. Tiền đề "engine gọi `OnLeave` khi `DelMSPlayer`" **CHƯA XÁC MINH trên ELF** |
| **C2 / C6..C13** (nhóm nhỏ Vượt Ải) | C2: `dragonboat_main.lua:58 for i=20,80,10` vs `npcnhiepthitran.lua:50 for j = 20, 90, 10`, thoại `:69` ghi "cấp 90 trở xuống" ✅. C6: `:128 local nPrisonId = 208`, `grep nPrisonId 03` = 0 ✅. C7: `rank_perday.lua:16 tonumber(GetLocalDate("%H%M"))`, `:17 if (ntime < 4 or ntime > 2300)` ✅ (cửa thật 00:04→23:00). C8: `:79 for i = 1, 5`, `:82` chỉ chặn `i == 1`, `:86-88` `break` bị comment, `:78/:80` biến toàn cục ✅. C9: `mission_match.lua:22-24` đúng 3 dòng comment `--由relay通告报名开始，2次` / `--local min…` / `--broadcast(…)` ✅. C10: `include.lua:176 GetMSPlayerCount(MISSION_MATCH, 1)` vs `:177 GetNextPlayer(MISSION_MATCH, index, 0)` ✅. C11: `include.lua:384 map_posfiles[…]` vs `npc.lua:11-19` định nghĩa, `npc.lua:6` Include include.lua ✅. C12: trong 171 dòng, `_Message` = **1 hit** (chính dòng `:9`), `CallBack` = **0** ✅. C13: **11** `pFun` đúng ở `:26,32,38,44,50,76,82,88,94,100,106`; `nWidth/nHeight/nFreeItemCellLimit` `:114-116` ✅ | ✅ **ĐÚNG** cả 10 | giữ nguyên |
| **Bảng bộ tệp hạt giống** (đầu mục 16): satthu 7 tệp/3 998 dòng/3 242 comment · PLĐ 13/832/40 · Vượt Ải 17/2 543/278 | Đếm lại: satthu **7 tệp / 3 995 dòng / 3 272 comment**; PLĐ **13 / 826 / 40**; Vượt Ải **17 / 2 528 / 278**. Số **tệp** khớp cả 3; số **dòng comment** khớp tuyệt đối cho PLĐ (40) và Vượt Ải (278) | ⚠️ **KHÔNG XÁC MINH ĐƯỢC** khác biệt 3/6/15 dòng | Chênh nằm ở quy ước đếm dòng cuối tệp / định nghĩa "dòng comment"; **không kết luận nào phụ thuộc mấy con số này**. Riêng bộ số A10 (243/3 144/3 421) thì khớp **tuyệt đối** |

**Tổng kết đối chất:** 41 khẳng định đã kiểm → **3 SAI** (B4 cơ chế, B9 số, C14 danh sách) + **1 ĐÚNG-SỐ-SAI-PHẠM-VI** (A6/C4 "104 dòng") + **1 lệch nhỏ ở tóm tắt** (17 → 18 dòng upvalue) + **1 KHÔNG XÁC MINH ĐƯỢC** (bảng bộ tệp hạt giống). **35/41 khẳng định đúng nguyên văn.** Đáng chú ý: **mọi mục "thiếu / không có / chặn tiến độ" quan trọng nhất (A10, C1, C3, D1, D2, D3, D6) đều ĐỨNG VỮNG** sau khi kiểm chéo cả hai gốc dữ liệu.

---

### Bỏ sót của chính vòng 2

**1. C1 bỏ mất hoá thạch chứng minh chính nó — `nTaskItem` là biến chết.**
`missions\challengeoftime\npc\dragonboat_main.lua:29`:
`local nTaskItem = CallPlayerFunction(nPlayerIndex, CalcItemCount, 3, 6, 1, 400, 90)`
`grep "nTaskItem"` trong toàn tệp = **đúng 1 hit** (chính dòng khai báo) ⇒ **không ai dùng**.
Tham số `3` = **hành trang**, cấp `90` — **khớp chính xác** với `ConsumeItem(3, 1, 6,1,400,90)` ở `:142`.
⇒ Đây là dấu vết cho thấy cửa kiểm cấp 2 **vốn là kiểm TÚI** (`CalcItemCount(3,…)`), sau đó bị thay bằng
`CalcEquiproomItemCount` ở `:65` mà **quên đổi đường trừ**, để lại `nTaskItem` mồ côi.
Mục C1 lập luận đúng nhưng **thiếu bằng chứng nội tại mạnh nhất** này; khi port sang JX1 phải quyết:
theo `:65` (rương — ý bản mới) hay theo `:29+:142` (túi — ý bản gốc) — **không được giữ cả hai**.

**2. `D:\ServerLinux\Patch` còn HAI nhánh mà cả vòng 1 lẫn vòng 2 chưa mở: `Patch\maps` và `Patch\vng_script`.**
§F của mục 16 chỉ tuyên bố về `Patch\script` và `Patch\settings`. Thực tế `Patch\` còn:
- **`Patch\maps` — 52 tệp**: 42 tệp `.map`, 8 tệp `.dat`, thêm `map.dat` và `map id.txt` (**0 byte**).
  **Không một tệp nào trong số đó có bản đối chiếu trong `server1`**: `server1\maps` chỉ chứa
  `map_publish\` (6 mục), `mapgs_01..08.ini`, `worldset.ini`, `rongyuxitong` — **không có tệp `.map` số nào**;
  dữ liệu bản đồ của gốc A nằm **đóng gói** trong `server1\pak\maps.pak`. `find D:\ServerLinux -name "75.map"`
  trả về **đúng 1 kết quả** và nó nằm ở `Patch`.
  **Bốn trong 52 tệp đó — `75.map`, `93.map`, `225.map`, `321.map` — nằm ĐÚNG trong danh sách 40 map boss Sát Thủ**
  (mục E.1 đã liệt kê 75, 93, 225, 321). Ai port mà chỉ lấy map từ `maps.pak` sẽ dùng **bản cũ** cho 4 map này.
- **`Patch\vng_script`** — 1 tệp (`item\tuiduocphamdacbiet.lua`); đã kiểm: **không liên quan** 3 tính năng.
  (`server1\vng_script` có 175 tệp, `grep` 3 tính năng = **0 hit** ⇒ cũng không liên quan.)

**3. Câu hỏi treo của §F đã trả lời được — và giả định trong ghi chú điều phối về `bosstask_lev*.ini` là SAI.**
§F ghi "chưa kiểm hết 467 tệp settings chỉ có ở `Patch`". Đã đối chiếu **toàn bộ** `Patch\settings` (1 754 tệp)
với `server1\settings` theo MD5:
- **1 283 tệp giống hệt từng byte**, **467 tệp chỉ có ở `Patch`**, và **chỉ 4 tệp khác nội dung**:
  `gamesetting.ini`, `product_config.ini`, `tong\workshop\bingjia_level_data.txt`, `tong\workshop\workshops.txt`
  ⇒ **không tệp nào trong 4 tệp lệch đó thuộc 3 tính năng**.
- Trong **467 tệp chỉ-có-ở-`Patch`**, phần chạm 3 tính năng đúng bằng **1 tệp**:
  `maps\中原北区\渡船\渡船刷怪点.txt` (893 B) = `npcthiefpos` của `fld_head.lua:18`
  (báo cáo 02 đã biết thiếu tệp này). Ba tệp lân cận `风陵渡北岸\baiyingyingboss.txt`,
  `风陵渡南岸\herenwoboss.txt`, `风陵渡北岸\yanxiaoqianboss.txt` thuộc **hệ boss dã ngoại**, không phải mission.
  Hai tệp `droprate` chỉ-có-ở-`Patch` là `droprate\goldennpc\各迷宫所使用droprate对照.txt` và
  `各种类droprate文字描述.txt` — **tài liệu mô tả, không phải bảng chạy**.
- 🔴 **Đính chính giả định điều phối:** 8 tệp `settings\droprate\boss\bosstask_lev20..90.ini`
  (đường rơi đồ của boss Sát Thủ, `kill_level.lua:47-69`) **CÓ Ở CẢ HAI GỐC và giống hệt từng byte** —
  **không phải "chỉ có ở B"**. Tương tự `settings\task\tollgate\killer\killer.txt` (14 033 B),
  `settings\huoyuedu\huoyuedu.txt` (1 865 B) và cả 7 tệp `settings\maps\challengeoftime\lineup*.txt`
  đều **trùng byte** ở hai gốc ⇒ **3 tính năng này KHÔNG dính bẫy hai gốc ở tầng `settings`**, chỉ dính đúng
  một tệp `渡船刷怪点.txt` và (mới) tầng `maps` ở §2 trên.

**4. A5 tiến thêm được một bước trên ELF (mục 16 để "CHƯA XÁC MINH" hoàn toàn).**
`ITEM_DropRateItem` = **`0x08154DE0`** (`jx_linux_y.luamap.full.txt` dòng 961). Tháo mã:
- `0x08154E08 cmp eax, 5 / jg` ⇒ **bắt buộc > 5 tham số** (tức ≥ 6) — 8 lời gọi trong `kill_level.lua` đều 6.
- tham số **1** → `[ebp-0x34]`, dùng ở `0x08154F43 imul eax, [ebp-0x34], 0x1a4c` ⇒ **chỉ số NPC** nhân cỡ struct `0x1a4c`.
- tham số **2** → `[ebp-0x30]`, ép về int, **phải > 0** (`0x08154F3D jle → return`), là **cận trên vòng lặp** (`0x08154F62`) ⇒ **số lượt rơi** (= 8).
- tham số **3** → `lua_tostring` `0x08233850`, **phải khác NULL và khác chuỗi rỗng** (`0x08154E71/76`) ⇒ đường dẫn `.ini`.
- tham số **4** → `0x08154EAF test edx,edx / setne dl` ⇒ **bị ép về CỜ 0/1 (boolean)**, không phải số lượng. Mọi lời gọi đều truyền `0`.
- tham số **5** → `[ebp-0x3c]`, giữ **nguyên giá trị int**, được đẩy tiếp ở `0x08154F85 mov [esp+0x14], eax`.
- tham số **6** → `[ebp-0x40]`, int (`nseries`).
⇒ **Đã bác được** cách ghi "hai dấu `?`" của §7 báo cáo 01: tham số 4 là **cờ boolean**, tham số 5 là **số nguyên**.
Ý nghĩa cụ thể của tham số 5 (và vì sao dãy `4..10` rồi lặp `10` ở cấp 90) **vẫn CHƯA XÁC MINH**.

**5. Hai tiền đề của mục 16 chưa được chứng minh — đừng port theo chúng như sự thật.**
- **B1** giả định "engine gọi `OnLeave` mỗi khi rời mission **vì bất cứ lý do gì**, kể cả `DelMSPlayer`".
  Chưa dò trên ELF. Nếu `DelMSPlayer` **không** kích `OnLeave` thì cả B1 (spam thông báo) lẫn hệ quả
  "cờ 200 mang ra ngoài" của B2 đổi nghĩa hoàn toàn. **Phải xác minh trước khi port.**
- **B3/B4** giả định `GetNextPlayer` có thể trả `pidx <= 0` **ở giữa** danh sách. Chưa dò trên ELF.
  Nếu `pidx <= 0` chỉ xảy ra ở **cuối** (hết danh sách) thì cả hai lỗi chỉ còn là lỗi lý thuyết.
