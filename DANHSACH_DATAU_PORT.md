# DANH SÁCH PORT DÃ TẨU TASKLINK (bản gốc Linux → JX1) — 15/08/2026

Nguồn: `D:\ServerLinux\server1` · Đích chạy: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` · Repo: `D:\GAMEDEVNEW`
Nguyên tắc: **script gốc giữ nguyên byte**, chỉ vá đúng các điểm liệt kê ở mục C (mỗi điểm có lý do). Hàm thiếu viết vào C++.

## A. BAO ĐÓNG SCRIPT — 53 file, 399.252 byte (đã tính bằng máy, 0 file thiếu)

### A1. Chép mới (46 file KHÔNG va chạm — đường dẫn giữ nguyên `script\...`)
```
script\task\newtask\tasklink\tasklink_head.lua    35.866   LÕI (bản tl_*, KHÔNG lấy bản /global/)
script\task\newtask\tasklink\tasklink_award.lua   23.269   thưởng
script\global\fuyuan.lua                           6.588
script\task\task_addplayerexp.lua                  3.175
script\task\metempsychosis\task_head.lua           7.475
script\event\great_night\huangzhizhang\event.lua   2.969
script\lib\gb_taskfuncs.lua                        2.274   (trùng hệt scriptjx2\lib — vẫn chép để Include path gốc chạy)
script\tong\tong_award_head.lua                    4.595
script\tong\tong_header.lua                        6.722
script\tong\contribution_entry.lua                 7.488
script\tong\tong_setting.lua                       5.610
script\tong\log.lua                                1.651
script\event\storm\function.lua                   16.531
script\event\storm\head.lua                        8.610
script\event\storm\custom.lua                      8.331
script\lib\awardtemplet.lua                        1.392   (+vá C4)
script\lib\log.lua                                 4.318
script\activitysys\g_activity.lua                  2.613
script\lib\baseclass.lua                             841
script\lib\common.lua                              3.475
script\activitysys\activity.lua                    8.685
script\activitysys\activitydetail.lua              4.276
script\lib\pfunction.lua                           3.488
script\lib\composeex.lua                          14.325
script\dailogsys\dailogsay.lua                     4.810
script\lib\string.lua                              6.965
script\script_protocol\protocol_def_gs.lua         4.792
script\protocol.lua                                4.024
script\vng_lib\vngtranslog.lua                     1.007
script\vng_lib\files_lib.lua                       2.329
script\vng_lib\taskweekly_lib.lua                  1.029
script\activitysys\functionlib.lua                10.164
script\vng_lib\bittask_lib.lua                     1.389
script\vng_lib\extpoint.lua                        1.959
script\vng_lib\extpoint_head.lua                   2.871
script\misc\taskmanager.lua                       12.799
script\activitysys\ladderfunlib.lua                1.940
script\activitysys\playerfunlib.lua               18.746
script\item\class\virtualitem.lua                  1.221
script\global\itemset.lua                         68.641
script\global\seasonnpc_death.lua                  1.074
script\task\newtask\map_index.lua                 10.700
script\task\newtask\lib_setmembertask.lua          1.451
script\item\seasonnpc_item.lua                     1.835   (ngủ đông — chưa nối item)
script\global\forbidmap.lua                        3.087
script\item\seasonnpc_expitem.lua                  1.181   (ngủ đông)
+ script\lib\awardtype\simple.lua                  1.9K    (thêm để mốc 10/30/40 hoạt động)
+ script\lib\awardtype\exp.lua                     0.6K    (đăng ký nExp / nExp_tl)
+ script\lib\awardtype\item_jx1.lua                MỚI     (shim tbProp → AddItem, thay NewItemEx)
```

### A2. GHI ĐÈ stub JX1 (3 file — nội dung stub cũ ghi ở cuối mục này)
| File | JX1 hiện tại | Hành động |
|---|---|---|
| `script\global\seasonnpc.lua` | 110 B → `Include tinhnang\datau\datau.lua` | Đè bằng bản Linux 37.199 B (+vá C1) |
| `script\item\tasklink_goods.lua` | 99 B → `Include diadochi.lua` (hệ cũ) | Đè bằng bản Linux 2.131 B |
| `script\item\tasklink_goods_secret.lua` | 95 B → `Include matchi.lua` (hệ cũ) | Đè bằng bản Linux 2.145 B |

Stub cũ (để hoàn nguyên nếu cần): seasonnpc=`Include("\\script\\tinhnang\\datau\\datau.lua")`; goods=`Include("\\script\\tinhnang\\datau\\diadochi.lua")`; secret=`Include("\\script\\tinhnang\\datau\\matchi.lua")` (đều kèm 2 dòng comment Fong Kiều).

### A3. GIỮ NGUYÊN của JX1 (không chép đè)
| File | Lý do |
|---|---|
| `script\task\newtask\newtask_head.lua` | Bản JX1 = bản Linux + khối AddGoldItem chắp thêm; `nt_setTask/nt_getTask` giống hệt. 12 file khác của JX1 đang Include nó. Chỉ vá C5 |
| `script\task\task_award_extend.lua`, `script\task\system\task_string.lua`, `script\lib\objbuffer_head.lua` | md5 TRÙNG KHỚP bản Linux — đã có sẵn |
| `settings\npc\player\level_exp.txt` | Giữ bảng exp CỦA JX1 (tl_countuplevelexp phải tính theo thang exp JX1) |

### A4. KHÔNG PORT (lý do ghi rõ)
- `script\global\tasklink_head.lua` (bản 2004): mồ côi trên chính Linux, lỗi tràn exp + lỗi logic loại 5.
- `script\global\mel\lenhbaidatau.lua`: mồ côi trên Linux, dùng task 8000 vượt `MAX_TASK 3000`.
- `script\global\gm\dataulenhbai.lua` (item GM 5126): item đã bị gỡ khỏi magicscript sống của Linux; vòng AddItem×1000 nặng.
- `vng_script\features\quest_daily\*`: hệ Dã Tẩu ĐỜI SAU, chưa từng chạy trên Linux — không phải "bản gốc".

## B. BẢNG DỮ LIỆU — 17 file → `settings\task\` (+`talk\`), đích đang TRỐNG hoàn toàn
```
levellink.txt 53B · tasklink_mainlink.txt 844B · tasklink_buygoods.txt 6.048B
tasklink_findgoods.txt 78.964B · tasklink_showgoods.txt 5.959B · tasklink_findmaps.txt 19.329B
tasklink_upground.txt 4.189B · tasklink_worldmaps.txt 1.263B
award_basic.txt 4.735B · award_link.txt 5.182B · award_loop.txt 793B
talk\talk_{buygoods,findgoods,showgoods,findmaps,upground,worldmap}.txt (6 file ~6KB)
```
+ `settings\task\metempsychosis\translife.txt` nếu Linux có (metempsychosis/task_head nạp trong hàm — kiểm khi chép).

## C. CÁC ĐIỂM VÁ SCRIPT (safe_edit, ASCII-only — TOÀN BỘ danh sách, ngoài ra không sửa gì)
| # | File | Vá | Lý do |
|---|---|---|---|
| C1 | `seasonnpc.lua:119` (bản chép) | bỏ `do return end` | Dòng VNG THÊM 30/10/2014 để đóng NPC — bỏ = khôi phục bản gốc |
| C2 | `tasklink_head.lua:163,390` | `nt_getTask(151)` → `GetFuYuan()` | Task 151 = kho phúc duyên JX2; JX1 không ai ghi 151 → nhiệm vụ loại 5-phúc-duyên không bao giờ xong. GetFuYuan() là nguồn phúc duyên thật của JX1 |
| C3 | `tasklink_head.lua:167,404` | `nt_getTask(747)` → `GetTask(38)` | Task 747 = tích lũy Tống Kim JX2; bên JX1 tích lũy TK nằm ở task 38 (TASKVALUE_STATTASK_ACCUM) |
| C4 | `awardtemplet.lua` (bản chép) | thêm 3 Include awardtype ở cuối | Trên Linux 1-state, handler đăng ký từ pack khác; JX1 mỗi file 1 state → state seasonnpc phải tự nạp handler, nếu không mốc 10/30/40 lặng lẽ không trao gì |
| C5 | `newtask_head.lua` (file JX1) | lưu `_C_AddGoldItem` trước shadow; shadow nhận `(a,b)`: `b>0` → `_C_AddGoldItem(b)` | mySG gọi `AddGoldItem(0, genre)` kiểu JX2; shadow Lua 1-tham-số của JX1 nuốt mất → mất thưởng Hoàng Kim im lặng. Caller cũ của shadow là mã chết |
| C6 | 10 file `startgame\thanh|thon\*.lua` | đổi path trong AddNpcNew: `npcchucnang\datau.lua` → `global\seasonnpc.lua` | Nối NPC Dã Tẩu (template 108, NameID 59, 10 map) vào hệ mới |

## D. HÀM C++ VIẾT MỚI (ScriptFuns.cpp — đăng ký GameScriptFuns)
| # | Hàm | Cách làm |
|---|---|---|
| D1 | `C_Random(min,max)` | GetRandomNumber(min,max) có sẵn |
| D2 | `SetRandSeed(n)` | srand(n) — bản lề chống quay-lại-thưởng (khoá bộ 3 theo task 1037) |
| D3 | `GetTeamMember(i)` | alias LuaGetTeamMem (kiểm chữ ký khi viết) |
| D4 | `GetTiredDegree()` | trả 0 — bản gốc tự gán đè 0 nên đây là hành vi ĐÚNG NGUYÊN BẢN |
| D5 | `GetBitTask(id,s,l)` / `SetBitTask(id,s,l,v)` | bit ops trên GetSaveVal/SetSaveVal |
| D6 | `GetItemMagicAttrib(idx,i)` | trả `m_aryMagicAttrib[i-1].nAttribType, nValue[0], nValue[1], nValue[2]` |
| D7 | `SetItemMagicLevel(idx,slot,v)` + `SyncItem(idx)` | ghi nValue[0] + đồng bộ (mẫu LuaSetParamItem:5231) |
| D8 | `Prise(msg,s1,s2,s3)` | tách `nhãn/icon/tham số/hàm` → SelectUI 3 nút `nhãn/#hàm(tham số)`; cần nới `MAX_PARAMLIST_COUNT` 5→8 (KPlayer.cpp — mySG có 6 tham số; nội bộ server, không đụng protocol) |
| D9 | `DynamicExecuteByPlayer(idx,path,fn,arg)` | g_GetScript → thiếu script thì WriteLog + return (hook huoyuedu ngủ đông); fn dạng `a:b` xử lý method |
| D10 | `curpack()`→0, `usepack(n)`→0 | JX1 mỗi file 1 state — stub tương đương |
| D11 | `CallPlayerFunction(idx,fn,...)` | save global `PlayerIndex`, gán idx, gọi fn, khôi phục (GetPlayerIndex đọc global này — ScriptFuns.cpp:8746) |
| D12 | `GetLastFactionNumber()` | alias GetFactionNo (kiểm thứ tự đánh số 0-10 khi viết) |
| D13 | `TM_SetTimer`→0, `TM_GetRestCount`→nil | storm ngủ đông đúng nghĩa (`~= nil` → false → storm invalid → addpoint bỏ qua) |
| D14 | `BT_GetGameData`/`BT_GetData` → 0 | chặn mọi nhánh Tống Kim của storm |

