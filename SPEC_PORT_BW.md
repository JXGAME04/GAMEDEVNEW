# ĐẶC TẢ THI CÔNG — LÔI ĐÀI TỶ VÕ (`missions/bw`, tag **bw**)

> Lập **22/08/2026**. Nguồn: `D:\ServerLinux\server1` (script + ELF `jx_linux_y`), đích: `D:\GAMEDEVNEW\Sources` + cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`.
> Mọi khẳng định có `tệp:dòng` hoặc địa chỉ ELF đã đọc thật. Công cụ phụ trợ + kết quả thô: `D:\GAMEDEVNEW\ReverseTools\port_bw\` (xem mục 0.1-0.2).
> Chính sách chủ game: chép 100% bản Linux, chỉ đổi cổng cấp → `GetLevel() >= 90`, bỏ trùng sinh (bw **không có** trùng sinh), thiếu item thì làm thêm (bw **0 item**).

---

## 0. TÓM TẮT ĐIỀU HÀNH

| Việc | Kết luận |
|---|---|
| Làm được không? | **LÀM ĐƯỢC CÓ ĐIỀU KIỆN** — script nhỏ (9 tệp/854 dòng + 1 tệp `branch_bwsj.lua`), **0 item, 0 ảnh, 0 NPC template mới, map 209 đủ cả hai phía**. Điều kiện: **7 điểm engine** (mục C) — trong đó 3 điểm là *lỗ hổng ngữ nghĩa chung* của lớp tương thích JX2 hiện có (`AskClientForNumber` đang là stub trả −1, `SetPKFlag` đang map sai sang `SetFightState`, timer mission gọi `OnMissionTimer` chứ không `OnTimer`) mà **không sửa thì trận không chạy**. |
| Hỗn Chiến có phải bản chép bw? | **KHÔNG.** `tinhnang/loidaihonchien` (86 dòng) là *đánh tự do 1 camp đồ sát, mặt nạ 2019, thưởng 10 triệu exp/mạng ≤ 4 người, 16h và 22h* (`mainloidai.lua:30-49`, `bigiet.lua:10-16`). bw là *2 đội cỡ 1v1…8v8, số thứ tự vào đài, mission + 2 timer, không thưởng*. Chỉ trùng **map 209**. ⇒ **GIỮ CẢ HAI**, dời Hỗn Chiến sang **map 210** (map 210/211 là bản sao y hệt `中原南区\演武场一`, `maplist.ini:2281-2289`; bw chỉ dùng `BW_COMPETEMAP[1] = 209` — 210/211 khai báo nhưng **không dùng** ở bất kỳ dòng nào). 1 dòng sửa: `mainloidai.lua:46`. |
| Hàm engine thiếu thật (api_gap2) | **4**: `CreateChannel`, `EnterChannel`, `LeaveChannel`, `DeleteChannel` → **stub** (client JX1 không có kênh chat động). `ForbidEnmity`/`ChangeOwnFeature` đã có từ đợt Hoạt động phường (`ScriptFuns.cpp:13833`, `:3915`). |
| Hàm "đã có" nhưng SAI/stub làm bw chết | `AskClientForNumber` (stub −1, `KTongJX2.cpp:4132`) · `SetPKFlag` (= SetFightState, `KJx2WarInfra.cpp:227`) · `ForbidChangePK` (no-op, `:234`) · callback timer `OnMissionTimer` (`KMission.cpp:342`) · callback chết `OnPlayerDeath` (`KNpc.cpp:1542`) · `SetPunish` **ngược nghĩa** (`ScriptFuns.cpp:12797` ↔ ELF `0x0810F470`) · `GetTeamMember` **lệch chỉ số** (`ScriptFuns.cpp:7235` ↔ ELF `0x08115530`). |
| Khe | Mission **4 → 11** (`missions.txt` dòng 12 đang `mission_trong.lua`); timer **10/11 → 20/21** (`TimerTask.txt` chưa dùng 19-49). |
| Lối vào | NPC **Công Bình Tử** (tpl 309 "Lãng nhân 3") **12 vị trí** trong map-data 3 thành (Dương Châu 80 / Tương Dương 78 / Thành Đô 11) — **trùng từng byte** ở `maps.pak` hai cây, engine dự án **bỏ** (`NotAddNpcNormal=1`) ⇒ `AddNpc` bằng Lua lúc `startgame` (mục F). |

### 0.1 Cách chạy lại số liệu (cwd `D:\GAMEDEVNEW\ReverseTools`, `set PYTHONIOENCODING=utf-8`)
```
python port_bw\bw_closure.py            # bao đóng Include (kết quả: port_bw\bw_closure_out.txt)
python api_gap2.py BW missions/bw       # ENG 4 / LIB 2 / OK 57 (port_bw\bw_api_gap.txt)
python port_bw\bw_stubcheck.py          # hàm OK nhưng thân là stub (port_bw\bw_stubcheck_out.txt)
python item_remap.py missions/bw        # 0 item
python npc_skill_remap.py missions/bw   # NPC 332 TRÙNG ID, 0 skill
python port_bw\bw_regionnpc.py <pakdir> <maplist.ini> 80,78,11 bw   # NPC Công Bình Tử trong region (2 cây trùng)
python port_bw\bw_cellcheck.py <pakdir> "中原南区\演武场一" 1620,3202 ...  # ô đứng có vật cản không
python re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x08115CA0 120   # AskClientForNumber …
```
### 0.2 Tệp kết quả
`port_bw\dac_ta_ham_bw.json` (đặc tả 10 điểm engine theo khuôn `dac_ta_17_ham_hoatdong_phuong.json`), `bw_npc_congbinhtu_{linux,duan}.txt`, `bw_stubcheck_out.txt`, `bw_closure_out.txt`, `bw_api_gap.txt`, `find_imm.py` (tìm hằng offset trong ELF).

---

## 1. CƠ CHẾ (đọc hết 9 tệp bằng `gbktool.py`)

```
NPC Công Bình Tử (bwmanager.lua main):   SetTask(300/301/302) = vị trí đứng; đọc MS_STATE của map 209
  state 0 → "Được thôi!/OnRegister"  → tổ đội ĐÚNG 2 người + là đội trưởng (bwmanager.lua:39-47)
            → chọn 1v1..8v8 → SignUpFinal(n) (:52-106): OpenMission(4) trên map 209, MissionS(1/2)=tên 2 đội trưởng,
              MissionS(6)=tên thành, MissionV(10/11)=2 số thứ tự ngẫu nhiên 1..9999 (bw_getkey :108-122), Msg2Team
  state 1 → OnShowKey (bwhead.lua:195) | OnEnterMatch (:130) → đội trưởng OnJoin(0) / đội viên AskClientForNumber→OnEnterKey1(Key) (:145-164)
            | OnLook → onwatch → OnJoin(3) khán giả (:166-189)
  state 2 → OnFighting (đang đánh, chờ trận sau)
