# BÀN GIAO 01/09 — MỔ BẢN LINUX: ĐỔI TÊN NHÂN VẬT + ĐỔI PHÁI (phân tích, CHƯA sửa gì)

Chủ yêu cầu: *"mổ bản linux xem có tính năng đổi tên nhân vật - đổi phái rồi phân tích"*.
Nguồn đúng luật: `D:\ServerLinux` (script + `jx_linux_y` disasm) · client VLTK
`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky` · nguồn JX1 `D:\GAMEDEVNEW` + script sống `E:\`.
4 tác tử điều tra độc lập (1,15 triệu token, 365 lượt đọc); phụ lục bằng chứng thô:
`ReverseTools\doiten_doiphai_ketqua_0109.txt`. Bảng 1.653 hàm engine Lua Linux kèm địa chỉ:
`ReverseTools\linux_api_names.txt` (tool `cauhinh\t80_trich_ten_api_linux.py`).

---

## 0. KẾT LUẬN NHANH

| Tính năng | Bản Linux | JX1 hiện có | Port "đúng chuẩn Linux" cần gì |
|---|---|---|---|
| **Đổi phái** ("Đại nghệ đầu sư") | **CÓ**, trọn vẹn, chạy thật | Engine đủ 90%; script sống có bản **tự chế** `lenhbaitanthu.lua` | **Chủ yếu SCRIPT** + 3 hàm Lua nhỏ (C++ vài chục dòng) + 1 sửa `RollbackSkill` |
| **Đổi tên nhân vật** | **CÓ**, bất đồng bộ qua Goddess/Relay, kick 3 phút | **HOÀN TOÀN KHÔNG** (0 API, 0 gói, 0 op DB) | **C++ 4 tầng** (Goddess + Core + S3Relay + log) — khoá CSDL nhân vật = TÊN nên rủi ro cao |

Cả hai tính năng phía **client KHÔNG cần UI mới** — Linux cũng chỉ dùng hộp nhập chuỗi chung
(`AskClientForString`) và hộp giao đồ (`GiveItemUI`), JX1 đã có cả hai (`KUiGetString2`, `KUiAffairItem`).

⚠️ **Phát hiện phụ cần xử lý sớm (không liên quan port):** script sống JX1
`global\skills_table.lua:70-110` đang gọi `SetLastFactionNumber` — hàm này **chưa đăng ký** trong
`ScriptFuns.cpp` ⇒ khi đường đó chạy sẽ ScriptError (gọi nil). Xem mục 4.1.

---

## 1. ĐỔI TÊN NHÂN VẬT — bản Linux vận hành thế nào

### 1.1 Kích hoạt (3 đường)
1. **Vật phẩm "Tinh Danh Chi Lệnh"** G6/D1/**P30448** (`settings\item\004\magicscript.txt:3199`, icon
   `\spr\item\script\tianbaokuling.spr`, script `\script\vng_event\item\rename_func.lua`,
   `forbititem.ini [Item_288]` cấm vứt). Dùng item **chỉ cấp quyền**: bật **bit 1 của task 2320**
   (`TASKVALUE_BLEND`) + ghi **task 3060 = giờ dùng**; cooldown **7 ngày** (`rename_func.lua:12-15`);
   đã có bit thì báo "đã có 1 lần đổi tên, gặp Sứ Giả".
2. **Sát nhập máy chủ**: tool goddess tự đổi tên trùng và bật bit 1 (`TASKVALUE_MASK_RENAME` trong
   `libgoddesscfg.so`), `login_hint.lua:210-229` nhắc người chơi đi đổi lại.
3. **GM/NPC test**: `gm\lbadmin.lua:1931`, `gm\ex_lib_function.lua:445`, `thinh\npc\npc_vocong.lua:30`,
   `thunghiem\doiten.lua:36` (thu 50 Xu = item 4/417) — gọi `RenameRole` thẳng, không điều kiện.

### 1.2 Luồng tại NPC "Sứ giả" (template 87, đặt ở 8 map: 20/53/121/99/100/101/174/153 —
`global\addspreadernpc.lua:5-12`)
`misc\spreader\spreader.lua:41-43` menu "Đặt lại tên cho nhân vật" → `misc\rename\rename_func.lua`:
- `check_renamerole()` = `GetBit(GetTask(2320),1)==1` (`global\rename_head.lua:14-19`);
- **cấm khi đang trong bang hội** (`rename_func.lua:42-46`, `GetTongName()` có id);
- tra tên trước: `AskClientForString('on_query_rolename','',1,20,...)` → `QueryRoleName(tên)`;
- đổi: `AskClientForString('on_change_rolename','',1,20,'Xin nhập tên nhân vật mới')` → tên mới ≠ tên cũ
  → `SetTask(3060, now)` + **`RenameRole(tên)`**; thoại báo *"bạn sẽ tự động rời mạng, 3 phút sau
  đăng nhập lại"*.

### 1.3 Engine (disasm `jx_linux_y`)
- `RenameRole(L)` `0x0810A9C0` → `KPlayer::RenameRole` `0x080AA560`: từ chối nếu `strlen > 31` hoặc
  trùng tên cũ (`+0x5974`); **KHÔNG đổi tên tại chỗ** — tạo `KRoleRenameCmd{player, tên mới}`
  đẩy vào list `KPlayer+0x86d4` rồi **`KickOut(2)`** (`[ShutdownClient] Kick Out, ReasonId (2)`).
- Lúc logout (`"Del %s(%s): quit"` `0x080C6680`) → `ExecLogoutCmds` `0x080AA4D0` → `Execute` `0x080AA630`
  gửi 2 gói qua relay: **`0x9D {account, 180}` kênh 1** (khoá tài khoản 180 s = "3 phút") và
  **`0x9F {account, tên mới, tên cũ}` kênh 0** (Goddess).
- Goddess (`goddess_y`, chỉ quét chuỗi): `KQuery_ChangeRoleName` —
  `UPDATE Role SET RoleName='%s', RoleData='%s' WHERE RoleName='%s'`, chặn tên của nhân vật vừa xoá
  (`SELECT COUNT(*) FROM RoleBack WHERE RoleName=... AND LastModify>=...`), lọc `goddess_rolename.flt`.
- Trả về GameServer gói **`0xC0` len 0x62** (`0x081D7880`): kết quả ≠ 0 → tìm player theo tên cũ, gọi
  script `ChangeNameResult(tên, kq)` nếu còn online; = 0 → ghi lịch sử + chuyển tiếp `0x9F` **kênh 2**
  (s3relay: `UPDATE Relation SET RoleName=...` — `L_RELATION_RENAME` / `L_TONGMEMBER_RENAME` ⇒ relay tự
  cập nhật **bạn bè + thành viên bang**). Cả hai nhánh gửi `0x9E {account}` kênh 1 (mở khoá).
- `QueryRoleName` `0x0810AA50`: gửi `0x9C` kênh 0, **trả 1 ngay (bất đồng bộ)**; đáp `0xBE` len 0x66 →
  `\script\global\rolename_op.lua:QueryNameResult(tên, 0 FREE/1 INVALID/2 USED)`;
  `ChangeNameResult(tên, 0..4 SUCCESS/FAILURE/INVALID/USED/ONLINE)` — đường dẫn + tên hàm nằm cứng
  trong binary (`0x082694D4`, `0x082694B1`, `0x082694C1`).
- Bộ lọc tên `\settings\rolename.flt` (dạng `=từ_cấm`, `@+GM`); log `\rolenamechangehis.ini`
  (`[Global] Count=N`, `Account_N: Account/OldName/NewName`) nạp lúc boot, ghi lúc `GameClose`.
- `ForbitSyncName` **không liên quan** (ẩn tên NPC/người chơi trong Đại Tứ Sát).
- Lưu ý: script Linux **không xoá bit 1** sau khi đổi thành công (chỉ bit 2 – bang) ⇒ "chỉ 1 cơ hội"
  do phía goddess/tool xoá, chưa thấy trong 4 nơi được phép đọc.

```
Item P30448 --(bit1 task2320, task3060)--> NPC Sứ giả (spreader.lua)
  |-- tra tên --> AskClientForString --> QueryRoleName --[0x9C k0]--> Goddess
  |                                          <--[0xBE]-- QueryNameResult(tên,kq)  (rolename_op.lua)
  '-- đổi   --> AskClientForString --> RenameRole(new)
                 |-> len<=31 & ≠cũ -> list += KRoleRenameCmd -> KickOut(2)
                 '-> logout -> Execute: [0x9D acc,180 k1] + [0x9F acc,new,old k0]
                        Goddess: UPDATE Role SET RoleName=new WHERE RoleName=old (+RoleBack)
                        <--[0xC0 kq]-- lỗi: ChangeNameResult (nếu online) | ok: history + [0x9F k2 → relay sửa bạn bè/bang]
                        cả hai: [0x9E acc k1] mở khoá ; người chơi relog sau 3 phút thấy tên mới
