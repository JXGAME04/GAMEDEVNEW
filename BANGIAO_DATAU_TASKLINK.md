# BÀN GIAO — DÃ TẨU BẢN GỐC LINUX (tasklink) TRÊN MÁY CHỦ JX1

Ngày: 15/08/2026 · Manifest chi tiết: `DANHSACH_DATAU_PORT.md` · Phân tích nền: `PHANTICH_DATAU_LINUX_GOC.md`
Trạng thái: **script + bảng + C++ đã vào chỗ, Core x64 build PASS, DLL đã chép — CHỜ RESTART GAMESERVER + TEST**

## 1. Đã làm gì (tóm tắt)

| Khối | Nội dung | Trạng thái |
|---|---|---|
| Script | 51 file bao đóng tasklink chép nguyên byte từ `D:\ServerLinux\server1` vào `bin\server\script\...` (+ awardtype simple/exp + shim `item_jx1.lua`) | ✅ |
| Bảng dữ liệu | 17 bảng + translife.txt vào `settings\task\` | ✅ |
| Vá script (C1–C6) | 6 mối vá ASCII, mỗi mối có lý do trong manifest | ✅ byte-safe |
| C++ | 17 hàm mới trong `ScriptFuns.cpp` + nới `MAX_PARAMLIST_COUNT` 5→8 (`KPlayer.cpp`) | ✅ build PASS |
| Binary | `CoreServer.dll` x64 Server Release build 20:22 15/08, đã chép vào `bin\server\Coreserver.dll` | ✅ |
| NPC | 10 điểm `AddNpcNew` (template 108, NameID 59) trỏ sang `\script\global\seasonnpc.lua` | ✅ |
| Item | 205/212 đã trỏ sẵn `tasklink_goods*.lua` từ trước (magicscript khớp nguyên trạng) | ✅ |
| Dữ liệu remap | `award_basic.txt` 31 dòng remap theo TÊN; `tasklink_findmaps.txt` lọc 28/208 dòng theo map đã nạp (bản gốc giữ ở `*.goc`) | ✅ |
| GameServer.exe | KHÔNG cần build lại (không đổi export) | — |

**Hệ Dã Tẩu cũ (Fong Kiều 2021) tự ngắt** vì 10 AddNpcNew trỏ đi nơi khác — không xóa file nào. Muốn quay lại: đảo 10 dòng AddNpcNew + trả 3 file stub (nội dung stub ghi trong manifest mục A2).

## 2. CÁCH BẬT (thứ tự bắt buộc)

1. Đảm bảo `bin\server\Coreserver.dll` là bản 15/08 20:22 (**scripts mới + DLL cũ = thiếu hàm, hộp thoại chết**).
2. Restart **GameServer** (Relay/client KHÔNG cần — không đụng protocol).
3. Xem log boot: `grep "Include HONG\|IncludeLib HONG" DebugLog` — phải 0 dòng thuộc `tasklink|seasonnpc|awardtype`.
4. Vào game gặp **Dã Tẩu** ở 1 trong 10 thành/thôn (Thành Đô 3154,5067 v.v.) — hội thoại phải là "khảo nghiệm nhiệm vụ liên tục" (KHÔNG phải menu hệ cũ).

## 3. KHÁC BIỆT CÓ CHỦ ĐÍCH so với bản gốc (đầy đủ, không giấu)

| # | Khác biệt | Lý do | Muốn về gốc thì |
|---|---|---|---|
| 1 | `do return end` (VNG 20141030) đã gỡ | Đây là dòng VNG THÊM để đóng NPC — gỡ = khôi phục gốc | (chính là gốc) |
| 2 | Loại 5 phúc duyên đọc `GetFuYuan()` thay `nt_getTask(151)` | JX1 không ai ghi task 151 → nhiệm vụ không bao giờ xong | vá C2 trong manifest |
| 3 | Loại 5 Tống Kim đọc `GetTask(38)` thay `nt_getTask(747)` | tích lũy TK bên JX1 nằm ở task 38 | vá C3 |
| 4 | seasonnpc thêm 2 Include awardtype cuối khối Include | JX1 mỗi file 1 Lua state; đặt sớm hơn bị dailogsay reset bảng TYPE | vá C4 |
| 5 | `tbProp` handler = `item_jx1.lua` trên `AddItem` (không port `NewItemEx` 17 tham số) | NewItemEx là API tạo item JX2, không có bên JX1 | shim, hành vi trao giống |
| 6 | `AddGoldItem` trong `newtask_head.lua` nhận cả 2 kiểu gọi | mySG gọi kiểu JX2 `(0, mãHK)`; shadow Lua cũ nuốt mất | vá C5 |
| 7 | **Mốc-10 (rương 6/1/2374) TẠM KHÓA** bằng cờ `DATAU_MOC10_BAT` | 2374 bên JX1 = "Băng bạch kim" (vật liệu quý) — phát nhầm là sự cố kinh tế | xem mục 4.1 |
| 8 | `tasklink_findmaps.txt` lọc còn 28 dòng / 14 map | 180 dòng trỏ map JX1 chưa nạp → nhiệm vụ không thể hoàn thành | `.goc` giữ nguyên bảng gốc |
| 9 | `award_basic.txt` remap 31 dòng id (Huyền Tinh 147→146 giữ cột Level; nguyên thạch/lộ ±1 theo TÊN) | bảng item 6/1 hai bên lệch cục bộ ±1 | `.goc` giữ bảng gốc |
| 10 | Storm/TK ngủ đông (`TM_*`, `BT_*` stub) — `storm_addpoint` tự bỏ qua | hệ sự kiện Storm không tồn tại bên JX1 | port Storm (việc riêng) |
| 11 | Hook hoạt-động-độ `huoyuedu` ngủ đông (`DynamicExecuteByPlayer` log-rồi-bỏ-qua khi script chưa có) | JX1 chưa có hệ huoyuedu | port huoyuedu (việc riêng) |
| 12 | `GetTiredDegree()` trả 0 | bản gốc TỰ GÁN ĐÈ 0 trong TireReduce — 0 là hành vi nguyên bản | — |
| 13 | Cửa sổ chọn thưởng `Prise` = hộp thoại 3 nút chuẩn JX1 (không icon rương như client JX2) | UI client JX2 không tồn tại; logic chọn + khóa seed giữ nguyên | sửa client (việc riêng) |
| 14 | Nhánh 1475 "Tích lũy Dã Tẩu" ngủ đông (`SetItemMagicLevel` stub) | chỉ chạy khi TireDegree==2 (không bao giờ) và 1475 không nằm trong bảng thưởng | — |

## 4. VIỆC CHỦ GAME CẦN QUYẾT

### 4.1 Item rương mốc-10 (đang khóa)
Bản gốc: mỗi 10 nhiệm vụ tặng 1 "Bảo rương thần bí của Dã Tẩu" (6/1/2374 JX2). Chọn item JX1 thay thế (gợi ý: 4850 Rương Trang Bị Xanh — hệ cũ phát 2 cái/ngày ở mốc 40; hoặc item tự chế). Sửa trong `script\global\seasonnpc.lua` (bản đã chép): thay `2374` bằng id mới, đặt `DATAU_MOC10_BAT = 1` ở đầu file (hoặc bỏ điều kiện). Tần suất tối đa: 4 rương/ngày/người (trần 40 nv).

### 4.2 Người chơi đang dở nhiệm vụ hệ CŨ
Task hệ cũ (12,13,14,87–99,317,318) thành mồ côi — không lỗi, chỉ dở dang. Mảnh Sơn Hà Xã Tắc hệ cũ nằm ở **task 75**, hệ mới đếm ở **task 1027**. Nếu muốn chuyển: script GM 1 dòng `SetTask(1027, GetTask(1027) + GetTask(75)); SetTask(75, 0)` — cần chủ game gật đầu (kinh tế).

### 4.3 NPC Xa Phu
Menu "Đến nơi làm nhiệm vụ dã tẩu/godatau" của Xa Phu vẫn đọc bảng hệ CŨ (`station.lua`) — giờ thành mục chết vô hại. Muốn gỡ/để lại tùy chủ game.

## 5. HƯỚNG DẪN TEST (theo 6 loại + khung)

| # | Test | Kỳ vọng |
|---|---|---|
| 0 | Boot GS, xem `ScriptError.log` + DebugLog | không lỗi mới thuộc bao đóng Dã Tẩu |
| 1 | Gặp Dã Tẩu lần đầu → "Được thôi!..." (Task_Confirm) | nhận nhiệm vụ đầu, thoại ngẫu nhiên có tên tiệm/người/vật |
| 2 | Loại 4 (địa đồ — tỉ lệ cao nhất): đến map yêu cầu, giết quái nhặt cuộn **205** click phải | đủ số → về trả → cửa sổ 3 phần thưởng (Prise) |
| 3 | Chọn từng loại thưởng: Tiền / Exp / Vật phẩm / "Đổi lần nữa" / Lượt hủy | nhận đúng; **bấm lại không nhận lần 2** (course=3) |
| 4 | Thoát game NGAY khi cửa sổ 3 thưởng đang mở, vào lại, gặp NPC | mở lại ĐÚNG 3 ô cũ (seed 1037 — SetRandSeed/C_Random) |
| 5 | Loại 1 (mua đồ): mua đúng món theo thoại, GiveItemUI nộp | khớp 5 trường thì nhận thưởng, đồ bị xóa |
| 6 | Loại 2/3 (đồ có thuộc tính): nộp/khoe món có dòng "tăng sinh lực/nội lực..." đạt min | loại 3 KHÔNG mất đồ |
| 7 | Loại 5: nhận nv tăng KN/danh vọng/phúc duyên/PK/TK rồi cày đủ hiệu số | xong; đặc biệt phúc duyên (GetFuYuan) và TK (task 38) |
| 8 | Loại 6: gom 10 mảnh SHXT (nhặt cuộn sai map sẽ ra mảnh → task 1027) | nộp trừ đúng số mảnh |
| 9 | Hủy: hủy >2 lần ở đầu chuỗi → NPC giận 605s; hủy bằng 100 mảnh; lượt hủy từ thưởng | khớp _CancelTaskDebug (task 1046) |
| 10 | Trần ngày: làm 40 nhiệm vụ | câu "Mỗi ngày làm 40 lần là đủ rồi", mai reset (task 2419/2420) |
| 11 | Mốc 30/40 liên tục | +30tr / +100tr exp qua `nExp_tl` (awardtype exp.lua) — **đây là test tbAwardTemplet hết rỗng ruột** |
| 12 | Đồ vàng: chỉnh `award_basic` cho ra dòng Quality=1 (GM) | ra ĐÚNG món An Bang/Định Quốc cùng tên (offset đã kiểm = 0) + loan báo toàn server |
| 13 | Cống hiến bang: làm nv khi có bang | cống hiến +floor(giá trị/8) qua `ContriValueEntryLogic` (hệ đợt 12) |
| 14 | Tổ đội nhặt cuộn 205/212 | cuộn chia cho cả tổ (`GetTeamMember` alias) |

## 6. HÀM C++ MỚI (ScriptFuns.cpp, cụm sau `{"IL",...}`)
`C_Random, SetRandSeed, GetTiredDegree, GetTeamMember(=GetTeamMem), GetBitTask, SetBitTask, GetItemMagicAttrib, SetItemMagicLevel(stub), SyncItem, curpack, usepack, CallPlayerFunction, GetLastFactionNumber, TM_SetTimer, TM_GetRestCount, BT_GetGameData, BT_GetData, Prise, DynamicExecuteByPlayer` — thân hàm nằm ngay trước `LuaIncludeLib`, mỗi hàm có chú thích lý do. `MAX_PARAMLIST_COUNT` 5→8 vì `mySG` nhận 6 tham số (nội bộ server, không đổi protocol).

## 7. RỦI RO CÒN LẠI / ĐÃ BIẾT
1. **Chưa boot test** — con số "0 hàm thiếu" là quét tĩnh; `ScriptError.log` sau restart là phép đo thật. Nếu còn `attempt to call a nil value`, tra tên hàm trong manifest mục D.
2. Enum thuộc tính 85–110 đã đối chiếu **khớp từng số** với `KMagicAttrib.h` (85=lifemax… 110), nhưng nên test thật 1 nhiệm vụ loại 3.
3. Offset bảng vàng kiểm bằng tên (offset 0 = công thức `nEventId+2`) — test GM 1 lần trước khi mở (mục 5.12).
4. `execute` (io) vẫn mở trong Lua — lỗ bảo mật CŨ có sẵn của cây, không do đợt này; nên cân nhắc gỡ (việc riêng).
5. buygoods (35 dòng, genre 0 = trang bị thường): chưa đối chiếu từng dòng với hàng bán trong shop JX1 — nếu người chơi báo "mua không có món X", tra dòng tương ứng trong `tasklink_buygoods.txt`.

## 8. FILE ĐÃ ĐỘNG VÀO
- **Repo:** `Sources/Core/Src/ScriptFuns.cpp`, `Sources/Core/Src/KPlayer.cpp`, `serverscript_jx2/datau_tasklink/**` (70 file), `serverscript_jx2/jx1_edits/datau/**` (11 bản sao file JX1 bị sửa), `DANHSACH_DATAU_PORT.md`, `PHANTICH_DATAU_LINUX_GOC.md`, tài liệu này.
- **Runtime E:\...\bin\server:** script/settings như manifest + `Coreserver.dll` + 10 file startgame + `newtask_head.lua`.
- **KHÔNG đụng:** `D:\ServerLinux\*` (nguồn gốc — chỉ đọc), protocol/packet/DB, GameServer.exe.