Đã xác minh CÓ SẴN (không viết): GetItemProp:13215, Ladder_NewLadder:13097, FormatTime2Number, GetCurServerTime, GetTongName:12848, GetTong:12945, GetFuYuan, toàn bộ TONG_*/TONGM_*/Week-offer (cống hiến bang đợt 12), TabFile_*, GetOTItem, GiveItemUI, GetGiveItemUnit, RemoveItemByIndex, AddGoldItem(C++, tham số 1 = mã HK), Say/Talk/Msg2*, Earn/Pay/GetCash, CalcFreeItemCellCount, AddOwnExp, ST_GetTransLifeCount, GetGameTime, GetLocalDate, WriteLog, AddGlobalNews, AddGlobalCountNews, GetExtPoint, Ladder_*, SubWorldID2Idx, GetTeamSize.

KHÔNG cần viết (đường chết/ngủ đông): CalcEquiproomItemCount/ConsumeEquiproomItem (chỉ menglanjie — khoá ngày 060808-060815, chết vĩnh viễn), AddNpcEx/SubWorldName (seasonnpc_item ngủ đông), CreateNewSayEx (là hàm LUA trong dailogsay.lua), SendScriptData/Require (nhánh client), GetGlodEqIndex/GetPlatina*/GetItemGenTime... (getItemInfo — không nằm trên đường Dã Tẩu).