```

## 2. ĐỔI PHÁI — bản Linux "Đại nghệ đầu sư" (daiyitoushi)

### 2.1 Vị trí & công tắc
Menu `"Đại nghệ đầu sư/#daiyitoushi_main(N)"` trong `oldentence()` của **cả 10 NPC chưởng môn**
(`global\pgaming\npc\chuongmoncacphai\*.lua`, N = số hiệu phái 0..9); lõi
`misc\daiyitoushi\{toushi_head,toushi_function,toushi_resetbase}.lua` (`item\card\toushi_function.lua`
là bản sao cũ hơn). Công tắc `gb_GetModule("SWITH_DAIYITOUSHI")==1` (league task 502/1 — trên relay).

### 2.2 Hai bước
1. **Ở chưởng môn CŨ**: nộp **"Tín vật môn phái"** {6,1,**1670**} qua `GiveItemUI` (`toushi_function.lua:189`)
   → `RemoveItemByIndex` + `SetTask(1881,1)`; cần cấp ≥ 120, đang có số hiệu phái.
2. **Ở chưởng môn MỚI**: `toushiCheckTransFactState` — task 1881==1; giới tính (nam cấm Nga Mi 4/Thuý Yên 5,
   nữ cấm Thiếu Lâm 0; nữ vào Thiên Vương cần thêm {6,1,**1671**}); phái đích ≠ hiện tại; cấp ≥ 120;
   Hoa Sơn (10) cần item **4328** + ≥ 5 chuyển sinh; **KHÔNG trong liên đấu WLLS**; **PHẢI CỞI HẾT TRANG BỊ**
   (`CalcItemCount(2,0,-1,-1,-1)==0`); lần đầu cần 2 ô túi trống; đang làm nhiệm vụ kỹ năng 150 (task
   2885 < 14) thì chặn; **không tốn tiền**; cooldown 60 ngày **đã bị comment** (ThanhLD 20131024) nhưng task
   2604 vẫn ghi thời điểm; số lần đổi task 1883 chỉ tăng, không trần; chọn nhánh kỹ năng 90.

### 2.3 `toushiDoTransPlayer(A,B,nhánh)` — thứ tự bắt buộc (`toushi_function.lua:349-529`)
```
LeaveTeam → zhuansheng_clear_skill{ RollbackSkill(); AddMagic lại khinh công 210/400; AddMagicPoint }
→ lưu cấp+%exp kỹ năng 90/120/150 của A → DelMagic toàn bộ skill A → AddMagic skill B (10..70 cấp 0; 90/120/150 map theo bảng)
→ toushiAddWuXingSkill (đổi skill ngũ hành 445-449/534-538 + Khí Doanh 1501+series; kinh mạch ChangeFullBreathByNewSeries)
→ zhuansheng_clear_prop(seriesB) → SetSeries(seriesB) → NPCINFO_SetMaxLife/Mana → SetTask(1982,1)
→ SetLastFactionNumber(B) → UpdateMeridianSkill (xoá 1220..1229/1370, cấp lại theo phái mới) → SetTask(task phái A, 0)
→ chưa xuất sư ? SetFaction("B") + SetCamp/SetCurCamp(campB) + SetTask(taskB, 80*256) : chỉ SetTask(taskB, 70*256) (giữ camp 4)
→ SetRank → SetTask(137,..)(144,0)(1881,0)(1883,+1) → tặng 2 viên Trấn Phái Linh Đan/Dược 1704/1705 (1 lần)
→ xử lý chuyển sinh 7 → Msg2Faction + WriteLog → KickOutSelf
```
**`SetSeries` PHẢI đứng trước `SetFaction`**: cả Linux (`0x08060BB0`) lẫn JX1 (`KPlayerFaction.cpp:80`)
từ chối phái không khớp hệ hiện tại.

### 2.4 Engine (disasm)
- **`SetFaction(s)`** `0x0811A540`: **luôn** `LeaveCurFaction` (cur=−1, camp=4, gói `0x7C` 1 byte) rồi nếu
  `s≠""` → `g_Faction.GetID(series=Npc+0x28, tên)` (11 entry, Hoa Sơn=10) → kiểm `entry.series==series`
  → cur=id, addTimes++, first (nếu lần 1), **last=id** → `SetCamp` → gói `0x7B` 8 byte
  `{camp, cur, LAST, addTimes}`. **Không** đụng kỹ năng/series/rank/task.
- **"Faction record"** = 16 byte tại `Player+0x59CC` `{cur, first, last, addTimes}` — **không phải danh
  sách nhiều phái**. DB chỉ lưu `{cur, LAST, addTimes}` vào **đúng slot JX1 dùng cho `{cur, FIRST, addTimes}`**;
  `m_nFirstAdd(+0x59D0)` không có tham chiếu nào ⇒ slot "first" của JX1 tương đương "phái gần nhất" Linux.
- `GetLastFactionNumber` `0x0810E6D0` trả **last** (giữ số phái cả sau xuất sư — là **khoá tra cứu** của kỹ
  năng 120/150, sách đại thành, kinh mạch, BXH, log, Tống Kim…); `SetLastFactionNumber` ghi last;
  `GetFactionNumber` trả cur; `GetLastAddFaction` trả TÊN `g_Faction[last]`;
  `ClearFactionRecord` (cur=first=last=−1, addTimes=0) và `ReloadSkill` (GM nạp lại bảng skill)
  **không script nào gọi**.
- **`RollbackSkill()`** `0x0811C640`: **không tham số**; duyệt 80 slot, bỏ `IsBase`/`IsExp`, hạ mọi cấp về 0,
  trả tổng điểm; gửi gói `0x5E` đồng bộ `m_nSkillPoint`; **không tự cộng điểm** (script `AddMagicPoint`).
- `SetSeries` (`Npc+0x28`, n≤4) / `SetSex` (`Npc+0x152C`): **không sync client** ⇒ script luôn `KickOutSelf`.
- Menu khác trong chưởng môn: **xuất sư** (`SetTask 70*256; SetFaction(""); SetCamp/SetCurCamp(4)`),
  **trùng phản phái cũ** (`Pay(50000); SetTask 80*256; SetFaction(tên); SetCamp; SetRank; add_xx(70)`),
  **phản sư cùng hệ** `defection_yes` = **mã chết** (`conlon.lua:99` còn gọi nhưng hàm không tồn tại).
  Mỗi `oldentence()` có khối "hệ thống phát hiện sai sót" tự sửa lệch task/GetFaction/GetCamp/bang — bản port
  cần giữ.
- Phái 11 Vũ Hồn / 12 Tiêu Dao chỉ tồn tại ở script (`faction_def.lua`), engine `g_Faction` chỉ 0..10.
- "Tẩy tuỷ" ở Linux = **reset điểm**, không phải đổi phái (Tẩy Tuỷ Kinh P22, Bạch/Tử Ngọc P2390/2391, Bắc Đẩu
  Tẩy Tuỷ Đơn P30127, đảo tẩy tuỷ map 242).

## 3. HIỆN TRẠNG JX1

### 3.1 Đổi phái — đã có gần đủ
- Engine: `SetFaction`→`LuaChangePlayerFaction` (`ScriptFuns.cpp:8601-8620`: `LeaveCurFaction`+`AddFaction`,
  chỉ đổi `m_cFaction`/camp, gói `s2c_playerfactiondata {camp,cur,FIRST,addTimes}`), `RollbackSkill`→
  `KSkillList::RollBackSkills` (`KSkillList.cpp:919-941`), `SetSeries`/`SetSex` (không sync — giống Linux),
  `ClearFactionIfnfo` + `DelAllMagic` (2 hàm JX1 tự chế thay `ClearFactionRecord`), `GetFaction`/`GetFactionNo`/
  `GetFirstAddFaction`/`GetLastFactionNumber`/`GetLastAddFaction`/`GetEquipCount`.
- Script sống: **`item\lenhbaitanthu.lua:1047-1266`** menu "Chức năng chuyển phái" (cởi hết trang bị, đã vào
  phái, phí 0) — luồng **tự chế của JX1**: `DelAllMagic → ClearFactionIfnfo → SetSeries/SetSex → SetFaction →
  SetCamp/SetCurCamp/SetRank → hockynang → chuyển cấp skill 90/150/120 → ResetAP → ResetJN (RollbackSkill(0)
  + AddMagicPoint) → KickOutSelf`; nhiều NPC tẩy tuỷ (`phantang.lua`, `quanly.lua`, chuyển sinh `task_func.lua`).
- **5 khoảng cách so với Linux**:
  1. `LuaGetLastFactionNumber` JX1 = `GetFactionNo()` = **cur** → sau xuất sư trả −1 (Linux giữ last).
     `KPlayerFaction` JX1 không có trường `last`.
  2. **Chưa đăng ký `SetLastFactionNumber` / `GetFactionNumber` / `ClearFactionRecord`** — mà
     `E:\...\script\global\skills_table.lua:70-110` **đang gọi `SetLastFactionNumber`** (⚠️ lỗi tiềm ẩn).
  3. `LuaRollBackSkills` **bắt buộc 1 tham số** (`ScriptFuns.cpp:6732-6746`, thiếu → trả nil) trong khi mọi
     script Linux gọi `RollbackSkill()` không đối số; ngữ nghĩa Linux ≡ `bRbAll=false`; JX1 không gửi gói sync
     điểm sau rollback (`AddMagicPoint` mới gửi).
  4. `MAX_FACTION=10`, kiểm id theo `[series*2,(series+1)*2)` (`KFaction.h:15-16`, `KPlayerFaction.cpp:76-90`);
     Linux 11 entry (Hoa Sơn=10, hệ Thuỷ) kiểm theo trường `series`.
  5. Gói `s2c_playerfactiondata` JX1 mang **FIRST**, Linux mang **LAST** (client `KUiSkillsNew` chọn bảng kỹ
     năng theo `nFirstAddFaction` — đổi phái phải reset first, `ClearFactionIfnfo` đã làm).

### 3.2 Đổi tên — hoàn toàn chưa có, và rất nguy hiểm
- Grep toàn `D:\GAMEDEVNEW\Sources` + script sống: **0** `RenameRole`/`QueryRoleName`/`ChangeNameResult`; Goddess
  chỉ 6 op (`ClientNode.cpp:82-89`: getrolelist/createroleinfo/saveroleinfo/deleteplayer/getroleinfo/lock); Lua
  chỉ `GetName`/`GetAccount`; client chỉ nhập tên lúc tạo nhân vật (`UiNewPlayer.cpp:354-390`).
- **Khoá CSDL nhân vật = TÊN**: MySQL `role_name` là khoá `ON DUPLICATE KEY`, `acc_name` chỉ là chỉ mục phụ
  (`Goddess\DBTable_MySQL.cpp:813-829, 1180-1198`); `GetRoleInfo/SaveRoleInfo/DeleteRole/IsRoleLock` theo tên;
  blob `TRoleData.BaseInfo.szName` cũng chứa tên và Heaven **ghi lại bằng `m_PlayerName`** khi save
  (`KPlayerDBFuns.cpp:943`); bảng phụ `role_history/role_anomaly/role_delete_log/role_save_fail` cột `role_name`.
- **ID runtime = hash tên**: `Player.m_dwID = g_FileName2Id(szName)` (`KPlayerSet.cpp:194-213`) ⇒ đổi tên =
  đổi ID ⇒ mọi so sánh `m_dwID` (chat/friend/mission/team) lệch.
- **Nơi lưu tên dạng chuỗi phải đồng bộ**: bang hội S3Relay `STONG_MEMBER{m_dwNameID, m_szName}` + master/
  director/manager (khớp CẢ id LẪN strcmp, `KTongControl.cpp:1317-1318`; `KPlayerTong.cpp` 40 chỗ hash tên);
  bạn bè `FriendMgr` khoá DB = tên + danh sách bạn của **người khác**; hôn nhân `m_PlayerMateName` nằm trong
  **blob của người kia**; xếp hạng `DBBackup TRoleList.Name`; thành chiến `KJx2CityWar szMaster`; tiêu xa
  `Npc.Owner`; pet/partner `Owner`; xúc xắc `szName`; Bishop bảng online theo tên; log/chat lịch sử **không sửa được**.
- **5 rủi ro đặc thù**: (1) role lock theo tên — đổi khi đang khoá/online làm bản ghi cũ kẹt khoá; (2) đổi PK
  MySQL mà quên sửa blob ⇒ lần save kế **ghi lại tên cũ vào khoá cũ = nhân bản nhân vật**; (3) `ON DUPLICATE
  KEY` ⇒ nếu Heaven save trước khi Goddess đổi thì **2 bản ghi cùng tài khoản**; (4) tong/friend không có ID ổn
  định ⇒ tên mới = "người lạ" nếu quên quét; (5) lịch sử/log không sửa được.

## 4. PHÍA CLIENT & DỮ LIỆU (cả hai tính năng)
- Linux Patch: **không có cửa sổ riêng** (không tệp 改名/更名/转职); đổi tên dùng `ui\ui3_1024\输入字串界面.ini`
  (hộp nhập chung, `MaxLen=500`) do server mở bằng `AskClientForString`; đổi phái dùng menu NPC + `给予界面.ini`.
- **JX1 đã có sẵn**: `AskClientForString` (`ScriptFuns.cpp:14261/15336`, gói `S2C_INPUT_BOX` nType 1 →
  `KUiGetString2` `KWndEdit512` → callback nhận chuỗi y hệt Linux; giới hạn prompt ≤31 byte, chuỗi ≤63, tên
  callback ≤31); `OpenGetString`; `GiveItemUI` → `KUiAffairItem`; luật kiểm tên tạo nhân vật (6..16 ký tự, ASCII
  0x21-0x7E hoặc cặp 2 byte, cấm 0xA1A1 — `UiNewPlayer.cpp:354-390`, `LoginDef.h:54-55`) tái dùng được (kiểm lại
  phía server); Goddess `RoleNameFilter` (`rolename.flt`).
- VLTK Level Up (36 pak, đã quét slistcache/slistfree/1024/updatejx17/vng00): **không** có Tinh Danh Chi Lệnh;
  bảng chuỗi `59A637DE` không có chuỗi đổi tên nhân vật; `vng00.pak` magicscript mới có buff **"Đang đợi đổi tên
  nhân vật"** G8 D680 (3 ngày, `buff662.lua`, NPC "Phủ An Sứ") ⇒ VLTK dùng cơ chế **chờ 3 ngày** — vẫn không UI riêng.
  Chưa giải nén `update*.pak`/`updatejx01-16`/`spr` (icon).
- **Dữ liệu cần thêm nếu port**: magicscript JX1 (nhớ bẫy *RECORD INDEX = mã item*): Tinh Danh Chi Lệnh
  (`tianbaokuling.spr`), Tín vật môn phái `zhuanzhiling.spr`, Tín vật Dương Anh `nvtianwangling.spr`, Trấn phái
  linh đan/dược `zhenpailingdan/yao.spr`, Hoa Sơn Kỳ Thư `other\23miji.spr` — **kiểm icon có trong pak client JX1**;
  `forbititem` cấm vứt; NPC Sứ giả template 87 (hoặc gắn menu vào NPC sẵn); **quét trùng task id**
  2320(bit1/2/3)/3060/1881/1882/1883/2604/1982/2885 (luật `jx1-trung-task-id`); chuỗi thoại TCVN3.

## 5. ĐÁNH GIÁ PORT (để chủ quyết)

### 5.1 Đổi phái theo chuẩn Linux — **vừa phải, chủ yếu script**
- C++ (nhỏ): đăng ký `SetLastFactionNumber`/`GetFactionNumber`/`ClearFactionRecord`; thêm trường `last` vào
  `KPlayerFaction` (hoặc **quyết định**: dùng slot `m_nFirstAddFaction` làm "last" như Linux lưu DB — đổi ngữ nghĩa
  gói `s2c_playerfactiondata` và client `KUiSkillsNew`); cho `RollbackSkill()` không tham số = `bRbAll=false`;
  cân nhắc `MAX_FACTION` 11 nếu muốn Hoa Sơn.
- Script: mang `misc\daiyitoushi\{toushi_head,toushi_function,toushi_resetbase}.lua` + `task\metempsychosis\
  task_func.lua` (`zhuansheng_clear_skill/prop`) + `skill\lvlup150_limit.lua` + `global\meridian\meridian.lua`
  (phần `UpdateMeridianSkill`/`ChangeFullBreathByNewSeries`) + `settings\npc\player\magic_level_exp.txt`;
  ghép vào 10 NPC chưởng môn JX1; nắn mã vật phẩm/skill theo bảng JX1.
- **Điểm phải quyết trước khi làm**: giữ `lenhbaitanthu.lua chuyenphai` (JX1 tự chế, đang chạy) hay thay bằng
  daiyitoushi; có mở cooldown 60 ngày (Linux đã tắt) không; có Hoa Sơn không; điều kiện bang hội/kết hôn (Linux
  **không chặn** bang hội khi đổi phái).

### 5.2 Đổi tên nhân vật — **lớn, 4 tầng C++ + DB, khuyến nghị làm đúng kiểu Linux (offline)**
- Goddess: op mới `rename` (kiểm trùng `search(pName)`, lọc `CRoleNameFilter`, `UPDATE` PK `role_name` + sửa
  `BaseInfo.szName` trong blob + CRC + chỉ mục tài khoản; chặn tên của nhân vật vừa xoá; ghi `role_history`).
- Core/Heaven: Lua `RenameRole`/`QueryRoleName` + callback `QueryNameResult`/`ChangeNameResult`; **thực thi lúc
  logout** (mẫu `KRoleRenameCmd` của Linux) — tuyệt đối không đổi tên khi online vì `m_dwID` = hash tên.
- S3Relay: đổi khoá FriendMgr (`DB_DeleteSomeone` tên cũ + `DB_StoreSomeone` tên mới) + quét tên cũ trong
  `FRIENDSMAP` của mọi người; tong: `STONG_MEMBER.m_szName/m_dwNameID` + master/director/manager (Linux có
  `RenameTong` riêng).
- Blob người khác: `m_PlayerMateName` (bạn đời) — cần Goddess mở blob người kia hoặc script tự sửa khi người
  kia đăng nhập.
- Bishop: không cần (bảng online tạm; role list đọc từ Goddess theo `acc_name`).
- Client: **không bắt buộc** (tên về từ `Npc.Name` khi relog); UI nhập đã có.
- Rủi ro: xem 3.2. Chi phí ước lượng: **lớn nhất trong mọi đợt port đến nay** vì đụng khoá CSDL + relay; sai
  một tầng là **nhân bản/mất nhân vật**. Nếu vẫn làm: phải có backup roledb + kịch bản rollback trước khi thử.

## 6. CÂU HỎI CHỜ CHỦ QUYẾT (tôi KHÔNG tự làm)
1. Có port **đổi phái** theo daiyitoushi không? Giữ hay bỏ `chuyenphai` tự chế trong `lenhbaitanthu.lua`?
2. Có làm **đổi tên** không, hay chỉ cần đường **GM/offline** (đổi trực tiếp trong DB lúc bảo trì — rẻ hơn nhiều)?
3. Nếu có: kích hoạt bằng vật phẩm (như Linux P30448, 7 ngày) hay NPC/GM?
4. Cho phép xử lý ngay lỗi phụ `skills_table.lua:70-110` gọi `SetLastFactionNumber` chưa đăng ký (đăng ký hàm
   theo ngữ nghĩa Linux, hoặc rào lại)?