InitMission (bwmission.lua:2-21): reset MissionV 1..40 / MissionS 1..10, MS_STATE=1, 2 timer (20 s / 12 phút), 2 kênh chat, 2 NPC camper tpl 332
JoinCamp(c) (bwhead.lua:106-131): LeaveTeam, AddMSPlayer(4,c), SetCurCamp(c+1 | 0 khán giả), cờ vào trận (:133-178), NewWorld 209
  khán giả: SetPKFlag(0) + ChangeOwnFeature(0,0,-1,-1,-1,-1,-1) = tàng hình
bwtimer.lua OnTimer mỗi 20 s: state 1 → đếm ngược báo danh, V==GO_TIME(6)=2 phút → bw_begin_compete (:76-111):
   0 bên vào → huỷ; 1 bên vào → xử thắng; đủ → RunMission(4) → SetFightState(1) cả hai phe, MS_STATE=2
   state 2 → ReportBattle + bw_checkwinner (một bên hết người → kết thúc)
bwdeath.lua OnDeath(Launcher): kẻ giết là người chơi phe 2/3 → DelMSPlayer người chết → bw_all_death → hết người → CloseMission
bwtotaltimer.lua OnTimer (12 phút): đếm người còn lại → thắng/hoà → CloseMission
EndMission (bwmission.lua:30-46): reset, GameOver() = NewWorld mọi người về 300/301/302, dừng timer, xoá kênh, ClearMapNpc(209)
OnLeave(RoleIndex) (:48-54): LeaveGame() + NewWorld về chỗ cũ
```
Hằng số (`bwhead.lua`): `BW_MISSIONID=4` (:40), timer `10/11` (:34-35), `TIMER_1=20*18` (:36), `TIMER_2=12*3*TIMER_1` (:37), `GO_TIME=6` (:39), map `{209,210,211}` (:8), 3 toạ độ vào `(209,1620,3202)/(209,1612,3187)/(209,1598,3216)` (:11-14), camper `(1599,3202,"Trương Tam")/(1608,3211,"L?T?")` (:15-18), hồi sinh `SetRevPos(80,36)/(29)/(6)` (:42-49), task vị trí `300/301/302` (:23-25), `MAX_MEMBER_COUNT=100` (:21, không dùng).
Dòng khoá cứng: `bwmanager.lua:7  do Talk(1, "", "Chức năng đã đóng.") return end` (không có `--`).

---

## A. DANH SÁCH TỆP CHÉP (bao đóng Include — `port_bw\bw_closure.py`)

Bao đóng thô ra 41 tệp vì `branch_bwsj.lua:2` kéo `battles\battlehead.lua` (→ 26 tệp Tống Kim JX2: `battles/*`, `lib/droptemplet.lua`, `missions/boss/bigboss.lua`, `bonusvlmc/head.lua`…). **Cắt** tại đó (mục A.2) ⇒ còn **11 tệp**:

| # | Tệp (đường Linux = đường dự án) | Dòng | Dự án | Cách |
|---|---|---|---|---|
| 1 | `script\missions\bw\bwhead.lua` | 240 | THIẾU | chép + **sửa** (H1-H5, H7, H9) |
| 2 | `script\missions\bw\bwmanager.lua` | 256 | THIẾU | chép + **sửa** (H6, H8, H10, H11) |
| 3 | `script\missions\bw\bwmanager_chengdu.lua` | 2 | THIẾU | chép nguyên |
| 4 | `script\missions\bw\bwmanager_xiangyang.lua` | 2 | THIẾU | chép nguyên |
| 5 | `script\missions\bw\bwmission.lua` | 75 | THIẾU | chép nguyên |
| 6 | `script\missions\bw\bwtimer.lua` | 176 | THIẾU | chép nguyên (*nếu chọn cách B ở C6 thì thêm 1 dòng cuối*) |
| 7 | `script\missions\bw\bwtotaltimer.lua` | 31 | THIẾU | chép nguyên (*như trên*) |
| 8 | `script\missions\bw\bwdeath.lua` | 59 | THIẾU | chép nguyên (*nếu chọn cách B ở C7 thì thêm 1 hàm cuối*) |
| 9 | `script\missions\bw\bwcamper.lua` | 13 | THIẾU | chép nguyên |
| 10 | `script\task\newtask\branch\branch_bwsj.lua` | 60 | THIẾU (cả thư mục `branch\` chưa có) | chép + **sửa dòng 2** (H12) |
| 11 | `script\task\newtask\newtask_head.lua` | — | **ĐÃ CÓ, DIFF** | **dùng bản dự án** — diff = dự án **thêm** 36 dòng `AddGoldItem` wrapper (`53a54,89`), không bớt gì; `nt_getTask/nt_setTask` ở `:31-39` |
| 12 | `script\missions\bw\bw_addnpc.lua` | ~30 | **MỚI** | đặt NPC Công Bình Tử (mục F) |

`IncludeLib("RELAYLADDER")` (`bwhead.lua:1`) và `IncludeLib("SETTING")` (`branch_bwsj.lua:3`) → `scriptjx2\lib\noop.lua` (`ScriptFuns.cpp:2450-2467`), bw không gọi hàm Ladder nào ⇒ vô hại.
Các tệp DIFF khác mà bao đóng thô liệt kê (`lib/awardtemplet.lua`, `misc/eventsys/eventsys.lua`) **chỉ bị kéo qua `battlehead.lua`** ⇒ không liên quan sau khi cắt.
`script\header\loidai.lua` (160 dòng, dự án) là bản **viết lại dở của bwhead**, **không ai Include** (grep 0) ⇒ **không dùng**, có thể xoá hoặc để nguyên.

### A.2 Vì sao cắt `battlehead.lua`
`branch_bwsj.lua` chỉ có 3 hàm: `branchTask_BW1` (:4-19), `branchTask_GainBW1` (:22-40) — dùng `nt_getTask/nt_setTask/Msg2Player`; `branchTask_JoinSJend1` (:42-61) dùng `BT_GetData(PL_TOTALPOINT)` (hằng từ battlehead) — **bw không gọi hàm này**, và `BT_GetData` trong dự án là stub trả 0 (`ScriptFuns.cpp:2254`). Task 1011-1013/1052/1056/1060 là chuỗi nhiệm vụ JX2 (Ngạo Vân Tông / Liễu Nam Vân / Lưu Uẩn Cổ) **dự án không có** (grep `Task(1011` = 0) ⇒ 2 hàm BW luôn rơi vào nhánh không làm gì — giữ để trùng mã, vô hại.

---

## B. SETTINGS / MAP

| Gì | Ở đâu | Sửa |
|---|---|---|
| Mission | `settings\task\missions.txt` **dòng 12** (id 11) | `\script\missions\mission_trong.lua` → `\script\missions\bw\bwmission.lua` (tra theo SỐ DÒNG, cấm xoá dòng) |
| Timer | `settings\TimerTask.txt` (tra theo KHOÁ) | nối 2 dòng: `20<TAB>\script\missions\bw\bwtimer.lua` và `21<TAB>\script\missions\bw\bwtotaltimer.lua` (Linux dùng 10/11 — `timertask.txt:10-11` Linux; dự án 10 = `timertask\task10.lua` đang sống, 11 trống nhưng chọn 20/21 cho liền khối; 19-49 chưa ai dùng) |
| Map 209 | `maplist.ini:2281` dự án `209=中原南区\演武场一` (Linux `:2274` = `中原南区\yanwuchang`, biến thể nhỏ hơn) | **không đổi**. Region server dự án S=99 (Linux yanwuchang S=22), client `data\*.pak` C=108; 5 ô đứng + 2 ô camper đều vật cản 0 ở dự án (`bw_cellcheck`: `(1620,3202) region(101,100) vatcan=0x0` … riêng ô camper `(1599,3202)` = 0x14, NPC đứng được). Hỗn Chiến đang thả người ở `(1628,3213)` cùng vùng ⇒ hình học tương thích. |
| Map 210 (Hỗn Chiến dời sang) | `maplist.ini:2285` `210=中原南区\演武场一` | đã khai, cùng dữ liệu |
| RevivePos | `settings\RevivePos.ini` rev 6 (map 11) / 29 (map 78) / 36 (map 80) | **trùng từng số** 2 cây (`bw_cellcheck` phụ: map 11 rev 6 = 101824,165792; 78/29 = 50464,103616; 80/36 = 59648,97152) |
| Item | — | **0 item** (`item_remap.py`: 0 mục `(6,1,N)`, grep `Money|Cost|Cash|Pay|Item` = 0) |
| NPC template | `settings\npcs.txt` | 309 "Lãng nhân 3" kind 3 camp 6 (Công Bình Tử map-data) và 332 "Nam thanh niên 14" kind 3 (camper `AddNpc(332…)` `bwmission.lua:18`) — **trùng id + tên** 2 cây (dòng 311/334). `NpcName.txt` dự án: `公平子→Công Bình Tử`, `张三→Trương Tam`, `李四→Lý Tư` |
| Skill / goldequip | — | 0 (`npc_skill_remap.py`) |

---

## C. HÀM ENGINE — đặc tả từ disasm + gợi ý cài đặt (chi tiết mã: `port_bw\dac_ta_ham_bw.json`)

Bảng 57 hàm "OK" đã soi thân hàm (`bw_stubcheck_out.txt`). Các hàm thật sự chạy đúng: `AddMSPlayer/DelMSPlayer/GetNextPlayer/GetMSPlayerCount/PIdx2MSDIdx/Msg2MSAll` (nhóm 0 = tất cả, `KMission.h:263-291`), `OpenMission` (đã dò `InitMission`, `ScriptFuns.cpp:10890-10908`), `RunMission/CloseMission`, `StartMissionTimer/StopMissionTimer/GetMSRestTime` (đơn vị frame 18/s, `KTaskFuns.cpp:117-132`), `SetMissionV/S` (MissionS kho riêng 4096/subworld, `KJx2WarInfra.cpp:888`), `AddNpc(id,lv,sw,x,y,series,tên)` (`ScriptFuns.cpp:6580`), `SetNpcScript`, `ClearMapNpc` (chỉ NPC thường, `KJx2WarInfra.cpp:66-90`), `Say` với `#Hàm( n )` (`KPlayer.cpp:7488, 7011-7040` atoi bỏ khoảng trắng), `Talk` 5 trang, `SetFightState`, `SetCurCamp/GetCamp/GetCurCamp`, `ForbidEnmity` (thật, `:13833`), `DisabledStall/ForbitTrade` (thật), `SetLogoutRV`, `SetCreateTeam`, `SetDeathScript`, `SetTempRevPos(map,x,y mps)`, `SetRevPos(map,rev)|(rev)`, `SearchPlayer`, `IsCaptain`, `GetTeamSize` (=m_nMemNum+1), `LeaveTeam`, `Msg2Team`, `NpcIdx2PIdx`, `SubWorldID2Idx/Idx2ID`, `random` (builtin Lua 4), `nt_getTask/nt_setTask`.

| # | Hàm | Linux (ELF) | Dự án hiện tại | Phải làm | Bắt buộc? |
|---|---|---|---|---|---|
| C1 | `CreateChannel(tên[,loại])` `EnterChannel(pidx,tên)` `LeaveChannel(pidx,tên)` `DeleteChannel(tên)` | `0x081045A0 / 0x081044C0 / 0x08104420 / 0x08104560` → quản lý kênh chat động `0x0813AFE0` (map tên→id tại `0x9780D94`, cấp id WORD `0x813C1B0`, loại kênh `0x813B5A0`; Enter: kiểm player 1..0x4AF + `Player+0x3F0`≠0, `kênh->AddPlayer` `0x813B7A0`, gửi gói cho client `0x813AB70`) | **không đăng ký** | **4 stub `return 0`** trong `KJx2WarInfra.cpp`, đăng ký cạnh `{"SetPKFlag",…}` `ScriptFuns.cpp:~14907`. Lua 4 gọi global nil = lỗi → cắt đứt `InitMission`/`JoinCamp`/`LeaveGame`/`EndMission` | **CÓ** (chỉ mất chat riêng theo phe) |
| C2 | `AskClientForNumber(cb, min, max, prompt)` | `0x08115CA0`: gettop>3, kiểm kiểu 4 tham số, lưu script id `Player+0x5F9C`, tên cb `Player+0x5FA0` (0x80 byte), cờ chờ số `Player+0x78E8=1`, gửi gói `0xA3` dài 0x4C loại 2 kèm min/max/prompt(0x20); khi trả lời gọi `cb(số)` (bằng chứng `OnEnterKey1(Key)`) | **STUB trả −1** `KTongJX2.cpp:4132` ⇒ đội viên 2v2…8v8 **không vào được đài**; `SignUpTheOne` Lôi đài bang hội CN cũng chết | Viết thật theo khuôn `AskClientForString` (`ScriptFuns.cpp:13535-13557`): gửi `S2C_INPUT_BOX nType=2` (hộp số sẵn có của client, `KProtocolProcess.cpp:4031` → `GDCNI_OPEN_INPUT2`), `m_bWllsAskStrArg = 2`, lưu min/max (2 trường int mới `KPlayer.h:475`); ở `c2sInputCommand case 2` (`KProtocolProcess.cpp:5956-5960`) nếu cờ==2 → kiểm khoảng → `ExecuteScript(m_dwNumberBoxId, fun, nNum)` (bản int `KPlayer.cpp:6987`) | **CÓ** |
| C3 | `SetPKFlag(n)` | `0x0810F610` → `KPlayerPK::SetNormalPKState(&m_cPK=Player+0x5A50, n, bForce=1)` `0x080C3740`: ghi `m_nNormalPKFlag`=n (0/1/2), reset đồng hồ, gửi gói `0x90`; **không** ghi `m_bLockPK` (+8) | `KJx2WarInfra.cpp:227 return LuaSetFightState(L)` ⇒ PK mode không đổi ⇒ người "luyện công" đánh **không lên damage** (`KNpc.cpp:3896`), quan hệ = none (`KNpcSet.cpp:1547-1552` `!m_nPKFlag`) | `Player[i].m_cPK.SetNormalPKState((BYTE)n, m_cPK.GetLockPKState())` (`KPlayerPK.cpp:55`) — giữ khoá như Linux | **CÓ** (ảnh hưởng tốt tới 20 tệp JX2 khác đang gọi, mục G) |
| C4 | `ForbidChangePK(n)` | `0x0810F590`: `Player+0x5A58` (= `m_cPK+8` = `m_bLockPK`) = (n==1) | no-op `KJx2WarInfra.cpp:234` | `m_cPK.SetNormalPKState(GetNormalPKState(), n==1)` (tự sync client, client đã tôn trọng khoá `CoreShell.cpp:12133`); **thêm** chặn server `KProtocolProcess.cpp:5780`: `if (GetLockPKState() && !pApply->m_bLockPK) return;` | NÊN (không có thì đấu thủ tự tắt PK giữa trận) |
| C5 | `ChangeOwnFeature(0,0,-1,-1,-1,-1,-1)` khán giả tàng hình | `0x08131350` (đã dịch ngược đợt phường): nIdx<0 + 4 phần = −1 ⇒ client không vẽ thân | `ScriptFuns.cpp:3927 if (nIdx <= 0) return 0` ⇒ **no-op**, khán giả hiện hình | Nhánh `nTop==7 && nIdx<0 && 4 phần <0` → `Npc.m_HideState.nTime = 0x7FFFFFF0` (STATE_HIDE: client không tạo NPC `KProtocolProcess.cpp:1910-1956`, `KNpc::Paint:7098`; giảm 1/frame `KNpc.cpp:1288`); `LuaRestoreOwnFeature` (`KJx2WarInfra.cpp:247`) thêm `ZeroMemory(&m_HideState)`. **Không** dùng `camp_audience=7` (≠ camp_begin ⇒ có thể bị đánh, `KNpcSet.cpp:143-164`) | KHÔNG bắt buộc (khán giả camp 0 = `camp_begin` ⇒ `relation_ally` với mọi người, không bị đánh) |
| C6 | Timer mission gọi `OnTimer` | script Linux định nghĩa `OnTimer` (`bwtimer.lua:4`, `bwtotaltimer.lua:3`) | `KMission.cpp:342` gọi **`"OnMissionTimer"`**, `KLuaScript::CallFunction` không fallback (`Engine/Src/KLuaScript.cpp:200-218`) ⇒ **2 timer bw không bao giờ chạy** | **Cách A (khuyên)**: `g_MissionTimerCallBackFun` dò `OnMissionTimer`, không có thì `OnTimer` (khuôn `LuaInitMission` `ScriptFuns.cpp:10893-10907`). **Cách B**: thêm dòng cuối 2 tệp `OnMissionTimer = OnTimer` (lệch byte) | **CÓ** |
| C7 | Script chết gọi `OnDeath(Launcher)` | `bwdeath.lua:2` `OnDeath(Launcher)`, Launcher = chỉ số NPC kẻ giết (`:8 NpcIdx2PIdx`) | `KNpc.cpp:1542` gọi **`"OnPlayerDeath"(pidx, killerNpcIdx)`** ⇒ người chết không bị loại, trận không kết thúc theo mạng | **Cách A**: dò `OnPlayerDeath`, không có → `ExecuteScript2(id,"OnDeath", m_nLastDamageIdx, m_nPlayerIdx)`. **Cách B**: thêm vào cuối `bwdeath.lua`: `function OnPlayerDeath(nPlayerIdx, nDamageIdx) OnDeath(nDamageIdx) end` | **CÓ** |
| C8 | `DisabledUseTownP(1)` | `0x08130A80`: bật bit `0x100000` cờ người chơi (`0x80A8C80/0x80A9370`) | no-op `KJx2WarInfra.cpp:241` | **Giữ no-op**: Thổ Địa Phù dự án đã chặn 209-211 trong script (`item\ib\thodiaphuvh.lua:32`) | không |
| C9 | `SetPunish(n)` | `0x0810F470`: `Npc+0x1818 = (n!=0) ? 0 : 3` — `+0x1818` = `m_nCurPKPunishState` (Linux `DeathCalcPKValue 0x0807A3C2 cmp …,3 ⇒ return 3` ≡ `KNpc.cpp:8678`). **SetPunish(0) = KHÔNG phạt**, (1) = phạt | `ScriptFuns.cpp:12797`: 0 → phạt, ≠0 → không phạt — **NGƯỢC** (script JX1 `mainloidai.lua:43` dùng đúng nghĩa JX1) | **Không đổi engine**; đảo 2 dòng trong script bw (H4) | **CÓ** (script) |
| C10 | `GetTeamMember(n)` | `0x08115530`: n=1 → đội trưởng (`Team+0`, bảng `0x8BB86E8` stride 0x30); n≥2 → thành viên hợp lệ thứ n−1 (bỏ ô −1) | `ScriptFuns.cpp:7235`: **0** = đội trưởng, 1..6 = `m_nMember[n-1]` theo ô (đội trưởng không nằm trong m_nMember, `KPlayerTeam.cpp:690-718`) | sửa script (H6) | **CÓ** (script) |
| C11 | Đổi map không gỡ khỏi mission | Linux: citywar_arena gốc vẫn gọi `LeaveGame()` từng người trong GameOver (`citywar_arena/head.lua:73-76`); riêng bw chỉ `NewWorld` (`bwhead.lua:98-101`). **CHƯA RÕ** ChangeWorld Linux có gỡ mission không | `KNpc.cpp:9852, 9876` `m_MissionArray.RemovePlayer` **bị chú thích**; `StopMission` (`KMission.cpp:112-130`) xoá danh sách **không gọi OnLeave** ⇒ người ra khỏi đài còn nguyên cờ cấm giao dịch/bày bán/lập đội/khoá PK | sửa script (H5) | **CÓ** (script) |

Đã có sẵn, không phải làm: `ForbidEnmity` (`ScriptFuns.cpp:13833`, thật), `ChangeOwnFeature` 3 tham số, `RestoreOwnFeature` (= `ReSetMask`), `SetPKMode` (`:14348`, dùng làm lõi cho C3).

---

## D. BẢNG REMAP

| Loại | Linux | Dự án | Kết luận |
|---|---|---|---|
| Item | — | — | **0** (không một `(6,1,N)`, không `AddItem/AddGoldItem`) |
| NPC tpl 309 Công Bình Tử | dòng 311 "Lãng nhân 3" kind 3 camp 6 | y hệt | **TRÙNG** |
| NPC tpl 332 camper | dòng 334 "Nam thanh niên 14" kind 3 camp 6 | y hệt | **TRÙNG** (`bwmission.lua:18`) |
| NPC tpl 323 (map-data 209 dự án) | — | 2 NPC "张三/李四" script `bwcamper.lua` trong region map 209 **của dự án** (không có ở Linux yanwuchang) | engine bỏ NPC map-data; bwmission tự AddNpc(332) ⇒ bỏ qua |
| Skill | 0 | 0 | — |
| goldequip | 0 | 0 | — |
| RevivePos 6/29/36 | — | trùng từng số | — |
| Task 300/301/302 | vị trí về | dự án `lib_task.lua:141-143` `T_SAVE_TK_KILLPLAYER/KILLNPC/DEATH` (Tống Kim lưu/khôi phục `mobinhtk.lua:331-333`, `lib_tktc.lua:748`) | **VA CHẠM** (citywar_arena đã port cũng dùng 300-302 — `citywar_arena/manager.lua:25-27`, `citywar_global/infocenter_head.lua:74`). Khuyên đổi bw sang **2340/2341/2342** (trống ở cả hai cây: dự án dải 1758-2348 trống, Linux grep `Task(2340..2342)` = 0) — H3 |
| TaskTemp 200 | cờ "đang trong trận" | citywar_arena/city cũng dùng 200 cùng nghĩa | chấp nhận |
| Mission id 4 | `missions.txt` Linux dòng 5 | dự án 4 = Phong Lăng Độ (`MS_PLANGDO`, `lib_task.lua:285`) | **→ 11** (dòng 12 `mission_trong.lua`; `OpenMission(11)` grep 0) |
| Timer id 10/11 | Linux `timertask.txt:10-11` | 10 = `task10.lua` đang sống | **→ 20/21** |

---

## E. LỊCH & KÍCH HOẠT

**Không có lịch, không relay, không driver.** Trận do người chơi kích hoạt: `SignUpFinal` → `OpenMission(BW_MISSIONID)` trên SubWorld 209 (`bwmanager.lua:70`) → `InitMission` khởi 2 timer mission (`bwmission.lua:12-13`, 360 frame = 20 s và 12 960 frame = 12 phút). Mỗi lúc chỉ 1 trận toàn server (`ms_state ~= 0 → ErrorMsg(8)`, `bwmanager.lua:66-69`). Không cần gì trong `timerserver.lua`.

---

## F. LỐI VÀO — NPC Công Bình Tử

Linux **không** có script nào AddNpc Công Bình Tử (grep `bwmanager` trong `script\` = 0 ngoài 2 wrapper) ⇒ NPC nằm trong **map-data** (`maps.pak` → `<map>\v_NNN\NNN_Region_S.dat`, mục NPC). Đọc bằng `port_bw\bw_regionnpc.py` — **12 NPC, trùng từng trường ở maps.pak Linux và dự án** (`bw_npc_congbinhtu_{linux,duan}.txt` diff sạch): tpl **309**, kind 3, camp 6, tên `公平子`:

| Thành (map) | Ô (x,y) | Script |
|---|---|---|
| Dương Châu 80 | (1659,3020) (1852,3049) (1627,3208) (1709,3251) | `\script\missions\bw\bwmanager.lua` |
| Tương Dương 78 | (1464,3183) (1705,3235) (1474,3272) (1577,3376) | `\script\missions\bw\bwmanager_xiangyang.lua` |
| Thành Đô 11 | (3071,5002) (3225,5042) (3040,5096) (3166,5195) | `\script\missions\bw\bwmanager_chengdu.lua` |

Engine dự án bỏ NPC thoại trong map-data (`KRegion.cpp:474-480`: khi `g_NotAddNpcNormal` chỉ `AddNpcSet3` cho `shKind != kind_dialoger`; `settings\gamesetting.ini:259 NotAddNpcNormal=1`; Công Bình Tử kind 3 = dialoger ⇒ bị bỏ) ⇒ **tệp mới** `script\missions\bw\bw_addnpc.lua` theo đúng khuôn `task\tollgate\tinsu_addnpc.lua:33-41`:
```lua
-- bw_addnpc.lua (MOI): 12 NPC Cong Binh Tu doc tu region maps.pak (port_bw\bw_regionnpc.py), engine bo NPC map-data
bw_dialognpc = {
  {309, 80, 1659, 3020, "\\script\\missions\\bw\\bwmanager.lua",           "Công Bình Tử"},
  ... (12 dòng theo bảng trên; tên TCVN3 = bytes 43 AB 6E 67 20 42 D7 6E 68 20 54 F6 — lấy từ NpcName.txt)
}
function bw_addnpc()
  for i = 1, getn(bw_dialognpc) do
    local t = bw_dialognpc[i]; local SId = SubWorldID2Idx(t[2])
    if (SId >= 0) then
      local idx = AddNpc(t[1], 1, SId, t[3] * 32, t[4] * 32, 0, t[6]); SetNpcScript(idx, t[5])
    end
  end
end
```
Nối dây `script\startgame.lua`: thêm `Include("\\script\\missions\\bw\\bw_addnpc.lua")` cạnh dòng 18 và gọi `bw_addnpc()` ngay sau `tinsu_addnpc()` (dòng 107).
Menu NPC (bwmanager.lua:26/28): *"Được thôi!" → chọn cỡ trận* | *"Ta là đội trưởng, muốn biết số thứ tự"* | *"Ta là tuyển thủ, muốn vào đấu trường"* (đội trưởng vào thẳng; đội viên nhập số) | *"Ta muốn xem trận đấu này"* (khán giả). Trong đài: 2 NPC camper Trương Tam/Lý Tư (AddNpc lúc InitMission) → *"Phải, ta ra đây một lát!"* / hỏi số thứ tự.
Hỗn Chiến vẫn vào bằng NPC 240 Ba Lăng Huyện (`startgame\thon\balanghuyen.lua:84`) — không đụng.

---

## G. XUNG ĐỘT & CÁCH GIẢI QUYẾT

| # | Xung đột | Bằng chứng | Giải quyết |
|---|---|---|---|
| G1 | **Map 209 ↔ Lôi Đài Hỗn Chiến** | `tinhnang\loidaihonchien\mainloidai.lua:46 NewWorld(209, 1628, 3213)`; bw `bwhead.lua:11-13, 183, 197`, `bwmanager.lua:15/63/132/151/167/178/193` đều là 209 | Hỗn Chiến **là tính năng khác** (đánh tự do camp 4, `SetPKMode(2,1)`, `SetMask(2019)`, thưởng exp `bigiet.lua:10-16`, 16h/22h) ⇒ **giữ**, sửa 1 dòng `mainloidai.lua:46` → `NewWorld(210, 1628, 3213)`. Map 210 cùng `.wor`/region (`maplist.ini:2285`), Thổ Địa Phù đã chặn 210 (`thodiaphuvh.lua:32`). `bigiet.lua` không tham chiếu map. Không có `timerserver` nào đuổi người khỏi 209 (grep `\b209\b` trong `script\` chỉ còn skill id `hocvocong.lua:171`, `codenew.lua:833` — không phải map). Phương án phụ nếu chủ game không muốn dời: `bwmanager.lua main()` thêm chặn giờ 16:00-16:59 / 22:00-22:59 — phức tạp hơn, không khuyên. |
| G2 | Mission 4 | `lib_task.lua:285 MS_PLANGDO = 4`, `missions.txt` dòng 5 `mission04.lua` (Phong Lăng Độ) | bw → **11** (H1) |
| G3 | Timer 10/11 | `TimerTask.txt` dòng 11 `task10.lua` | bw → **20/21** (H2) |
| G4 | Task 300-302 ↔ Tống Kim | mục D | bw → **2340-2342** (H3) — *hoặc* chấp nhận rủi ro nhỏ (chỉ hỏng khi đi bw giữa lúc bị đá khỏi TK và vào lại) |
| G5 | `SetPKFlag` sửa nghĩa (C3) tác động 20 tệp JX2 khác | `grep SetPKFlag`: citywar_arena/city, clearskill, leaguematch/combat, tong/*, messenger/* — **toàn bộ là script JX2 port**, JX1 gốc không dùng | Đổi là **đúng Linux** cho tất cả; Lôi đài bang hội CN (đang chờ restart) phải test lại PK. |
| G6 | `SetPunish` ngược nghĩa (C9) ở các cây JX2 đã port | `citywar_arena/head.lua:48/100`, `citywar_city/camper.lua:10/142`, `leaguematch/head.lua:521`, `tong/*`, `messenger/*` (57 tệp gọi SetPunish, trộn JX1 + JX2) | **Ngoài phạm vi bw** — báo chủ game: các cây JX2 đang *phạt khi chết trong trận* và *miễn phạt sau khi rời trận*. |
| G7 | Timer mission `OnTimer` (C6) ở cây khác | `citywar_arena/timer.lua:3`, `tong/*/schedule/*.lua:3` đều `OnTimer`; `leaguematch` dùng GlbTimer riêng (`KJx2League.cpp:1225` gọi "OnTimer") nên không ảnh hưởng | Cách A của C6 sửa luôn cho tất cả. |
| G8 | `GetTeamMember` lệch chỉ số (C10) ở Tín Sứ | `task\tollgate\messenger\lib_messenger.lua:37,76,123,161` lặp `for i=1,nMemCount GetTeamMember(i)` theo nghĩa Linux ⇒ trên JX1 bỏ sót đội trưởng, đọc ô trống | **Ngoài phạm vi** — báo. |
| G9 | `header\loidai.lua` | mã chết, không ai Include | bỏ qua / xoá |

---

## H. DANH SÁCH ĐÍCH DANH TỪNG CHỖ SỬA (chuỗi cũ → mới)

Khuyên viết `ReverseTools\port_bw\bw_patch.py` kiểu `tinsu_patch.py` (assert số lần thay). Tất cả tệp là byte GBK/TCVN3 — **không** mở bằng Edit/Write, dùng python bytes.

| # | Tệp:dòng | Cũ | Mới | Lý do |
|---|---|---|---|---|
| H1 | `bwhead.lua:40` | `BW_MISSIONID = 4;` | `BW_MISSIONID = 11;` | khe mission (G2) |
| H2 | `bwhead.lua:34-35` | `BW_SMALLTIME_ID = 10;` / `BW_TOTALTIME_ID = 11;` | `= 20;` / `= 21;` | khe timer (G3) + `TimerTask.txt` mục B |
| H3 | `bwhead.lua:23-25` | `BW_SIGNPOSWORLD = 300; BW_SIGNPOSX = 301; BW_SIGNPOSY = 302;` | `2340; 2341; 2342;` | tránh đè Tống Kim (G4) — *tuỳ chọn nhưng khuyên* |
| H4 | `bwhead.lua:62` và `:144` | `SetPunish(1)--设置PK惩罚` / `SetPunish(0);` | `SetPunish(0)` / `SetPunish(1);` | JX1 ngược nghĩa (C9) |
| H5 | `bwhead.lua:99-100` (GameOver) | `PlayerIndex = PTab[i];\n\t\tNewWorld(...)` | chèn `LeaveGame();` trước `NewWorld(...)` | JX1 không gỡ mission khi đổi map (C11); giống `citywar_arena/head.lua:75` |
| H6 | `bwmanager.lua:86-87` | `for i = 1, 2 do \n\t\tPlayerIndex = GetTeamMember(i);` | `local tbMem = {GetTeamMember(0), 0}; for k = 1, 6 do local m = GetTeamMember(k); if (m > 0 and m ~= tbMem[1] and tbMem[2] == 0) then tbMem[2] = m end end; for i = 1, 2 do PlayerIndex = tbMem[i];` | JX1: 0 = đội trưởng, 1..6 = ô thành viên có thể trống (C10). Kèm guard `if tbMem[2] <= 0 then ErrorMsg(1) return end` trước `OpenMission` |
| H7 | `bwmanager.lua:7` | `do Talk(1, "", "Chức năng đã đóng.") return end` | **xoá dòng** | mở khoá (DIEUKIEN #1) |
| H8 | `bwmanager.lua:37-42` (OnRegister, sau kiểm `GetTeamSize() ~= 2`) + `:52-56` (SignUpFinal, sau vòng H6 lấy 2 người, trước `OpenMission`) + `:192-198` (OnJoin, nhánh `group == 1 or group == 2` và nhánh đội trưởng) | — | thêm `if (GetLevel() < 90) then Say("Phải đạt cấp <color=yellow>90<color> trở lên mới được tham gia Lôi đài thi đấu.", 0) return end` (SignUpFinal kiểm **cả 2** người bằng `PlayerIndex = tbMem[i]`). Khán giả (`group == 3`) **không** chặn | chính sách cấp 90 (DIEUKIEN #2) |
| H9 | `bwhead.lua:17` | `{1608, 3211, "L?T?"},` | `{1608, 3211, "Lý Tư"},` (bytes `4C FD 20 54 AD`, theo `NpcName.txt` `李四`) | tên camper bị hỏng sẵn ở bản VNG — *tuỳ chọn* |
| H10 | `bwmanager.lua:146` | `"Xin nhập số thứ tự vào đấu trường:"` (34 byte) | `"Nhập số thứ tự vào đấu trường:"` (30 byte) | `S2C_INPUT_BOX.Value[32]` (`KProtocol.h:1878-1884`) cắt ở 31 byte |
| H11 | (chỉ khi chọn cách B ở C6/C7) `bwtimer.lua`, `bwtotaltimer.lua` cuối tệp | — | `OnMissionTimer = OnTimer` ; `bwdeath.lua` cuối: `function OnPlayerDeath(nPlayerIdx, nDamageIdx) OnDeath(nDamageIdx) end` | tên callback JX1 |
| H12 | `branch_bwsj.lua:2` | `Include("\\script\\battles\\battlehead.lua")` | `--Include(...)  -- [BW 22/08] chi phuc vu branchTask_JoinSJend1 (khong ai goi), keo 26 tep Tong Kim JX2` | cắt bao đóng (A.2) |
| H13 | `tinhnang\loidaihonchien\mainloidai.lua:46` | `NewWorld(209, 1628 , 3213);` | `NewWorld(210, 1628 , 3213);` | nhường 209 cho bw (G1) |
| H14 | `settings\task\missions.txt` dòng 12 | `11<TAB>\script\missions\mission_trong.lua` | `11<TAB>\script\missions\bw\bwmission.lua` | B |
| H15 | `settings\TimerTask.txt` | — | `20<TAB>\script\missions\bw\bwtimer.lua` / `21<TAB>\script\missions\bw\bwtotaltimer.lua` | B |
| H16 | `script\startgame.lua:18` và `:107` | — | `Include("\\script\\missions\\bw\\bw_addnpc.lua")` / `bw_addnpc()` | F |

Không có chỗ nào về trùng sinh. Không đổi chuỗi thoại nào khác (giữ cả `ErrorMsg(2/6/9)` về "ngân lượng" vốn không dùng).

---

## I. RỦI RO + CÁCH TEST SAU RESTART

**Rủi ro**
1. Engine: 5 chỗ C++ (C1-C4 + C6/C7 cách A, C5 tuỳ chọn) — `CoreServer.dll` build lại, **không đụng client** (hộp số `nType=2` client đã có).
2. `Msg2MSAll`/`GetMissionV` trong `OnTimer` cần `SubWorld` = map 209: `KMission::ExecuteScript` đã bơm `SCRIPT_SUBWORLDINDEX` (`KMission.cpp:86-92`) ✓.
3. `OnLeave` bw gọi `NewWorld` cả khi **đăng xuất** (`KPlayerSet.cpp:375 → RemovePlayer → OnLeave`) — chưa có mission nào của dự án NewWorld trong OnLeave; cần test đăng xuất trong đài rồi vào lại (`SetLogoutRV(1)` đã buộc hồi sinh ở thành).
4. Người chết bị `DelMSPlayer → OnLeave → NewWorld` khi **đang chết** (xác) — Linux cũng vậy; test hồi sinh (điểm `SetTempRevPos` = chỗ đứng lúc báo danh).
5. `SetRevPos(80,36)/(29)/(6)` đổi **vĩnh viễn** điểm hồi sinh sang thành báo danh (Linux cũng không khôi phục).
6. `AskClientForNumber`: Linux xoá cờ chờ khi script khác chạy; JX1 không — trả lời muộn vẫn nhận. Nếu số sai → `Say` "Nhập số thứ tự không đúng" (`bwmanager.lua:161`).
7. Khán giả không tàng hình nếu bỏ C5 — nhưng không thể bị đánh (camp 0).
8. `CloseMission` gọi từ **bên trong** `OnTimer` (`bwtimer.lua:84/93/102`, `bw_all_gone:176`) ⇒ xoá timer đang duyệt — citywar_arena cùng kiểu, chưa thấy sập, vẫn để ý.
9. `printf("==>Run timer")` `KMission.cpp:344` in console mỗi 20 s suốt trận.
10. **Lỗ hổng chung phát hiện khi làm bw** (ngoài phạm vi, phải báo): C3/C6/C9/C10 ảnh hưởng Lôi đài bang hội CN, Hoạt động phường, Tín Sứ (G5-G8).

**Test (GM, sau restart, 3 nhân vật A/B/C ≥ 90)**
1. Log boot: `ScriptError.log` không có `missions\bw`; 12 Công Bình Tử hiện ở 3 thành (toạ độ mục F).
2. A+B lập đội (2 người), A bấm Công Bình Tử → "Được thôi!" → "2 vs 2" → A và B mỗi người nhận `Msg2Player` số thứ tự; `Msg2Team` "trận đấu sẽ chính thức bắt đầu sau 2 phút". Kiểm `GetMissionV` trên 209: state 1, MissionS(1)/(2) = tên A/B.
3. Cấp < 90: "Phải đạt cấp 90…" ở OnRegister / OnJoin.
4. A bấm lại NPC → "Ta là tuyển thủ…" → vào 209 tại (1620,3202), camp 2, PK state 1 (client hiện "chiến đấu", khoá nút PK); B → (1612,3187), camp 3.
5. C (không trong đội) → "Ta là tuyển thủ" → nhập số của A → vào phe A (đếm `GetMSPlayerCount(11,1)` = 2); nhập sai → thông báo sai; nhập số B khi phe B đủ → `ErrorMsg(10)`.
6. D → "Ta muốn xem" → vào (1598,3216), camp 0, không bị đánh, (C5) người khác không thấy.
7. Mỗi 20 s có thông báo đếm ngược (chứng tỏ C6 đúng); hết 2 phút → "thi đấu chính thức bắt đầu", cả hai phe `SetFightState(1)`, đánh **lên damage**, **không mất** exp/đồ khi chết (C9), kẻ chết bị loại + thông báo "X đã đánh bại Y" (C7).
8. Một phe chết hết → "giành thắng lợi chung cuộc", mọi người về đúng chỗ đứng lúc báo danh, **giao dịch/bày bán/lập đội** dùng lại được ngay (H5).
9. Không ai vào sau 2 phút → "trận đấu hủy bỏ"; chỉ 1 phe vào → phe kia được xử thắng.
10. Để hết 12 phút → bwtotaltimer xử theo số người còn lại.
11. Hỗn Chiến 16:00: NPC 240 đưa vào **map 210**; bw ở 209 không bị xoá NPC camper bởi Hỗn Chiến và ngược lại.
12. Đăng xuất giữa trận → vào lại ở thành báo danh, không còn cờ cấm.