## E. KIỂM DỮ LIỆU CHÉO (trước khi mở cho người chơi)
1. 104 MapID trong `tasklink_findmaps.txt` — map nào JX1 không nạp → sửa dòng hoặc chấp nhận nhiệm vụ "đến map lạ" không ra.
2. `MagicEnName` trong findgoods/showgoods — đối chiếu bảng thuộc tính ma pháp JX1 (magicattr). Tên lệch → loại 2/3 không khớp được đồ.
3. `award_basic.txt` 98 dòng + `award_loop.txt`: Genre/Detail/Particular theo bảng item JX2 — dòng nào lệch bảng item JX1 phải remap (nhớ bẫy "lệch đều +1" của đợt phường). Dòng Quality=1 (HK) phải mang mã g_GoldItemTab của JX1.
4. Item đặc biệt: 2374 (rương mốc 10), 1475 (tích lũy — chỉ ra khi TireDegree=2, hiện không thể), 205/212 (đã có magicscript JX1 trỏ đúng path), Lục Thủy Tinh 4/240/1 (Task_TotalCancel).
5. Kênh nhặt đồ chia tổ đội trong tasklink_goods.lua PickUp — item 205/212 của JX1 kích PickUp qua magicscript (click phải). 2 hook C++ KPlayer.cpp:4748/4758 (PickItem) vốn NO-OP — không ảnh hưởng.

## F. TRIỂN KHAI + LƯU REPO
- Chạy thật: chép vào `E:\...\bin\server\script\...` + `settings\task\...`; thay `CoreServer.dll` build mới; **restart GameServer**.
- Repo: bản sao y ở `serverscript_jx2/datau_tasklink/` (script + settings); file JX1 bị sửa lưu thêm bản sao ở `serverscript_jx2/jx1_edits/datau/`.
- Hệ Dã Tẩu cũ (tinhnang/datau) TỰ NGẮT khi 10 AddNpcNew trỏ đi chỗ khác — không xoá file nào; muốn quay lại chỉ cần đảo C6 + A2.
