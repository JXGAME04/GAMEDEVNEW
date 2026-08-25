# 01 — SĂN BOSS SÁT THỦ (Killer Boss) — dịch ngược bản Linux JX2

Nguồn: `D:\ServerLinux\server1` (jx_linux_y, ELF32 i386, section header đã xoá).
Đối chiếu: `D:\GAMEDEVNEW\Sources\Core\Src` (engine JX1) và cây máy chủ JX1 **đang chạy**
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`.
Mọi khẳng định đều kèm `tệp:dòng` hoặc địa chỉ ELF. Chỗ nào chưa kiểm được ghi rõ **CHƯA XÁC MINH**.

---

## 0. TÓM TẮT NHANH

| Hạng mục | Số liệu |
|---|---|
| Tệp .lua đã đọc hết | 7 tệp hạt giống + 25 tệp liên quan = **32** [đã sửa theo phản biện 2] |
| Tệp trong bao đóng Include (`closure3.json` mục `satthu`) | 67 |
| Hàm Lua do **script** định nghĩa | **26** (lõi) + **13** (móc nối) = **39** |
| Hàm do **engine** cung cấp mà hệ này gọi | **36** |
| Số NPC boss trong bảng dữ liệu | **160** (8 nhóm cấp × 20) |
| Số bản đồ chứa boss | **40** (5 bản đồ / nhóm cấp × 4 boss / bản đồ) |
| Số biến nhiệm vụ (task variable) | **4 chính** (1082, 1192, 1193, 1217) + 4 phụ hệ khác |
| Bảng dữ liệu ngoài | `settings\task\tollgate\killer\killer.txt` (160 dòng), `npcs.txt` (id 761–820), `skills.txt` (541/547/548), `magicscript.txt`, `droprate\boss\bosstask_lev*.ini`, `settings\huoyuedu\huoyuedu.txt` |

**Kết luận về `settings\killer.ini`: KHÔNG thuộc hệ này.** Xem mục 9.

---

## 1. DANH SÁCH TỆP ĐÃ ĐỌC

### 1.1 Tệp hạt giống (đọc hết, không bỏ dòng)

| Tệp | Số dòng | Vai trò |
|---|---|---|
| `script\task\tollgate\killer\kill_level.lua` | 123 | Script gắn vào NPC boss — xử lý `OnDeath`, phát thưởng, bắn sự kiện |
| `script\task\tollgate\killer\lib_killlevel.lua` | 148 | Thư viện phát thưởng cho **cả tổ đội** (`SetMemberTask`) + bảng sát thủ lệnh theo cấp |
| `script\task\tollgate\killer\nieshichen.lua` | 232 [đã sửa theo phản biện 2] | Script NPC **Nhiếp Thí Trần** — giao nhiệm vụ, hợp thành sát thủ giản |
| `script\task\tollgate\killer\mibao_head.lua` | 22 [đã sửa theo phản biện 2] | Bảng dữ liệu phần thưởng hộp "Sát Thủ Bí Bảo" (11 dòng) |
| `script\task\tollgate\killer\shashou_mibao.lua` | 28 [đã sửa theo phản biện 2] | Script vật phẩm "Sát Thủ Bí Bảo" (mở hộp) |
| `script\task\tollgate\addtollgatenpc.lua` | 21 | Gọi `add_killertasknpc` cho các bảng phụ (đường ải) |
| `script\task\tollgate\killbosshead.lua` | **3421** | Bảng dữ liệu NPC + 3 hàm nạp NPC |

### 1.2 Tệp liên quan trực tiếp đã đọc

`script\global\autoexec.lua` (:3, :158) · `script\global\autoexec_npc.lua` (:26–32) ·
`script\task\newtask\newtask_head.lua` (:13–20, :31–39) · `script\tong\tong_award_head.lua` (:119) ·
`script\activitysys\npcfunlib.lua` (:142) · `script\activitysys\detailtype\finishkillerboss.lua` ·
`script\activitysys\g_activity.lua` (:34) · `script\misc\eventsys\eventsys.lua` · `script\misc\eventsys\type\func.lua` (:33) ·
`script\huoyuedu\huoyuedu.lua` (:26) · `script\global\mantlesystem\other_func_outputs.lua` (:39, :175) ·
`script\event\beidoulingpai\beidouactivity.lua` (:460, :481) · `script\event\longmenbiaoju\gaojiweirenzhuang.lua` ·
`script\global\achievementsys\type\killertask.lua` · `script\event\change_destiny\mission.lua` (:80) ·
`script\vng_feature\challengeoftime\npcNhiepThiTran.lua` · `script\missions\challengeoftime\rank_perday.lua` ·
`script\missions\challengeoftime\npc\dragonboat_main.lua` · `script\global\pgaming\xephang\bangxephang.lua` (:27, :80) ·
`script\gmscript\gmcommand_3.lua` (:458–488) · `script\item\killer_token.lua` · `script\item\killer_sword.lua` ·
`script\skill\npc\killerbossmianyi.lua` · `script\lib\awardtemplet.lua` · `script\task\task_award_extend.lua`

---

## 2. LUỒNG CHƠI ĐẦY ĐỦ

### 2.1 Khởi động máy chủ — sinh NPC

1. `script\global\autoexec.lua:3` `Include("\\script\\task\\tollgate\\killbosshead.lua")`.
2. `script\global\autoexec.lua:158` gọi `add_killertasknpc(addkillertasknpc)` → sinh **160 NPC boss** trên 40 bản đồ.
3. `script\global\autoexec.lua` nạp `adddialognpc` (`autoexec_npc.lua:26–32`) → sinh **7 NPC Nhiếp Thí Trần**
   (id mẫu 769) tại 7 thành: map **11** Thành Đô, **1** Phượng Tường, **37** Biện Kinh, **176** Lâm An,
   **162** Đại Lý, **78** Tương Dương, **80** Dương Châu.

### 2.2 Nhận nhiệm vụ (NPC Nhiếp Thí Trần)

`nieshichen.lua:56` `main()` mở hộp thoại 8 mục:

| Mục | Nội dung | Hàm |
|---|---|---|
| 1 | Thí luyện sát thủ (dẫn sang **Vượt Ải**) | `annealofkiller()` :65 |
| 2 | Hợp thành sát thủ giản | `compose()` :156 |
| 3 | **Nhiệm vụ sát thủ cấp 90** | `killer90()` :122 |
| 4 | Từ bỏ nhiệm vụ | `cancel()` :130 |
| 5 | Rời đi | `no()` :209 |
| 6 | Ta đến nhận thưởng | `rank_award()` — `missions\challengeoftime\rank_perday.lua:15` |
| 7 | Xem xếp hạng 5 đội cao nhất hôm nay | `get_top5team()` — `rank_perday.lua:77` |
| 8 | Đóng | `ContentList[15]` |
| (+1 có điều kiện — **KHÔNG BAO GIỜ HIỆN, LỖI GỐC** [đã sửa theo phản biện 2]) | Nhiệm vụ "Quá quan tầm bảo" khi `tbBirthday0905:IsActDate()==1` (:59–61). `tinsert(tbDialog, 12, ContentList[29])` chèn vào **chỉ số 12** của bảng chỉ có **8** phần tử; `luaB_tinsert` (Lua 4.0, `Sources\Library\LuaLib\src\baselib\lbaselib.c:391–410`) chỉ đặt `t.n = n+1 = 9` rồi `t[12] = v` (vòng dịch `for(; n>=pos; n--)` không chạy) ⇒ `getn(tbDialog) = 9`, `unpack` trả `t[1..9]` với **`t[9] = nil`**, còn `t[12]` không ai đọc. ⇒ ngày sinh nhật chỉ thêm **một ô rỗng**, mục "Quá quan tầm bảo" **không hiện**. | `guoguan_xunbao()` |

**🔴 Trong bản này CHỈ nhóm cấp 90 còn hoạt động.** `killer20()` … `killer80()` bị **comment hết**
(`nieshichen.lua:73–120`). Chỉ `killer90()` (:122) còn sống. Nhưng `add_killertasknpc` vẫn sinh
đủ **160** boss và `kill_level.lua:25` vẫn xử lý dải `1..160` → 140 boss cấp 20–80 **vẫn đứng trên bản đồ
nhưng không ai giao được nhiệm vụ cho chúng** (trừ GM, xem 2.7).

**Kiểm tra điều kiện — `killerCoundTakedTask(nLowLevel, nHighLevel)` (`nieshichen.lua:213`)**, gọi với `(90, 350)`:

```
1. havetask()                : nếu GetTask(1082) ~= 0 -> đang có nhiệm vụ, từ chối (ContentList[13])
2. GetLevel() < 90 hoặc > 350: từ chối (ContentList[14]) -- 350 = mở cho nhân vật chuyển sinh
3. nDate = tonumber(GetLocalDate("%y%m%d"))     -- ngày dạng YYMMDD
   myDate = GetTask(TSKID_KILLERDATE=1192)
   nếu myDate == nDate và GetTask(1193) >= KILLER_MAXCOUNT(8) -> HẾT LƯỢT HÔM NAY
   ngược lại nếu myDate ~= nDate -> reset: SetTask(1193, 0); SetTask(1192, nDate)
4. return 1
```

Sau đó `showboss(140)` (:126) hiện **10 boss đầu** của nhóm 90 (chỉ số 141–150) + nút "Trang kế";
`showbossnext(140)` (:147) hiện 151–160 + "Trang trước". Tên boss lấy từ
`killertabfile:getCell("BossName", n)` — bảng `\settings\task\tollgate\killer\killer.txt`
(`nieshichen.lua:54`, đối tượng `KTabFile`).

Chọn 1 boss → `givetask(taskid)` (:151):
```lua
nt_setTask(1082, taskid)      -- taskid = 1..160 (thực tế chỉ 141..160)
Describe(... BossName .. BossInfo ...)   -- BossInfo cho biết TÊN HANG + toạ độ ô
```
`nieshichen.lua:127` ghi log: `tbLog:PlayerActionLog("TinhNangKey","NhanNhiemVuBossSatThu")`.

### 2.3 Tìm & giết boss

- Người chơi tự đi tới bản đồ ghi trong `BossInfo`. NPC boss đã đứng sẵn ở đó (spawn khi boot),
  **không phải triệu hồi**, chết thì hồi sinh sau `ReviveFrame = 16200` khung
  (`npcs.txt` cột 85 của id 761) ≈ 16200/18fps = **900 giây = 15 phút**.
- Boss mang kỹ năng bị động `PasstSkillId = 547` = "Sát thủ - Vòng tròn miễn dịch"
  (`skills.txt` id 547, script `\script\skill\npc\killerbossmianyi.lua`):
  `fasthitrecover_v=100`, `fatallystrikeres_p=99`, `freezetimereduce_p=200`,
  `poisontimereduce_p=180`, `stuntimereduce_p=200` → **gần như miễn choáng/đóng băng/chí mạng**.
- `LevelScript = \script\npclevelscript\task_killboss.lua` quyết định chỉ số theo cấp.

### 2.4 Khi boss chết — `kill_level.lua`

Engine gọi `OnDeath(nNpcIndex)` trên **ActionScript** của NPC (đặt bởi `SetNpcScript`), với biến
toàn cục `PlayerIndex` = người ra đòn cuối.

```
OnDeath(nNpcIndex)                                   kill_level.lua:22
  └ nếu 1 <= GetTask(1082) <= 160 → kill_level20(nNpcIndex)          :25-27

kill_level20(nNpcIndex)                              kill_level.lua:34
  Uworld1082 = GetTask(1082)                                          :35
  nNpcSign   = GetNpcParam(nNpcIndex, 1)   -- chỉ số 1..160 gắn khi spawn :36
  nseries    = NPCINFO_GetSeries(nNpcIndex) -- ngũ hành THẬT của boss    :37
  Uworld1217 = GetTask(1217)                                          :38
  NẾU Uworld1082 == nNpcSign THÌ   -- đúng boss được giao              :39
     SetTask(1217, Uworld1217 + 1)          -- +1 tổng số boss đã săn  :41
     Ladder_NewLadder(10119, GetName(), Uworld1217+1, 1)  -- bảng xếp hạng :44
     NẾU 141 <= Uworld1082 <= 160 THÌ                                  :67
        tbChangeDestiny:completeMission_Killer()   -- nhiệm vụ Đổi Mệnh :68
        ITEM_DropRateItem(nNpcIndex, 8, "\settings\droprate\boss\bosstask_lev90.ini", 0, 10, nseries) :69
     G_ACTIVITY:OnMessage("FinishKillerBoss", PlayerIndex, 1082, Uworld1082, 0)  :71
     EventSys:GetType("KillerBoss"):OnPlayerEvent("OnKillBoss", PlayerIndex, 1082, Uworld1082) :72
     EventSys:GetType("KillerBoss"):OnPlayerEvent("OnFinish",   PlayerIndex, 1082, Uworld1082) :74
     SetMemberTask(1082, Uworld1082, 0, killbossall, nseries)          :75
```

> Các nhánh `ITEM_DropRateItem` cho nhóm cấp 20–80 (`:46–66`) đều **bị comment**, chỉ nhóm 90 còn rơi đồ theo ini.

### 2.5 Phát thưởng — `lib_killlevel.lua:21 SetMemberTask`

```
SetMemberTask(myTaskNumber=1082, myOrgValue=<chỉ số boss>, myTaskValue=0, fnCallback=killbossall, series)

  nPreservedPlayerIndex = PlayerIndex
  nMemCount = GetTeamSize()
  DynamicExecuteByPlayer(PlayerIndex, "\script\huoyuedu\huoyuedu.lua",
                         "tbHuoYueDu:AddHuoYueDu", "shashourenwu")        :29  ← CHỈ người ra đòn cuối

  ── nMemCount == 0 (đánh đơn) ────────────────────── :30
     nếu GetTask(1082) == myOrgValue thì
        add_shashouling(myOrgValue, series)      -- exp + sát thủ lệnh   :33
        SetTask(1193, GetTask(1193) + 1)         -- +1 lượt/ngày         :35
        tongaward_killer()                       -- cống hiến bang hội   :37
        SetTask(1082, 0)                         -- xoá nhiệm vụ         :38
        killbossall()                                                    :40

  ── nMemCount > 0 (tổ đội) ───────────────────────── :43
     nếu GetTask(1082) == myOrgValue thì tongaward_killer()  ← CHỈ 1 lần cho người ra đòn cuối :46
     vòng i = 1..nMemCount:                                              :48
        PlayerIndex = GetTeamMember(i)
        nếu GetTask(1082) == myOrgValue thì   -- thành viên cùng nhiệm vụ mới được
           add_shashouling(myOrgValue, series)                           :52
           SetTask(1193, GetTask(1193) + 1)                              :54
           SetTask(1082, 0)                                              :56
           killbossall()                                                 :59
     PlayerIndex = nPreservedPlayerIndex                                 :64
```

**Bất đối xứng đã kiểm chứng (không phải suy diễn):**
- `AddHuoYueDu` (:29) và `Ladder_NewLadder`/`SetTask(1217)` (`kill_level.lua:41,44`) chỉ chạy cho
  **người ra đòn cuối**, không chạy cho đồng đội.
- `tongaward_killer()` cũng chỉ 1 lần cho người ra đòn cuối (:46), **không** gọi trong vòng lặp.
- `local Uworld1217 = nt_getTask(1217)` ở `lib_killlevel.lua:27` là **mã chết** (khai báo, không dùng).

**`add_shashouling(nvalue, series)` — `lib_killlevel.lua:70`**

| Dải `nvalue` | Cấp nhiệm vụ | `AddOwnExp` | Vật phẩm |
|---|---|---|---|
| 1–20 | 20 | 15 000 | `AddItem(6,1,399, **20**, series, 0)` |
| 21–40 | 30 | 20 000 | `AddItem(6,1,399, **30**, series, 0)` |
| 41–60 | 40 | 30 000 | `AddItem(6,1,399, **40**, series, 0)` |
| 61–80 | 50 | 50 000 | `AddItem(6,1,399, **50**, series, 0)` |
| 81–100 | 60 | 60 000 | `AddItem(6,1,399, **60**, series, 0)` |
| 101–120 | 70 | 80 000 | `AddItem(6,1,399, **70**, series, 0)` |
| 121–140 | 80 | 100 000 | `AddItem(6,1,399, **80**, series, 0)` |
| 141–160 | 90 | 140 000 + `AddExp_Skill_Extend(140000)` | `AddItem(6,1,399, **90**, series, 0)` + **50 %** "Sát Thủ Bí Bảo" `{6,1,2347,1,1,0}` + `jiefangri_award()` |

`killbossall()` (`kill_level.lua:81`): `AddSkillState(541, 1, 0, 54)` (hiệu ứng "Hoàn thành nhiệm vụ Sát thủ",
54 khung ≈ 3 giây) + `Msg2Player("Bạn đã thu phục được tên sát thủ, hoàn thành nhiệm vụ!")`.

**`jiefangri_award()` (`lib_killlevel.lua:110`)** — sự kiện **HẾT HẠN**: chỉ chạy nếu
`ndate` (YYYYMMDDHHMM) nằm giữa 3 mốc `201104210000 / 201105020000 / 201105160000` và
`nEndDate = 201105230000` → **đã chết từ 23/05/2011**, và cần `PlayerFunLib:CheckTotalLevel(150,"",">=")`.

**Phần thưởng cố định cấp 90 — `OnFinishKillerTask` (`kill_level.lua:86`)**, đăng ký qua
`EventSys:GetType("KillerBoss"):Reg("OnFinish", OnFinishKillerTask)` (:123):
bảng `tbAward` 2 nhóm → `tbAwardTemplet:Give(tbAward, 1, {"KillerTask","FinishLevel90"})` (:119)
— nhóm `[1]` là bảng random 25 dòng, nhóm `[2]` cố định **10 000 000 điểm kinh nghiệm**.

### 2.6 Vòng lặp / giới hạn

| Giới hạn | Giá trị | Nguồn |
|---|---|---|
| Số nhiệm vụ / ngày | **8** | `KILLER_MAXCOUNT = 8` — `newtask_head.lua:20`, kiểm ở `nieshichen.lua:224` |
| Mốc reset | Đổi ngày lịch máy chủ (`GetLocalDate("%y%m%d")`) | `nieshichen.lua:222–229` |
| Chỉ 1 nhiệm vụ tại 1 thời điểm | `havetask()` chặn nếu `GetTask(1082) ~= 0` | `nieshichen.lua:135` |
| Cấp nhân vật | 90 ≤ cấp ≤ 350 | `nieshichen.lua:123` |
| Điểm hoạt động (huoyuedu) | tối đa **2 lần/ngày**, **+3 rồi +3** [đã sửa theo phản biện] | `settings\huoyuedu\huoyuedu.txt` dòng 12: `11	杀手任务	2871	2	3	3` (Param1=**3**, không phải 5) |
| Hồi sinh boss | 16200 khung ≈ 15 phút | `npcs.txt` cột `ReviveFrame` |

### 2.7 Hợp thành Sát Thủ Giản (`nieshichen.lua:156–207`)

```
compose()  -> GiveItemUI("Giao diện hợp thành sát thủ giản", <mô tả>, "exchange_token", "no")
exchange_token(ncount):
   duyệt i=1..ncount: nItemIdx = GetGiveItemUnit(i)
      itemgenre, detailtype, parttype, level, attribute = GetItemProp(nItemIdx)
      chỉ nhận (6,1,399) = Sát Thủ lệnh; tất cả phải CÙNG level
   nếu lẫn đồ khác / khác cấp   -> ContentList[18]  (:171,:182)
   nếu > 5                      -> ContentList[19]  (:186)
   nếu < 5                      -> ContentList[20]  (:190)
   nếu == 5 -> RemoveItemByIndex × 5 rồi givesword(scrollattr, compare_level)  (:193-198)

givesword(attr, level):                                              :201
   i = random(1,5)                    -- CHỌN NGẪU NHIÊN 1 trong 5 lệnh
   AddItem(6, 1, 400, level, attr[i], 0)   -- Sát Thủ Giản cùng cấp, ngũ hành = của lệnh thứ i
```
> Tức là **5 Sát Thủ lệnh cùng cấp → 1 Sát Thủ Giản cùng cấp**, ngũ hành lấy ngẫu nhiên từ 1 trong 5 lệnh.
> Chuỗi `series = {"metal>Kim","wood>Mộc","water>Thủy","fire>Hỏa","earth>Thổ "}` (:202), tra bằng `attr[i]+1`.

### 2.8 Mở "Sát Thủ Bí Bảo" (`shashou_mibao.lua`)

```
main(nItemIdx):
   n_curcnt = CalcItemCount(3, 6,1,2348, -1)          -- đếm Huyền Thiên Chùy trong hành trang
   nếu < 6 -> Msg2Player("Cần %d %s, các hạ đem không đủ số lượng!") ; return 1
   nếu CalcFreeItemCellCount() >= 3 và ConsumeItem(3, 6, 6,1,2348, -1) thì
        tbAwardTemplet:GiveAwardByList(tbItem_Mibao, "USE Sát Thủ Bí Bảo")
        AddStatData("baoxiangxiaohao_kaishashoumibao", 1)
   ngược lại -> Msg2Player("Khoảng trống trong túi không đủ!") ; return 1
```
`n_XuanTian_Chui = 6` (`shashou_mibao.lua:13`). **Huyền Thiên Chùy (6,1,2348) KHÔNG do hệ sát thủ sinh ra** —
nó đến từ `activitysys\config\17` và `bonus_onlinetime\func_onlineaward.lua:105`.

### 2.9 Lệnh GM (`gmscript\gmcommand_3.lua`)

| Hàm | Dòng | Tác dụng |
|---|---|---|
| `tbCommand3:KillBoss_TaskNpc()` | :458 | `NewWorld(11, 3207, 4978)` — dịch chuyển tới NPC Nhiếp Thí Trần (Thành Đô) |
| `tbCommand3:KillBoss_TaskItem()` | :462 | `AddItem(6,1,2346,1,0,0)` — "Thí Giả Yêu Bài" |
| `tbCommand3:KillBoss_Transfer(nLevel, nChoice)` | :468 | Liệt kê boss theo cấp từ `addkillertasknpc`, rồi `LoadMap(nMapId)` + `NewWorld(nMapId, nX, nY)` |

---

## 3. BẢNG BIẾN NHIỆM VỤ (task variable)

### 3.1 Biến chính

| Tên hằng | ID | Ý nghĩa | Giá trị | AI GHI | AI ĐỌC |
|---|---|---|---|---|---|
| `TSKID_KILLTASKID` | **1082** | Chỉ số boss đang nhận (1..160), 0 = không có | 0..160 | `nieshichen.lua:131` (`cancel`→0), `:152` (`givetask`→taskid); `lib_killlevel.lua:38,:56` (→0) | `kill_level.lua:23,35,87`; `lib_killlevel.lua:31,44,50`; `nieshichen.lua:57`; `npcfunlib.lua:148`; `metempsychosis\task_func.lua:166` |
| `TSKID_KILLERDATE` | **1192** | Ngày (YYMMDD) của lượt nhiệm vụ gần nhất | vd `260824` | `nieshichen.lua:229` | `nieshichen.lua:223` |
| `TSKID_KILLERMAXCOUNT` | **1193** | Số lần hoàn thành **trong ngày** | 0..8 | `nieshichen.lua:228` (reset 0); `lib_killlevel.lua:35,:54` (+1) | `nieshichen.lua:224`; `lib_killlevel.lua:35,54` |
| *(không có hằng — số trần)* | **1217** | **Tổng tích luỹ** số boss sát thủ đã săn (cả đời) | 0..∞ | `kill_level.lua:41` | `kill_level.lua:38`; `lib_killlevel.lua:27` (mã chết) |

Định nghĩa: `script\task\newtask\newtask_head.lua:14–16, :20`.
`nt_setTask(id, v)` = `SetTask(id,v)` + `SyncTaskValue(id)` (`newtask_head.lua:31–34`);
`nt_getTask(id)` = `GetTask(id)` (`newtask_head.lua:37–39`).

### 3.2 Biến khai báo nhưng KHÔNG dùng trong bản này

| Tên | ID | Ghi chú gốc |
|---|---|---|
| `TSKID_KILLTASKID_SPE` | 1793 | "有否接受高级杀手任务 — 收费版不用" (`newtask_head.lua:17`) — grep toàn cây: **0 điểm dùng** |
| `TSKID_KILLERDATE_SPE` | 1794 | (`newtask_head.lua:18`) — **0 điểm dùng** |

### 3.3 Biến của hệ khác bị hệ này chạm vào

| ID | Chủ sở hữu | Điểm chạm |
|---|---|---|
| **2871** | `huoyuedu` — đếm lượt nhận điểm hoạt động "sát thủ" trong ngày | `settings\huoyuedu\huoyuedu.txt` dòng 12; ghi bởi `tbHuoYueDu:AddHuoYueDu("shashourenwu")` |
| **2880 / 2882** | `huoyuedu` — tổng điểm hoạt động ngày / cờ nhận thưởng | `huoyuedu.lua:12,13` |
| `TASKID_CONTRIVALUE` | Cống hiến bang hội | `tongaward_killer()` → `ContriValueEntryLogic(10 * COEF_CONTRIB_TO_VALUE, EVE_KILLER)` (`tong_award_head.lua:120`) |
| `TSK_KILLER_ID = 1082` | Chuyển sinh | `metempsychosis\task_func.lua:166` — **chặn chuyển sinh khi đang có nhiệm vụ sát thủ** |
| `nMissionID_Killer` | Đổi Mệnh (change_destiny) | `mission.lua:80` — cần **4 lần** nhiệm vụ cấp 90 (accepted → partial 1/2/3 → completed) |

### 3.4 Danh mục ĐẦY ĐỦ mọi `GetTask/SetTask/nt_*` trong 7 tệp hạt giống

| Tệp:dòng | Lời gọi |
|---|---|
| `kill_level.lua:23` | `nt_getTask(TSKID_KILLTASKID)` = GetTask(1082) |
| `kill_level.lua:35` | `nt_getTask(TSKID_KILLTASKID)` |
| `kill_level.lua:38` | `nt_getTask(1217)` |
| `kill_level.lua:41` | `nt_setTask(1217, Uworld1217+1)` |
| `kill_level.lua:87` | `nt_getTask(TSKID_KILL)` (tham số truyền vào = 1082) |
| `lib_killlevel.lua:27` | `nt_getTask(1217)` *(mã chết)* |
| `lib_killlevel.lua:31,44,50` | `GetTask(myTaskNumber)` = GetTask(1082) |
| `lib_killlevel.lua:35,54` | `nt_setTask(1193, GetTask(1193)+1)` |
| `lib_killlevel.lua:38,56` | `nt_setTask(1082, 0)` |
| `nieshichen.lua:57` | `nt_getTask(1082)` |
| `nieshichen.lua:131` | `nt_setTask(1082, 0)` |
| `nieshichen.lua:152` | `nt_setTask(1082, taskid)` |
| `nieshichen.lua:223` | `nt_getTask(TSKID_KILLERDATE)` = 1192 |
| `nieshichen.lua:224` | `nt_getTask(TSKID_KILLERMAXCOUNT)` = 1193 |
| `nieshichen.lua:228` | `nt_setTask(1193, 0)` |
| `nieshichen.lua:229` | `nt_setTask(1192, nDate)` |

---

## 4. BẢNG NPC (`killbosshead.lua`)

### 4.1 Cấu trúc tệp `killbosshead.lua` (3421 dòng)

| Dòng | Nội dung | Ghi chú |
|---|---|---|
| 1 | `SHOUHUZHE_OFFSET = 2` | Hằng của hệ "hộ vệ" (Tín Sứ), không dùng ở hệ sát thủ |
| **4–180** | **`addkillertasknpc = { ... }` — 160 dòng dữ liệu** | Hệ SÁT THỦ |
| 183–194 | `function add_killertasknpc(Tab3)` | Nạp bảng 10 cột |
| 200–361 | `AddNpc_flyboss` (Phong Chi Kỳ) | **comment hết** — hệ Tín Sứ |
| 365–2606 | `AddNpc_allbugbear` (tiểu quái ải) | Tín Sứ |
| 2608–2932 | `AddNpc_turesurebug` | Tín Sứ (Thiên Bảo Khố) |
| 2936–2953 | `AddNpc_flypoint6079` | comment |
| 2956–2974 | `AddNpc_flypoint8089` | comment |
| 2977–2995 | `AddNpc_flypoint90` | comment |
| 2999–3067 | `AddNpc_templeboss` | Sơn Thần Miếu |
| 3071–3390 | `AddNpc_turesureboss` (9 Bảo Rương ×3 cấp) | Tín Sứ |
| 3393–3404 | `function add_bossnpc(Tab6)` | 10 cột, giống `add_killertasknpc` |
| 3406–3414 | `function add_messengernpc(Tab4)` | 9 cột, **không nhân 32**, không `SetNpcParam` |
| 3416–3421 | 5 hằng `DescLink_*` | Ảnh hộp thoại |

> **Chỉ đoạn 4–180 thuộc hệ Săn Boss Sát Thủ.** Phần còn lại (≈ 3240 dòng, ~95 % tệp) là hệ **Tín Sứ /
> Vượt ải** dùng chung tệp head. `addtollgatenpc.lua:16–19` chỉ gọi `AddNpc_turesureboss`,
> `AddNpc_turesurebug`, `AddNpc_allbugbear`.

### 4.2 Ý nghĩa TỪNG CỘT của `addkillertasknpc` (suy ra từ `add_killertasknpc`, :183–194)

```lua
function add_killertasknpc(Tab3)                        -- killbosshead.lua:183
  for i = 1 , getn(Tab3) do
    Tid = SubWorldID2Idx(Tab3[i][3]);                   -- cột 3: MAP ID -> chỉ số SubWorld
    if (Tid >= 0 ) then                                 -- map chưa nạp -> BỎ QUA IM LẶNG
      TabValue4 = Tab3[i][4] * 32                       -- cột 4: X theo Ô -> nhân 32 = toạ độ MPS
      TabValue5 = Tab3[i][5] * 32                       -- cột 5: Y theo Ô
      newtasknpcindex = AddNpc(Tab3[i][1], Tab3[i][2], Tid,
                               TabValue4, TabValue5,
                               Tab3[i][6], Tab3[i][7], Tab3[i][8]);
      SetNpcScript(newtasknpcindex, Tab3[i][9]);        -- cột 9: script hành động
      SetNpcParam(newtasknpcindex, 1, Tab3[i][10]);     -- cột 10: CHỈ SỐ NHIỆM VỤ 1..160
    end;
  end;
end;
```

| Cột | Tên | Kiểu | Giá trị trong bảng | Ý nghĩa |
|---|---|---|---|---|
| 1 | `nNpcTemplateId` | số | 761–820 (59 id khác nhau; **thiếu 769** = NPC giao nhiệm vụ) | Chỉ số dòng trong `settings\npcs.txt` (dòng = id + 2) |
| 2 | `nLevel` | số | 25/35/45/55/65/75/85/95 | Cấp NPC (cấp nhiệm vụ + 5) |
| 3 | `nMapId` | số | 40 giá trị | Map ID → `SubWorldID2Idx` |
| 4 | `nX` (ô) | số | 919..2065 | ×32 → toạ độ MPS |
| 5 | `nY` (ô) | số | 2569..3642 | ×32 → toạ độ MPS |
| 6 | **tham số 6 của `AddNpc`** | số | **luôn = 0** | Bản Linux: **một cờ BYTE tại `KNpc+0x1824`** — tên `bNoRevive` là **suy đoán chưa chứng minh** [đã sửa theo phản biện] (mã chỉ cho thấy `setne`; ghi chú JX1 ở `ScriptFuns.cpp` lại ánh xạ cùng ô này sang **camp**). **Bản JX1: nSeries!** (xem 9.2) |
| 7 | `szName` | chuỗi | tên riêng boss | Ghi đè `Npc[].Name` |
| 8 | tham số 8 của `AddNpc` | số | **luôn = 1** | Bản Linux: cờ nạp thuộc tính từ mẫu + đặt `+0x181C = 3/2` |
| 9 | `szScript` | chuỗi | **luôn** `\script\task\tollgate\killer\kill_level.lua` | ActionScript |
| 10 | `nTaskIndex` | số | **1..160 liên tục, không trùng, không đứt** | Ghi vào `NpcParam[1]`, so với `GetTask(1082)` |

**Ngũ hành boss là NGẪU NHIÊN.** Đã dịch ngược `AddNpc` bản Linux (`jx_linux_y` **0x0811BB10**):
tại `0x0811BBDF` gọi hàm PLT `0x804b28c` (rand), `0x0811BC72–0x0811BC8E` chia lấy dư 5
(`imul edx, 0x66666667` → `/5`, `lea eax,[edx+edx*4]`, `sub edi, eax`) rồi đẩy kết quả vào **đối số 1**
của `AddNpcSet2` (`0x809fb10`), còn `nNpcIdxInfo = (nId<<16)|nLevel` vào đối số 2.
Đối chiếu `AddNpcEx` (`0x0811BF40`, `0x0811C04D→0x0811C0AB`) đọc **tham số 3** rồi đẩy đúng vào cùng ô
`[esp+4]` — nên `[esp+4]` chắc chắn là **nSeries**. ⇒ **`AddNpc` của bản Linux luôn gán `series = rand()%5`.**
Điều này khớp gameplay: `kill_level.lua:37` đọc `NPCINFO_GetSeries(nNpcIndex)` để phát Sát Thủ lệnh
**cùng ngũ hành với boss**, và nếu ngũ hành cố định thì không thể "5 lệnh khác hệ".

### 4.3 Thống kê 160 dòng

| Nhóm | Comment mốc | Dòng | Chỉ số | Cấp NPC | 5 bản đồ (4 boss/bản đồ) |
|---|---|---|---|---|---|
| 20 | :5 | 6–25 | 1–20 | 25 | 73 Phục Lưu động · 43 Kiếm Các Trung Nguyên · 71 Bạch Thủy động · 83 Hoả Lang động · 14 Mãnh Hổ động |
| 30 | :27 | 28–47 | 21–40 | 35 | 4 Kim Quang động · 22 Bạch Vân động · 77 Yến Tử động · 141 Dược Vương động tầng 1 · 193 Vũ Di sơn |
| 40 | :49 | 50–69 | 41–60 | 45 | 5 Kinh Hoàng động · 168 **Phụng Nhãn** động · 23 Thần Tiên động · 91 Mê cung **Kê** Quán động · 135 Kiếm Tinh Phong sơn động [đã sửa theo phản biện] |
| 50 | :71 | 72–91 | 61–80 | 55 | 12 **Địa đạo hậu viện Tín Tướng tự** · 24 Huyền Thủy động · 42 Thiên Tâm động · 66 Đẩy Đằng Đình hồ tầng 1 · 194 Ngọc Hoa động [đã sửa theo phản biện] |
| 60 | :93 | 94–113 | 81–100 | 65 | 164 Thiên Tầm tháp tầng 1 · 117 Tương Vân động tầng 2 · 56 Hoành Sơn Phái · 148 Tuyết Báo động tầng 4 · 196 Dương Giác động |
| 70 | :115 | 116–135 | 101–120 | 75 | 123 Lão Hổ động · 94 Linh Cốc động · 319 Lâm Du Quan · 72 Đại Từ động · 76 Sơn Bảo động |
| 80 | :137 | 138–157 | 121–140 | 85 | 201 Băng Hà động · 10 Nhạn Thạch động · 202 Phù Dung động · 181 Lưỡng Thủy động · 143 Dược Vương động tầng 3 |
| **90** | :159 | 160–179 | **141–160** | 95 | 93 Tiến Cúc động · 225 Sa Mạc 1 · 75 Khoả Lang động · 321 Trường Bạch sơn Nam · 340 Mạc Cao Quật |

*(Dòng 158 trống, dòng 159 là comment mốc `--90级杀手任务Boss`, dữ liệu nhóm 90 nằm ở dòng 160–179.)*

### 4.4 40 dòng mẫu (5 dòng đầu mỗi nhóm) — cột: chỉ số | npcId | cấp | map | X | Y | tên | mô tả trong `killer.txt`

| # | npcId | cấp | map | X | Y | Tên (killbosshead.lua) | BossInfo (killer.txt) |
|---|---|---|---|---|---|---|---|
| 1 | 761 | 25 | 73 | 1545 | 2947 | Trác Lãnh Cầm | trong Phục Lưu động (193,184) |
| 2 | 770 | 25 | 73 | 1418 | 3034 | Tiêu Thiên Ngạo | trong Phục Lưu động (177,189) |
| 3 | 771 | 25 | 73 | 1640 | 3088 | Mục Minh Kiệt | trong Phục Lưu động (205,193) |
| 4 | 772 | 25 | 73 | 1740 | 3044 | Tất Vưu Phóng | trong Phục Lưu động (217,190) |
| 5 | 761 | 25 | 43 | 1607 | 3038 | Lục Cửu U | trong Kiếm Các Trung Nguyên (200,189) |
| 21 | 762 | 35 | 4 | 1577 | 2996 | Đào Đoạt Thu | trong Kim Quang động (197,187) |
| 22 | 773 | 35 | 4 | 1755 | 3251 | Xà Bá Đông | trong Kim Quang động (219,203) |
| 23 | 774 | 35 | 4 | 1747 | 2973 | Lương Tư Nam | trong Kim Quang động (218,185) |
| 24 | 775 | 35 | 4 | 1539 | 2894 | Trác Thất Lang | trong Kim Quang động (192,180) |
| 25 | 776 | 35 | 22 | 1722 | 3187 | Kiều Đỉnh Thiên | trong Bạch Vân động (215,199) |
| 41 | 763 | 45 | 5 | 1616 | 3476 | Du Thán Giang | trong Kinh Hoàng động (202,217) |
| 42 | 777 | 45 | 5 | 1442 | 3379 | Hạ Hùng Phi | trong Kinh Hoàng động (180,211) |
| 43 | 778 | 45 | 5 | 1602 | 3284 | Tống Vũ Phong | trong Kinh Hoàng động (200,205) |
| 44 | 779 | 45 | 5 | 1757 | 3616 | Lộ Thượng Nhân | trong Kinh Hoàng động (219,226) |
| 45 | 780 | 45 | 168 | 1765 | 3222 | Nhậm Tông Hoành | trong Phụng Nhãn động (220,201) |
| 61 | 764 | 55 | 12 | 1796 | 3172 | Mạnh Đạp Hồng | trong Mật đạo Tín Tướng tự (224,198) |
| 62 | 782 | 55 | 12 | 1724 | 3113 | Ân  Tức Hận | trong Mật đạo Tín Tướng tự (215,194) |
| 63 | 783 | 55 | 12 | 1703 | 3205 | Hoắc Thanh Sương | trong Mật đạo Tín Tướng tự (212,200) |
| 64 | 784 | 55 | 12 | 1783 | 3272 | Miên Tư Đạo | trong Mật đạo Tín Tướng tự (222,204) |
| 65 | 785 | 55 | 24 | 1796 | 3238 | Tiếu Vọng Du | trong Hưởng Thủy động (224,202) |
| 81 | 765 | 65 | 164 | 1787 | 3131 | Uông  Thệ Thủy | trong tầng 1 Thiên Tầm tháp (223,195) |
| 82 | 788 | 65 | 164 | 1780 | 2938 | Yến Lưu  Sanh | trong tầng 1 Thiên Tầm tháp (222,183) |
| 83 | 789 | 65 | 164 | 1723 | 3083 | Tang Thương Hải | trong tầng 1 Thiên Tầm tháp (215,192) |
| 84 | 790 | 65 | 164 | 1571 | 3040 | Bồ Thập Tam | trong tầng 1 Thiên Tầm tháp (196,190) |
| 85 | 791 | 65 | 117 | 1669 | 3011 | Hậu  Khất Kiếm | trong tầng  2 Tường Vân động (208,188) |
| 101 | 766 | 75 | 123 | 1604 | 3210 | Hạng Lệnh Ti | trong Lão Hổ động (200,200) |
| 102 | 795 | 75 | 123 | 1706 | 3254 | Tào Nhân  Phủ | trong Lão Hổ động (213,203) |
| 103 | 796 | 75 | 123 | 1583 | 3375 | Trình Phúc Ba | trong Lão Hổ động (197,210) |
| 104 | 797 | 75 | 123 | 1554 | 3303 | Đặng An Khuê | trong Lão Hổ động (194,206) |
| 105 | 798 | 75 | 94 | 1670 | 3126 | ứng  Tiêu Phong | trong Linh Cốc động (208,195) |
| 121 | 767 | 85 | 201 | 1768 | 3200 | Diệp ức Anh | trong Băng Hà động (221,200) |
| 122 | 803 | 85 | 201 | 1649 | 3184 | Lạc Nhĩ Kim | trong Băng Hà động (206,199) |
| 123 | 804 | 85 | 201 | 1698 | 3320 | Mạc Nam Trúc | trong Băng Hà động (212,207) |
| 124 | 805 | 85 | 201 | 1839 | 3365 | Tần Tử Du | trong Băng Hà động (229,210) |
| 125 | 806 | 85 | 10 | 1750 | 2997 | Cô Vô Thường | trong Nhạn Thạch động (218,187) |
| 141 | 768 | 95 | 93 | 1644 | 3279 | Giả Giới Nhân | trong Tiến Cúc động (205,204) |
| 142 | 812 | 95 | 93 | 1646 | 3058 | Trịnh Cửu Nhật | trong Tiến Cúc động (205,191) |
| 143 | 813 | 95 | 93 | 1736 | 3213 | Chu Sở Bá | trong Tiến Cúc động (217,200) |
| 144 | 814 | 95 | 93 | 1610 | 3152 | Trang Minh Trung | trong Tiến Cúc động (201,197) |
| 145 | 815 | 95 | 225 | 1590 | 3325 | Cam Chính Cô | trong Sa Mạc sơn  động (198,207) |

> ⚠️ ~~**Tên boss trong `killbosshead.lua` và trong `killer.txt` KHÔNG luôn giống nhau về chính tả**~~ **SAI [đã sửa theo phản biện 2]: đã đối chiếu MÁY cả 160 dòng — tên trùng NGUYÊN VĂN 160/160 (kể cả khoảng trắng thừa). Cái lệch thật nằm ở cột `BossInfo`: **3 dòng sai toạ độ** (#17 ghi (224,220) đúng phải (224,200); #19 ghi (203,196) đúng (203,198); #95 ghi (195,207) đúng (199,207)).**
> (bảng trên lấy cột "Tên" từ `killbosshead.lua`, cột "BossInfo" từ `killer.txt`).
> Tên hiển thị trên đầu NPC = `killbosshead.lua`; tên trong menu NPC Nhiếp Thí Trần = `killer.txt`.
> Tên bản đồ trong `BossInfo` cũng lệch với `maplist.ini` [đã sửa theo phản biện]: map 12 `maplist.ini` = **"Địa đạo hậu viện Tín Tướng tự"** (KHÔNG phải "Đao Đao học viện Tín Tường tự"), `killer.txt` = "Mật đạo Tín Tướng tự". Map 168 **KHÔNG lệch**: cả `maplist.ini` lẫn `killer.txt` đều ghi "Phụng Nhãn động". Map 91 `maplist.ini` = "Mê cung **Kê** Quán động".

### 4.5 NHÓM 90 — 20 dòng ĐẦY ĐỦ (nhóm DUY NHẤT còn chơi được)

| # | dòng | npcId | map | X | Y | Tên |
|---|---|---|---|---|---|---|
| 141 | 160 | 768 | 93 | 1644 | 3279 | Giả Giới Nhân |
| 142 | 161 | 812 | 93 | 1646 | 3058 | Trịnh Cửu Nhật |
| 143 | 162 | 813 | 93 | 1736 | 3213 | Chu Sở Bá |
| 144 | 163 | 814 | 93 | 1610 | 3152 | Trang Minh Trung |
| 145 | 164 | 815 | 225 | 1590 | 3325 | Cam Chính Cô |
| 146 | 165 | 816 | 225 | 1261 | 3247 | Vũ Nhất Thế |
| 147 | 166 | 817 | 225 | 1452 | 3377 | Dương Phong  Dật |
| 148 | 167 | 818 | 225 | 1425 | 3107 | Hà Sinh Vong |
| 149 | 168 | 819 | 75 | 1711 | 3187 | Tăng Chỉ óan |
| 150 | 169 | 820 | 75 | 1752 | 3124 | Vệ Biên Thành |
| 151 | 170 | 768 | 75 | 1831 | 3190 | Cố Thủ Đằng |
| 152 | 171 | 812 | 75 | 1639 | 3159 | Gia Cát Kinh Hồng |
| 153 | 172 | 813 | 321 | 1253 | 3002 | Phan Ngạt Nhạn |
| 154 | 173 | 814 | 321 | 1483 | 2742 | Liên Hình Thái |
| 155 | 174 | 815 | 321 | 1289 | 2613 | Bào Triệt sơn |
| 156 | 175 | 816 | 321 | 1113 | 2569 | Vạn Hồ Tinh |
| 157 | 176 | 817 | 340 | 1217 | 2740 | Trử Thiên Mẫn |
| 158 | 177 | 820 | 340 | 1723 | 2765 | Đoạn Lăng Nguyệt |
| 159 | 178 | 819 | 340 | 1275 | 2749 | Tả  Dật Minh |
| 160 | 179 | 818 | 340 | 1932 | 2759 | Nhậm Thương Khung |

### 4.6 NPC giao nhiệm vụ (`autoexec_npc.lua:26–32`, bảng `adddialognpc`, 6 cột)

Cột: `{npcId, mapId, X(ô), Y(ô), script, name}` — **KHÔNG có cột NpcParam**, hàm nạp là `add_dialognpc`.

| npcId | map | X | Y | Thành |
|---|---|---|---|---|
| 769 | 11 | 3210 | 4974 | Thành Đô |
| 769 | 1 | 1506 | 3198 | Phượng Tường |
| 769 | 37 | 1647 | 3050 | Biện Kinh |
| 769 | 176 | 1372 | 3010 | Lâm An |
| 769 | 162 | 1573 | 3227 | Đại Lý |
| 769 | 78 | 1512 | 3206 | Tương Dương |
| 769 | 80 | 1700 | 2963 | Dương Châu |

`npcs.txt` id 769 = "Nhiếp Thí Trần", `Kind=3` (NPC nói chuyện), `Camp=6`.
Ảnh hộp thoại: `DescLink_NieShiChen` = `\spr\npcres\enemy\enemy154\enemy154_pst.spr` (`newtask_head.lua:13`).

---

## 5. BẢNG VẬT PHẨM

### 5.1 Mọi lời gọi `AddItem` / `CalcItemCount` / `ConsumeItem` trong hệ

| Tệp:dòng | Lời gọi (đủ tham số) | Ý nghĩa |
|---|---|---|
| `lib_killlevel.lua:73` | `AddItem(6, 1, 399, 20, series, 0)` | Sát Thủ lệnh cấp 20, ngũ hành = của boss, `nLuck=0` |
| `lib_killlevel.lua:77` | `AddItem(6, 1, 399, 30, series, 0)` | cấp 30 |
| `lib_killlevel.lua:81` | `AddItem(6, 1, 399, 40, series, 0)` | cấp 40 |
| `lib_killlevel.lua:85` | `AddItem(6, 1, 399, 50, series, 0)` | cấp 50 |
| `lib_killlevel.lua:89` | `AddItem(6, 1, 399, 60, series, 0)` | cấp 60 |
| `lib_killlevel.lua:93` | `AddItem(6, 1, 399, 70, series, 0)` | cấp 70 |
| `lib_killlevel.lua:97` | `AddItem(6, 1, 399, 80, series, 0)` | cấp 80 |
| `lib_killlevel.lua:103` | `AddItem(6, 1, 399, 90, series, 0)` | cấp 90 |
| `nieshichen.lua:204` | `AddItem(6, 1, 400, level, attr[i], 0)` | **Sát Thủ Giản** — sản phẩm hợp thành 5 lệnh |
| `gmcommand_3.lua:464` | `AddItem(6, 1, 2346, 1, 0, 0)` | GM: "Thí Giả Yêu Bài" |
| `shashou_mibao.lua:16` | `CalcItemCount(3, 6, 1, 2348, -1)` | Đếm Huyền Thiên Chùy: `nPos=3` (hành trang), `nLevel=-1` (mọi cấp) |
| `shashou_mibao.lua:21` | `ConsumeItem(3, 6, 6, 1, 2348, -1)` | Trừ **6** Huyền Thiên Chùy |
| `shashou_mibao.lua:21` | `CalcFreeItemCellCount() >= 3` | Cần ≥ 3 ô trống |
| `npcNhiepThiTran.lua:54` | `CalcEquiproomItemCount(6, 1, 400, j)`, j = 20..90 bước 10 | Đếm Sát Thủ Giản (điều kiện **Vượt Ải**) |

**Chữ ký `AddItem` 6 tham số** = `(nGenre, nDetailType, nParticularType, nLevel, nSeries, nLuck)`.
`series` truyền vào là **ngũ hành THẬT của boss** lấy từ `NPCINFO_GetSeries` (`kill_level.lua:37`).

### 5.2 Bảng phần thưởng dạng bảng (qua `tbAwardTemplet`)

**a) `lib_killlevel.lua:105` — rơi khi hoàn thành cấp 90**
`tbAwardTemplet:GiveAwardByList({{szName="Sát Thủ Bí Bảo", tbProp={6,1,2347,1,1,0}, nRate=50}}, "Get Sát Thủ Bí Bảo", 1)`
→ **50 %** ra 1 Sát Thủ Bí Bảo (cấp 1, ngũ hành 1).

**b) `mibao_head.lua` — nội dung hộp Sát Thủ Bí Bảo (11 dòng)**

| # | Tên | tbProp | nRate (%) | Ghi chú |
|---|---|---|---|---|
| 1 | Tiên Thảo Lộ | 6,1,71,1,1,0 | 15.873 | |
| 2 | Tử Mãng Lệnh | 6,1,2350,1,1,0 | 0.002 | |
| 3 | Huyền Viên Lệnh | 6,1,2351,1,1,0 | 0.005 | |
| 4 | Thương Lang Lệnh | 6,1,2352,1,1,0 | 0.02 | |
| 5 | Vân Lộc Lệnh | 6,1,2353,1,1,0 | 1 | |
| 6 | Đại Lực hoàn | 6,**0**,3,1,1,0 | 25 | DetailType = 0 |
| 7 | Phi Tốc hoàn | 6,**0**,6,1,1,0 | 25 | DetailType = 0 |
| 8 | Khiêu chiến lễ bao | 6,1,2006,1,1,0 | 20 | |
| 9 | Quả Huy Hoàng (cao) | 6,1,906,1,1,0 | 10 | `nExpiredTime = 10080` phút = 7 ngày |
| 10 | Quả Hoàng Kim | 6,1,907,1,1,0 | 0.1 | `nExpiredTime = 10080` |
| 11 | Cẩm nang thay đổi trời đất | 6,1,1781,1,1,0 | 3 | `tbParam = {60}` |

**c) `kill_level.lua:89–118` — `OnFinishKillerTask`, nhóm `[1]` (25 dòng random) + `[2]` cố định**

| Tên | tbProp | nRate | Ghi chú |
|---|---|---|---|
| Tinh Tinh Khoáng | 6,1,3811,1,0,0 | 1.04 | |
| Tinh Thiết Khoáng | 6,1,3810,1,0,0 | 0.52 | |
| Chiếu Dạ Ngọc Sư Tử | 0,10,5,5,0,0 | 0.0021 | Ngựa |
| Xích Thố | 0,10,5,2,0,0 | 0.0021 | |
| Tuyệt ảnh | 0,10,5,3,0,0 | 0.0021 | |
| Đích Lô | 0,10,5,4,0,0 | 0.0021 | |
| Ô Vân Đạp Tuyết | 0,10,5,1,0,0 | 0.0021 | |
| Phi Vân | 0,10,8,1,0,0 | 0.0007 | |
| Bôn Tiêu | 0,10,6,1,0,0 | 0.0007 | |
| Phiên Vũ | 0,10,7,1,0,0 | 0.0007 | |
| Phong Vân Bạch Mã | 0,10,19,1,0,0 | 0.0007 | `nExpiredTime = 43200` (30 ngày) |
| Phong Vân Chiến Mã | 0,10,20,1,0,0 | 0.0003 | `nExpiredTime = 43200` |
| Phong Vân Thần Mã | 0,10,21,1,0,0 | 0.0001 | `nExpiredTime = 43200` |
| Túi Dược Phẩm | 6,1,30557,1,0,0 | **68.2639** | Dòng chính (đa số) |
| Đại Lực hoàn | 6,0,3,1,0,0 | 15 | |
| Phi Tốc hoàn | 6,0,6,1,0,0 | 15 | |
| Đồ Phổ Đằng Long Khôi | 6,1,30528,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Hài | 6,1,30530,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Yêu Đái | 6,1,30531,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Hộ Uyển | 6,1,30532,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Hạng Liên | 6,1,30533,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Bội | 6,1,30534,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Thượng Giới Chỉ | 6,1,30535,1,0,0 | 0.0003 | |
| Đồ Phổ Đằng Long Hạ Giới Chỉ | 6,1,30536,1,0,0 | 0.0003 | |
| Đằng Long Thạch - Hạ | 6,1,30538,1,0,0 | 0.16 | |
| **`[2]` Điểm kinh nghiệm** | `nExp = 10 000 000` | 100 % | Cố định |

**d) `lib_killlevel.lua:118–120` — `jiefangri_award` (sự kiện đã hết hạn 23/05/2011)**
Huy Chương Chiến Công `6,1,2823` ×2 · Cây Bút `6,1,2825` ×1 · Phù Hiệu `6,1,2826` ×2,
`nExpiredTime = 20110523`, kèm `AddStatData(...)`.

**e) Rơi từ NPC (`ITEM_DropRateItem`)** — `kill_level.lua:69`, chỉ nhóm 90:
`ITEM_DropRateItem(nNpcIndex, 8, "\settings\droprate\boss\bosstask_lev90.ini", 0, 10, nseries)`
→ tệp ini có `[Main] Count=100, RandRange=1000000, MinItemLevel=10, MaxItemLevel=10` + 100 mục
`Genre/Detail/Particular/RandRate`. **7 tệp `bosstask_lev20..80.ini` vẫn có mặt nhưng đường gọi bị comment.**

### 5.3 Vật phẩm định danh của hệ (bản Linux, `settings\item\004\magicscript.txt`)

| tbProp | Dòng | Tên | Script gắn kèm |
|---|---|---|---|
| 6,1,**399** | 401 | **Sát Thủ lệnh** | `\script\item\killer_token.lua` (chỉ hiện ngũ hành trong mô tả) |
| 6,1,**400** | 402 | **Sát thủ giản** | `\script\item\killer_sword.lua` (đã hết hiệu lực: chặn sau `20051006080000`) |
| 6,1,**2346** | 2358 | Thí Giả Yêu Bài | — |
| 6,1,**2347** | 2359 | **Sát Thủ Bí Bảo** | `\script\task\tollgate\killer\shashou_mibao.lua` |
| 6,1,**2348** | 2360 | Huyền Thiên Chùy | — (nguyên liệu, sinh ra ở hệ khác) |

---

## 6. HÀM LUA DO SCRIPT ĐỊNH NGHĨA

### 6.1 Lõi (26 hàm)

| Hàm | Tệp:dòng | Tham số | Trả về | Mô tả |
|---|---|---|---|---|
| `OnDeath` | `kill_level.lua:22` | `nNpcIndex` | — | Điểm vào engine khi boss chết; lọc `1..160` rồi chuyển tiếp |
| `kill_level20` | `kill_level.lua:34` | `nNpcIndex` | — | Xác thực đúng boss, tăng 1217, cập nhật ladder, rơi đồ, bắn 3 sự kiện, gọi `SetMemberTask` |
| `killbossall` | `kill_level.lua:81` | — | — | Callback: hiệu ứng skill 541 + thông báo |
| `OnFinishKillerTask` | `kill_level.lua:86` | `TSKID_KILL` | — | Thưởng bảng lớn cho nhóm 90 (đăng ký sự kiện `OnFinish`) |
| `SetMemberTask` | `lib_killlevel.lua:21` | `myTaskNumber, myOrgValue, myTaskValue, fnCallback, series` | — | Phát thưởng cho cá nhân hoặc **toàn tổ đội** |
| `add_shashouling` | `lib_killlevel.lua:70` | `nvalue, series` | — | Bảng exp + Sát Thủ lệnh theo dải 20 |
| `jiefangri_award` | `lib_killlevel.lua:110` | — | — | Sự kiện Chiến Thắng 04/2011 (đã hết hạn) |
| `main` | `nieshichen.lua:56` | — | — | Menu NPC Nhiếp Thí Trần |
| `annealofkiller` | `nieshichen.lua:65` | — | — | Menu con "Thí luyện sát thủ" → Vượt Ải |
| `aboutchallenge` | `nieshichen.lua:69` | — | — | Hiện luật Vượt Ải |
| `killer90` | `nieshichen.lua:122` | — | `0` nếu hỏng | Nhận nhiệm vụ cấp 90 |
| `cancel` | `nieshichen.lua:130` | — | — | `SetTask(1082, 0)` |
| `havetask` | `nieshichen.lua:135` | — | `0/1` | Chặn nhận khi đang có nhiệm vụ |
| `showboss` | `nieshichen.lua:143` | `row` | — | 10 boss (row+1..row+10) + "Trang kế" |
| `showbossnext` | `nieshichen.lua:147` | `row` | — | 10 boss (row+11..row+20) + "Trang trước" |
| `givetask` | `nieshichen.lua:151` | `taskid` | — | `SetTask(1082, taskid)` + hiện vị trí boss |
| `compose` | `nieshichen.lua:156` | — | — | Mở giao diện `GiveItemUI` |
| `exchange_token` | `nieshichen.lua:160` | `ncount` | — | Kiểm 5 lệnh cùng cấp rồi tiêu huỷ |
| `givesword` | `nieshichen.lua:201` | `attr, level` | — | Sinh Sát Thủ Giản, ngũ hành ngẫu nhiên trong 5 |
| `no` | `nieshichen.lua:209` | — | — | Rỗng (đóng hộp thoại) |
| `killerCoundTakedTask` | `nieshichen.lua:213` | `nLowLevel, nHighLevel` | `0/1` | Kiểm cấp + trần 8 lần/ngày + reset theo ngày |
| `add_killertasknpc` | `killbosshead.lua:183` | `Tab3` (10 cột) | — | Sinh NPC, ×32 toạ độ, `SetNpcScript` + `SetNpcParam(1)` |
| `add_bossnpc` | `killbosshead.lua:3393` | `Tab6` (10 cột) | — | Giống hệt `add_killertasknpc` (dành cho boss Tín Sứ) |
| `add_messengernpc` | `killbosshead.lua:3406` | `Tab4` (9 cột) | — | **Không nhân 32**, không `SetNpcParam` |
| `add_alltollgatenpc` | `addtollgatenpc.lua:10` | — | — | Nạp 3 bảng NPC ải (Tín Sứ), **không** nạp 160 boss sát thủ |
| `main` | `shashou_mibao.lua:15` | `nItemIdx` | `1` nếu huỷ | Mở hộp Sát Thủ Bí Bảo |

*(`mibao_head.lua` chỉ có bảng dữ liệu `tbItem_Mibao`, không có hàm.)*

### 6.2 Hàm móc nối ở tệp khác (13 hàm)

| Hàm | Tệp:dòng | Vai trò với hệ sát thủ |
|---|---|---|
| `NpcFunLib:CheckKillerdBoss(nTaskLevel, nNpcIndex)` | `activitysys\npcfunlib.lua:142` | Vị từ "xác này là boss sát thủ cấp N của tôi" — dùng ở **≥ 18 cấu hình activitysys** |
| `_Detail:OnMessage(tbParam)` (`FinishKillerBoss`) | `activitysys\detailtype\finishkillerboss.lua:5` | Nhận `G_ACTIVITY:OnMessage`, nhân bản cho cả tổ đội |
| `tongaward_killer()` | `tong\tong_award_head.lua:119` | `ContriValueEntryLogic(10 * COEF_CONTRIB_TO_VALUE, EVE_KILLER)` |
| `OnKillKillerBossDeath(nTaskId, nTaskValue)` | `global\mantlesystem\other_func_outputs.lua:39` | Rơi "tinh vũ nguyên thạch" 10 % (hệ áo choàng) |
| `OnKillKillerBoss(nTaskId, nTaskValue)` | `event\longmenbiaoju\gaojiweirenzhuang.lua:5` | Chỉ cấp 90: 0.2 % rơi `6,1,4202` |
| `tbBeidouActivity:OnFinishKillerBoss(nTaskId, TaskValue)` | `event\beidoulingpai\beidouactivity.lua:460` | Cấp 90 → phát Lệnh bài Bắc Đẩu |
| `tbChangeDestiny:completeMission_Killer(tbPlayers)` | `event\change_destiny\mission.lua:80` | Nhiệm vụ Đổi Mệnh: đủ **4 lần** cấp 90 |
| `FinishKillerTask` (lớp thành tựu) | `global\achievementsys\type\killertask.lua:7` | Thành tựu dựa trên `KillerBoss/OnFinish` |
| `tbCOT_Party:CheckCondition()` | `vng_feature\challengeoftime\npcNhiepThiTran.lua:3` | Kiểm điều kiện tổ đội Vượt Ải |
| `tbCOT_Party:CalcSword()` | `npcNhiepThiTran.lua:48` | Đếm Sát Thủ Giản `6,1,400` cấp 20..90 |
| `tbCommand3:KillBoss_TaskNpc / _TaskItem / _Transfer` | `gmscript\gmcommand_3.lua:458 / :462 / :468` | 3 lệnh GM |
| `pActivity:CheckCanCallKillerBoss()` | `activitysys\config\1035\extend.lua:36`, `1037\extend.lua:19` | Chống spam triệu hồi boss sát thủ sự kiện (3 / 5 phút) |
| `XepHangTop10MonPhai` (nhánh ladder 10119) | `global\pgaming\xephang\bangxephang.lua:80` | **⚠️ DÙNG NHẦM** ladder 10119 — xem 8.3 |

---

## 7. HÀM DO ENGINE CUNG CẤP (36 hàm)

`✔` = JX1 đã có · `✘` = JX1 **CHƯA CÓ** (phải viết)

| # | Hàm engine | Điểm gọi (tệp:dòng) | JX1 | Ghi chú port |
|---|---|---|---|---|
| 1 | `Include` | `kill_level.lua:10-16`, … | ✔ | |
| 2 | `IncludeLib` | `kill_level.lua:7,8,9` (`RELAYLADDER`, `ITEM`, `NPCINFO`) | ✔ một phần | `ScriptFuns.cpp:2480` — `RELAYLADDER`/`ITEM` có trong bảng (trỏ `noop.lua`); **`NPCINFO` KHÔNG có** → rơi vào nhánh "bỏ qua" (:2551) |
| 3 | `GetTask` | `lib_killlevel.lua:31,35,44,50,54`; `npcfunlib.lua:148` | ✔ | `ScriptFuns.cpp:14…` |
| 4 | `SetTask` | qua `nt_setTask` (`newtask_head.lua:32`) | ✔ | |
| 5 | `SyncTaskValue` | `newtask_head.lua:33` | ✔ | `ScriptFuns.cpp:14883` `LuaJX2_SyncTaskValue` |
| 6 | `GetNpcParam` | `kill_level.lua:36`; `npcfunlib.lua:149` | ✔ | `ScriptFuns.cpp:14509`; `MAX_NPCPARAM = 4` (`GameDataDef.h:421`) |
| 7 | `SetNpcParam` | `killbosshead.lua:191, 3401` | ✔ | `ScriptFuns.cpp:14507` |
| 8 | `GetNpcScript` | `npcfunlib.lua:144` | **✘** | Không có trong `ScriptFuns.cpp` |
| 9 | `SetNpcScript` | `killbosshead.lua:190, 3400, 3411` | ✔ | `ScriptFuns.cpp:14500` → `LuaSetNpcActionScript` |
| 10 | `NPCINFO_GetSeries` | `kill_level.lua:37` | **✘** | Linux `0x081C08E0`. JX1 có `GetNpcSeries` (`ScriptFuns.cpp:7195, 14504`) — **cùng ngữ nghĩa**, chỉ cần alias |
| 11 | `Ladder_NewLadder` | `kill_level.lua:44` | ✔ | `ScriptFuns.cpp:15085` |
| 12 | `GetName` | `kill_level.lua:44` | ✔ | |
| 13 | `ITEM_DropRateItem` | `kill_level.lua:69` | **✘** | Linux `0x08154DE0`. Chữ ký dùng: `(nNpcIdx, nCount, szIniPath, ?, ?, nSeries)` |
| 14 | `AddSkillState` | `kill_level.lua:82` | ✔ | `ScriptFuns.cpp:12784, 14349` — có nhánh `g_IsJx2Script` ép `nIfMagic = 1` |
| 15 | `Msg2Player` | `kill_level.lua:83`; `lib_killlevel.lua:74…104`; `shashou_mibao.lua:18,25` | ✔ | |
| 16 | `GetTeamSize` | `lib_killlevel.lua:24` | ✔ | `ScriptFuns.cpp:14365` |
| 17 | `GetTeamMember` | `lib_killlevel.lua:49` | ✔ | `ScriptFuns.cpp:14292` (alias `GetTeamMem`) |
| 18 | `DynamicExecuteByPlayer` | `lib_killlevel.lua:29` | ✔ | `ScriptFuns.cpp:14308` |
| 19 | `AddOwnExp` | `lib_killlevel.lua:72,76,80,84,88,92,96,100` | ✔ | `ScriptFuns.cpp:14617` |
| 20 | `AddItem` | `lib_killlevel.lua:73…103`; `nieshichen.lua:204`; `gmcommand_3.lua:464` | ✔ | |
| 21 | `GetLocalDate` | `lib_killlevel.lua:140`; `nieshichen.lua:222` | ✔ | `ScriptFuns.cpp` `LuaGetLocalDate` — dùng `strftime`, hỗ trợ `%y%m%d` |
| 22 | `AddStatData` | `lib_killlevel.lua:145`; `shashou_mibao.lua:23` | **✘** | Linux `0x080FF550`. Chỉ để thống kê — có thể stub |
| 23 | `Describe` | `nieshichen.lua:62,66,70,132,137,144,148,153,171,182,186,190,206,219,225` | ✔ | |
| 24 | `GiveItemUI` | `nieshichen.lua:157` | ✔ | `ScriptFuns.cpp:15214` |
| 25 | `GetGiveItemUnit` | `nieshichen.lua:166` | ✔ | `ScriptFuns.cpp:15215` |
| 26 | `GetItemProp` | `nieshichen.lua:167` | ✔ | JX1 trả **6** giá trị `(g,d,p,lv,series,luck)` (`KJx2WarInfra.cpp:769`, `return 6`); script chỉ nhận 5 — Lua bỏ giá trị thừa, không sao [đã sửa theo phản biện] |
| 27 | `RemoveItemByIndex` | `nieshichen.lua:195` | ✔ | `ScriptFuns.cpp:15216` |
| 28 | `GetLevel` | `nieshichen.lua:217` | ✔ | |
| 29 | `SubWorldID2Idx` | `killbosshead.lua:185, 3395, 3408` | ✔ | `ScriptFuns.cpp:14477` |
| 30 | `AddNpc` | `killbosshead.lua:189, 3399, 3410` | ✔ **nhưng KHÁC NGỮ NGHĨA** | Xem 9.2 — **rủi ro #1** |
| 31 | `CalcItemCount` | `shashou_mibao.lua:16` | ✔ | `ScriptFuns.cpp:14911` `LuaWllsCalcItemCount` — đã có dạng JX2 `(nPos, g, d, p[, lv])` |
| 32 | `CalcFreeItemCellCount` | `shashou_mibao.lua:21` | ✔ | `ScriptFuns.cpp:14431` |
| 33 | `ConsumeItem` | `shashou_mibao.lua:21` | ✔ | `ScriptFuns.cpp:14384` — đã phân nhánh dạng JX2 `(nPos, nCount, g, d, p[, lv])` |
| 34 | `TabFile_Load` / `TabFile_GetCell` | qua `KTabFile` (`class\ktabfile.lua`, `nieshichen.lua:54`) | ✔ | `ScriptFuns.cpp:15284` / `:15285` [đã sửa theo phản biện 2] (15283 là `GetLocalDateEx`) |
| 35 | `Add120SkillExp` | `task_award_extend.lua:6` ← `lib_killlevel.lua:102` | **✘** | **Rủi ro #4** — xem 9.4 |
| 36 | `random` / `format` / `getn` / `tinsert` / `unpack` / `tonumber` / `ceil` | rải rác | ✔ | Thư viện chuẩn Lua 4.0 |

**Vành ngoài (do các tệp móc nối gọi, không trực tiếp trong 7 tệp hạt giống):**
`CallPlayerFunction`, `GetNpcIdx`, `GetNpcSettingIdx`, `CalcEquiproomItemCount`, `ConsumeEquiproomItem`,
`ST_GetTransLifeCount`, `GetFightState`, `GetCurServerTime`, `GetGlbValue`, `NewWorld`,
`LoadMap` (**✘ JX1 chưa có**), `CreateNewSayEx` (**✘ JX1 chưa có** — chỉ dùng ở lệnh GM),
`TONG_*` (cống hiến bang hội) — tất cả đã có trong JX1 trừ 2 mục đánh dấu.

---

## 8. MÓC NỐI VỚI HỆ KHÁC

### 8.1 `activitysys` — `CheckKillerdBoss` + `FinishKillerBoss`

`NpcFunLib:CheckKillerdBoss(nTaskLevel, nNpcIndex)` (`npcfunlib.lua:142`):
```lua
szScript = GetNpcScript(nNpcIndex)
nếu szScript == "\script\task\tollgate\killer\kill_level.lua" thì
   Uworld1082 = GetTask(1082);  nNpcSign = GetNpcParam(nNpcIndex, 1)
   nếu 1<=Uworld1082<=160 và Uworld1082==nNpcSign thì
      nếu (nTaskLevel-20)*2+1 <= Uworld1082 <= (nTaskLevel-20)*2+20 thì return 1
```
> Công thức `(N-20)*2+1 .. (N-20)*2+20` ánh xạ cấp nhiệm vụ → dải chỉ số: 20→1..20, 30→21..40, … 90→141..160.

**≥ 18 cấu hình dùng nó** (đều với `{90}`): `config\1002,1007,1008,1010,1022,1026,1064,1065,1069,25,18,11,40,4,22,37,31,33` (+ 5 bản comment).
Ngoài activitysys còn: `vng_event\ngusackettinh\script\nskt_dropitem.lua:10`,
`vng_event\lunaryear2011\npcdeath_award.lua:13`.

`G_ACTIVITY:OnMessage("FinishKillerBoss", PlayerIndex, 1082, Uworld1082, 0)` (`kill_level.lua:71`)
→ `activitysys\detailtype\finishkillerboss.lua` nhân bản cho toàn đội, được các cấu hình
`config\1024, 1035, 30, 35` khai báo qua `szMessageType = "FinishKillerBoss"`.

### 8.2 `EventSys` — kiểu `KillerBoss`

Khai báo: `misc\eventsys\type\func.lua:33–37`, hai khoá:
- `"OnFinish"` — mọi người trong đội hoàn thành
- `"OnKillBoss"` — riêng người ra đòn cuối

Danh sách đăng ký đã tìm được:

| Đăng ký | Khoá | Tệp:dòng |
|---|---|---|
| `OnFinishKillerTask` | OnFinish | `kill_level.lua:123` |
| `tbBeidouActivity.OnFinishKillerBoss` | OnFinish | `beidouactivity.lua:481` |
| `FinishKillerTask` (thành tựu) | OnFinish | `achievementsys\type\killertask.lua:9` |
| `pActivity.OnReceiveHoaTriAn` | OnFinish | `activitysys\config\1050\extend.lua:101` |
| `OnKillKillerBossDeath` | OnKillBoss | `mantlesystem\other_func_outputs.lua:175` |
| `OnKillKillerBoss` | OnKillBoss | `longmenbiaoju\gaojiweirenzhuang.lua:18` |

**🔴 `OnPlayerEvent` KHÔNG TỒN TẠI trong bản Linux này.** `script\misc\eventsys\eventsys.lua` chỉ có
`NewType / GetType / Reg / UnReg / GetProcParam / OnEvent` (kiểm hết 73 dòng). Grep toàn cây
`D:\ServerLinux\server1`: **0 định nghĩa**, **82 điểm gọi trên 60 tệp** (không phải ~40) [đã sửa theo phản biện]. Grep chuỗi trong ELF `jx_linux_y`:
`OnPlayerEvent` — **không thấy chuỗi**. ⇒ trong bản Linux này, `kill_level.lua:72` và `:74`
sẽ ném lỗi Lua "attempt to call a nil value" **TRƯỚC** dòng `:75` `SetMemberTask(...)`.
Đây là **lỗi thật của bản gốc** (hoặc `eventsys.lua` bị lệch phiên bản so với các điểm gọi).

**Tin tốt:** cây JX1 đang chạy **đã có** `EventSys:OnPlayerEvent` — bổ sung ở đợt WLLS 20/08
(`E:\...\bin\server\script\misc\eventsys\eventsys.lua:79–106`, chú thích ghi rõ "bản Linux do ENGINE
bơm vào… 0 định nghĩa Lua, ~40 call site"). Ngữ nghĩa: đổi `PlayerIndex` rồi phát cho mọi handler đã `Reg`.
⇒ **Khi port, giữ nguyên `:72`/`:74` là ĐÚNG** trên cây JX1, nhưng phải hiểu là bản gốc Linux vốn hỏng.

### 8.3 Bảng xếp hạng — ladder `10119`

- `kill_level.lua:44` `Ladder_NewLadder(10119, GetName(), Uworld1217+1, 1)` — điểm = **tổng số boss đã săn**.
- `global\pgaming\xephang\bangxephang.lua:27` `Ladder_ClearLadder(10119) -- săn boss sát thủ` (đúng).
- **🔴 XUNG ĐỘT:** `bangxephang.lua:80` — trong `XepHangTop10MonPhai`, nhánh "không môn phái, không bang hội,
  camp == 4" lại ghi `Ladder_NewLadder(10119, Player:GetName(), Player:GetLevel(), 1)` → **ghi CẤP nhân vật
  vào bảng săn boss sát thủ**, phá dữ liệu. Đây là bug của bản Việt hoá, **không** có ở logic gốc.
- `missions\challengeoftime\npc.lua:125, :230` dùng `{func_ladder_getname, 10119}` để đặt tên NPC
  "Tiểu Nhiếp Thí Trần" theo người đứng đầu bảng — **liên kết chéo sang hệ Vượt Ải**.

### 8.4 `huoyuedu` (điểm hoạt động)

`lib_killlevel.lua:29` `DynamicExecuteByPlayer(PlayerIndex, "\script\huoyuedu\huoyuedu.lua", "tbHuoYueDu:AddHuoYueDu", "shashourenwu")`
→ `huoyuedu.lua:26` `["shashourenwu"] = 11` → dòng 12 của `settings\huoyuedu\huoyuedu.txt`:
`ActivityId=11, ActivityName=杀手任务, CountTask=2871, MaxCount=2, Param1=**3**, Param2=3` ⇒ **+3 điểm lần 1, +3 lần 2, tối đa 2 lần/ngày**. [đã sửa theo phản biện] (dòng 12 thực tế là `11	杀手任务	2871	2	3	3`; dòng có Param1=5 là dòng 11 `10 信使任务 2870 2 5 2`).
**Đây là phụ thuộc ĐỘNG (chuỗi đường dẫn), KHÔNG nằm trong bao đóng Include.**

### 8.5 Bang hội (`tong`)

`lib_killlevel.lua:37, :46` → `tongaward_killer()` (`tong_award_head.lua:119`) →
`ContriValueEntryLogic(10 * COEF_CONTRIB_TO_VALUE, EVE_KILLER)` (`tong\contribution_entry.lua`).
Có nhân đôi khi còn hiệu lực "Võ Thần Tứ Phúc" (`TASKID_TONG_WUSHENBLESS = 2509`) — nhưng
`tongaward_killer` **không** gọi `extong_double_award`, khác với `tongaward_battle`/`tongaward_message`.

### 8.6 Vượt Ải (`challengeoftime`) — liên kết hai chiều

- NPC Nhiếp Thí Trần là **cửa vào chung**: `nieshichen.lua:19` `Include(vng_feature\challengeoftime\npcNhiepThiTran.lua)`,
  `:5` `Include(missions\challengeoftime\npc\dragonboat_main.lua)`, `:11` `Include(missions\challengeoftime\rank_perday.lua)`.
- **Vé vào Vượt Ải là Sát Thủ Giản `6,1,400`** — sản phẩm của hệ sát thủ
  (`npcNhiepThiTran.lua:54`, `dragonboat_main.lua:29,59,65,136,142`): sơ cấp cần 2 giản < 90, cao cấp cần 2 giản cấp 90.
- Ngược lại, `rank_award()` / `get_top5team()` của Vượt Ải nằm ngay trong menu NPC này.

### 8.7 Các hệ khác

| Hệ | Điểm nối |
|---|---|
| Áo choàng (mantle) | `other_func_outputs.lua:39` — 10 % rơi "tinh vũ nguyên thạch" |
| Long Môn tiêu cục | `gaojiweirenzhuang.lua:5` — 0.2 % rơi `6,1,4202` |
| Bắc Đẩu lệnh bài | `beidouactivity.lua:460` |
| Thành tựu | `achievementsys\type\killertask.lua` |
| Đổi Mệnh | `change_destiny\mission.lua:80` — 4 lần cấp 90 |
| Chuyển sinh | `metempsychosis\task_func.lua:166` — chặn khi `GetTask(1082) ~= 0` |
| Ngũ Sắc Kết Tinh / Tết 2011 | `nskt_dropitem.lua:10`, `lunaryear2011\npcdeath_award.lua:13` |

---

## 9. PHỤ THUỘC DỮ LIỆU NGOÀI & RỦI RO PORT

### 9.0 KẾT LUẬN VỀ `settings\killer.ini` — **KHÔNG THUỘC HỆ NÀY**

Toàn bộ nội dung `D:\ServerLinux\server1\settings\killer.ini`:
```
;通缉追杀系统专用ini        <- "hệ TRUY NÃ / TRUY SÁT NGƯỜI CHƠI"
[Main]
MoneyPerHour=10000          ; tiền mỗi giờ treo lệnh
MinTargetLevel=50           ; mục tiêu dưới cấp này không truy nã được
MaxActiveTaskTime=10        ; số giờ tối đa một lệnh truy nã còn hiệu lực
MinReward=100000            ; tiền thưởng tối thiểu
[Messages]
TargetLevelTooLow=Đối tượng dưới cấp 50 không thể truy nã!
TargetAbsent=Nhân vật mục tiêu không trực tuyến, không thể ra lệnh truy nã!
```
Đây là hệ **treo thưởng truy sát NGƯỜI CHƠI khác** (PK bounty), gắn với Relay server
("bảo trì đồng nhất với Relay"). **Không một tệp .lua nào của hệ Săn Boss Sát Thủ đọc tệp này**
(grep toàn cây script: 0 kết quả). Tệp cấu hình đúng của hệ Săn Boss Sát Thủ là
`settings\task\tollgate\killer\killer.txt`. **Trùng tên "killer" là ngẫu nhiên.**

### 9.1 Bảng phụ thuộc dữ liệu

| Tài nguyên | Đường dẫn | JX1 đang chạy | Ghi chú |
|---|---|---|---|
| Bảng tên/mô tả boss | `settings\task\tollgate\killer\killer.txt` — 3 cột `Id / BossName / BossInfo`, **160 dòng dữ liệu** (1..160), mã hoá **TCVN3** | **CHƯA CÓ** | Đọc bằng `KTabFile` (`nieshichen.lua:54`). **Toạ độ trong `BossInfo` = `(floor(X/8), floor(Y/16))`** với X,Y là cột 4/5 của `addkillertasknpc` — đã kiểm **toàn bộ 160/160 dòng bằng máy [đã sửa theo phản biện 2]: khớp 157, LỆCH 3** (#17, #19, #95 — lỗi dữ liệu của `killer.txt`, không phải lỗi công thức). Tức là toạ độ hiển thị = MPS/256 và MPS/512 |
| Mẫu NPC | `settings\npcs.txt` id **761–820** + **769** | **CÓ DÒNG nhưng KHÔNG TRÙNG KHỚP** [đã sửa theo phản biện] — JX1 chỉ **87 cột**, Linux **103 cột**; JX1 **thiếu hẳn 16 cột**, trong đó có **`PasstSkillId` / `PasstSkillLevel` / `AuraSkillId` / `DropRateFile`**. Cả **60/60 id đều lệch**: 5 cột `*ResistMax` **85 (Linux) → 25 (JX1)**, 10 id lệch `LifeParam3`. Cột chung khớp: `LevelScript = \script\npclevelscript\task_killboss.lua`, `ReviveFrame = 16200` (**CÙNG vị trí ở cả hai** [đã sửa theo phản biện 2]: chỉ số 85 đếm-từ-0 = cột 86 đếm-từ-1; không có chuyện lệch cột), `Kind=0/Camp=5/Skill1..4=53` | ⚠️ **Bị động 547 KHÔNG gắn được qua `npcs.txt` của JX1** (không có cột `PasstSkillId`) — phải thêm cột hoặc gắn skill bằng đường khác |
| Script cấp NPC | `script\npclevelscript\task_killboss.lua` | **CÓ** (6687 B, bản JX1 khác 6331 B của Linux) | Cần đối chiếu nếu muốn chỉ số giống hệt |
| Kỹ năng | `settings\skills.txt` id **541** (hiệu ứng hoàn thành), **547** (miễn dịch boss), **548** | **CÓ ĐỦ 3** (tra theo cột `SkillId`) | 547 trỏ `\script\skill\npc\killerbossmianyi.lua` |
| Script skill 547 | `script\skill\npc\killerbossmianyi.lua` | **JX1 ĐÃ CÓ** (**1004 B** [đã sửa theo phản biện 2] — cả hai bên đều 1004 B, không phải 956 B) nhưng **LỆCH 1 GIÁ TRỊ** [đã sửa theo phản biện] | Đã kiểm đủ 5 thuộc tính: `fasthitrecover_v=100`, `fatallystrikeres_p=99`, `freezetimereduce_p=200`, `poisontimereduce_p=`**180 (Linux) / 280 (JX1)**, `stuntimereduce_p=200` |
| Bảng rơi đồ | `settings\droprate\boss\bosstask_lev20..90.ini` (8 tệp) | **CHỈ CÓ 1 tệp khác** (`helianpiaodroprate.ini`) → **THIẾU HẾT** | Chỉ `lev90.ini` thực sự được gọi |
| Bảng vật phẩm | `settings\item\004\magicscript.txt` (4997 dòng) | Có `magicscript.txt` 4867 dòng nhưng **ID LỆCH** | **RỦI RO #2** |
| Điểm hoạt động | `settings\huoyuedu\huoyuedu.txt` dòng 12 | **CHƯA CÓ** thư mục `huoyuedu` | |
| Bản đồ | 40 map boss + 7 map NPC | **CÓ ĐỦ 47/47** (đã đối chiếu `<id>_name=` của `maplist.ini` ↔ `MapList.ini`), **5** chỗ lệch tên: map 14, 83 ("Hoả/Khoả"), 91 ("Kê/Kế"), 12 ("tự/Tự"), 162 ("Đại Lý phủ / Đại Lý") [đã sửa theo phản biện] | Nếu map chưa `LoadMap` thì `SubWorldID2Idx` trả −1 → `add_killertasknpc` **bỏ qua im lặng** |

### 9.2 🔴 RỦI RO #1 — `AddNpc` khác ngữ nghĩa tham số 6 & ngũ hành ngẫu nhiên

| | Bản Linux (`0x0811BB10`) | JX1 (`ScriptFuns.cpp:6814 LuaAddNpc`) |
|---|---|---|
| tham số 3/4/5 | SubWorldIdx / X / Y | **giống** |
| **tham số 6** | cờ BYTE → `setne [KNpc+0x1824]` (`0x0811BDB2`) — tên **`bNoRevive` ĐÃ XÁC MINH** [đã sửa theo phản biện 2]: chính dự án đã chốt ánh xạ ô này ở đợt PORT5 23/08, xem `Sources\Core\Src\ScriptFuns.cpp:6925–6930` (`if (g_IsJx2Script(L) && Lua_ValueToNumber(L,7) != 0) Npc[nNpcIdx].m_bNoRevive = 1;`) | **`nSeries`** — `ScriptFuns.cpp:6834` [đã sửa theo phản biện 2] (dòng 6833 là `nNpcIdxInfo = MAKELONG(nLevel,nId)`; hàm bắt đầu ở `:6814`) |
| **nSeries** | `rand() % 5` — tính tại `0x0811BBDF…0x0811BC8E` | lấy từ **tham số 6** |
| tham số 7 | tên NPC | tên NPC (**giống**) |
| tham số 8 | cờ nạp thuộc tính (`0x0811BEF2` → `0x8085250`, đặt `+0x181C = 3/2`) | **BỎ QUA** |

`killbosshead.lua` truyền `..., 0, "Tên", 1)` ⇒ chép nguyên xi sang JX1 sẽ cho **cả 160 boss ngũ hành Kim (0)**
thay vì ngẫu nhiên ⇒ Sát Thủ lệnh nhận được luôn cùng một hệ ⇒ **hỏng gameplay hợp thành 5 lệnh**
và hỏng luôn luật tương khắc ngũ hành của Sát Thủ Giản.
**Cách xử lý:** hoặc dùng `AddNpcEx` với `nSeries = random(0,4)`, hoặc thêm nhánh
"script JX2 gọi `AddNpc` ⇒ series ngẫu nhiên" trong `LuaAddNpc` (đã có tiền lệ `g_IsJx2Script`).

### 9.3 🔴 RỦI RO #2 — KHÔNG GIAN ID VẬT PHẨM LỆCH HOÀN TOÀN

Đã tra chéo bằng bộ 3 `(Genre, DetailType, ParticularType)`:

| tbProp | Linux (`item\004\magicscript.txt`) | JX1 (`item\magicscript.txt`) |
|---|---|---|
| **6,1,399** | **Sát Thủ lệnh** (dòng 401) | **Sát thủ giản** (dòng 401) |
| **6,1,400** | **Sát thủ giản** (dòng 402) | **Sư đồ thiếp** (dòng 402, script `\script\item\card\card_shitu.lua`) [đã sửa theo phản biện] |
| 6,1,398 | Thần bí khoáng thạch | **Sát Thủ lệnh** |
| **6,1,2347** | **Sát Thủ Bí Bảo** | **Khấp Địa Quần** lễ hộp [đã sửa theo phản biện] |
| **6,1,2348** | Huyền Thiên Chùy | Sát Thủ Giản lễ hộp |
| 6,1,2346 | Thí Giả Yêu Bài | Kinh Thiên Giáp lễ hộp |
| 6,1,2355 / 2356 | — | **Thí Giả Yêu Bài** / **Sát Thủ Bí Bảo** (script `\script\item\satthubibao.lua`) |
| 6,1,2350–2353 | Tử Mãng/Huyền Viên/Thương Lang/Vân Lộc Lệnh | Hộp bánh chưng / Hồng Bao Đoan Ngọ / … |
| 6,0,3 · 6,0,6 | Đại Lực hoàn · Phi Tốc hoàn | **KHÔNG CÓ** |
| 6,1,30557 · 30528–30538 | Túi Dược Phẩm · Đồ Phổ Đằng Long | **KHÔNG CÓ** |
| 6,1,2823/2825/2826 | Huy Chương/Cây Bút/Phù Hiệu | 越南自定义道具154/156/104 |
| 6,1,3810/3811 | Tinh Thiết/Tinh Tinh Khoáng | 稽查令 / 手中线 |

Thêm nữa, JX1 **đã có sẵn** Sát thủ lệnh/giản ở **genre 2** (`item\mine.txt` dòng 401/402:
`2,1,399` và `2,1,400`, script `killer_token.lua` / `killer_sword.lua`).
⇒ **Bắt buộc phải viết bảng ánh xạ item và sửa TỪNG lời gọi** — không được chép Lua nguyên văn.

### 9.4 🔴 RỦI RO #3 — 5 hàm engine thiếu, 2 trong số đó nằm giữa đường thưởng

| Hàm | Điểm gọi | Hậu quả nếu thiếu |
|---|---|---|
| `Add120SkillExp` | `task_award_extend.lua:6` ← `lib_killlevel.lua:102` | **Nghiêm trọng**: `AddExp_Skill_Extend(140000)` chạy **trước** `AddItem(...399, 90 ...)` ở dòng 103 ⇒ lỗi Lua ⇒ **người chơi cấp 90 KHÔNG nhận được Sát Thủ lệnh, không nhận Bí Bảo, task 1082 không bị xoá**. Lưu ý `task_award_extend.lua` của JX1 **giống hệt** bản Linux (đã diff) nhưng JX1 **không có** `Add120SkillExp` (grep `ScriptFuns.cpp` = 0). |
| `NPCINFO_GetSeries` | `kill_level.lua:37` | Lỗi ngay dòng 37 ⇒ toàn bộ `kill_level20` chết. JX1 có `GetNpcSeries` cùng ngữ nghĩa ⇒ chỉ cần **thêm alias** |
| `GetNpcScript` | `npcfunlib.lua:144` | Mất toàn bộ ≥ 18 móc `CheckKillerdBoss` của activitysys |
| `ITEM_DropRateItem` | `kill_level.lua:69` | Mất đường rơi đồ theo ini của nhóm 90 |
| `AddStatData` | `lib_killlevel.lua:145`, `shashou_mibao.lua:23` | Chỉ mất thống kê — stub được |

### 9.5 🔴 RỦI RO #4 — `killbosshead.lua` ĐÃ nằm trong JX1 nhưng **không** gọi bảng sát thủ

`E:\...\bin\server\script\task\tollgate\killbosshead.lua` và `addtollgatenpc.lua` **giống byte-for-byte**
(sau khi bỏ CRLF) với bản Linux — do đợt port **Tín Sứ 21/08**.
Nhưng `tinsu_addnpc.lua:45` chỉ gọi `add_alltollgatenpc()` (= Bảo Rương + Bảo Khố),
**không** gọi `add_killertasknpc(addkillertasknpc)`.
⇒ Bảng 160 boss **đã có sẵn trong bộ nhớ**, chỉ thiếu **một dòng gọi** + các tệp `killer\*.lua`.
⚠️ Nếu bật lên mà chưa xử lý 9.2/9.3 thì 160 NPC sẽ sinh ra và phát **sai vật phẩm**.

### 9.6 🔴 RỦI RO #5 — Trùng/lệch biến nhiệm vụ và mã hoá tệp

- **Task 1082 / 1192 / 1193**: JX1 **đã khai báo sẵn** ở `newtask_head.lua:14–16` (giống hệt Linux) và
  `metempsychosis\task_head.lua:84` (`TSK_KILLER_ID = 1082`) — 1082 **không xung đột**, nhưng **1192/1193 CÓ XUNG ĐỘT** [đã sửa theo phản biện 2]: `global\general\lenhbaitanthu.lua:152,156,157,159` và `global\mel\baivip.lua:148,152,153,155` (hai vật phẩm bản Việt hoá) cũng `nt_getTask/nt_setTask(1192)` và `(1193)` — **chúng TỰ làm bước reset theo ngày** rồi hiển thị `nBossST/SoLuongBossSatThuTrongNgay` (`global\pgaming\configserver\configall.lua:36` = **100**, lệch với `KILLER_MAXCOUNT = 8`). Port phải mang theo hoặc chỉnh cả hai tệp này.
  **Task 1217** hiện chưa ai dùng trong cây JX1 (chỉ có ở `codenew.lua` là bảng mã chung) ⇒ an toàn.
- **Mã hoá**: `killer.txt` là **TCVN3**, `killbosshead.lua`/`lib_killlevel.lua`/`kill_level.lua` là
  **GBK trộn TCVN3 theo dòng**. Dùng `Edit`/`Write` thẳng sẽ **hỏng byte** — bắt buộc qua
  `gbktool.py` / skill `swordonline-dev`.
- **`IncludeLib("NPCINFO")`** không có trong bảng 21 module của `LuaIncludeLib`
  (`ScriptFuns.cpp:2486–2489`) ⇒ chỉ ghi log "bỏ qua", không lỗi — nhưng cũng không nạp gì.

### 9.7 Danh sách tệp cần port (đối chiếu cây JX1 đang chạy, bao đóng 67 tệp)

**[A] JX1 CHƯA CÓ — 28 tệp**
```
class\ktabfile.lua                       class\lerror.lua
event\birthday_jieri\200905\chuangguan\chuangguan.lua
event\change_destiny\head.lua            event\change_destiny\mission.lua
item\battles\songjinskill.lua            lib\coordinate.lua
lib\droptemplet.lua                      lib\file.lua
lib\mem.lua                              lib\sharedata.lua
lib\string.lua                           missions\boss\bigboss.lua
missions\challengeoftime\include.lua     missions\challengeoftime\npc\dragonboat_main.lua
missions\challengeoftime\rank_perday.lua task\metempsychosis\task_func.lua
task\tollgate\killer\kill_level.lua      task\tollgate\killer\lib_killlevel.lua
task\tollgate\killer\mibao_head.lua      task\tollgate\killer\nieshichen.lua
task\tollgate\killer\shashou_mibao.lua   tong\contribution_entry.lua
tong\log.lua                             tong\tong_award_head.lua
tong\tong_header.lua                     tong\tong_setting.lua
vng_feature\challengeoftime\npcNhiepThiTran.lua
```
**[B] CÓ nhưng NỘI DUNG KHÁC — 3 tệp** (phải hợp nhất thủ công, KHÔNG đè):
`lib\awardtemplet.lua` · `misc\eventsys\eventsys.lua` (JX1 **đã thêm** `OnPlayerEvent`) · `task\newtask\newtask_head.lua`

**[C] CÓ và GIỐNG HỆT — 36 tệp** (gồm `task\tollgate\killbosshead.lua`, `addtollgatenpc.lua`,
`activitysys\*`, `misc\eventsys\type\player.lua`, `vng_lib\*`, `task\task_award_extend.lua`, …)

**[D] Phụ thuộc ĐỘNG, KHÔNG có trong bao đóng Include** (phải nhớ port riêng):
`activitysys\npcfunlib.lua` (gọi qua chuỗi cấu hình) ·
`huoyuedu\huoyuedu.lua` + `settings\huoyuedu\huoyuedu.txt` (gọi qua `DynamicExecuteByPlayer`) ·
`global\achievementsys\type\killertask.lua` · `global\mantlesystem\other_func_outputs.lua` ·
`event\beidoulingpai\beidouactivity.lua` · `event\longmenbiaoju\gaojiweirenzhuang.lua` ·
`activitysys\detailtype\finishkillerboss.lua` · `global\pgaming\xephang\bangxephang.lua`

---

## 10. NHỮNG ĐIỂM CẦN LƯU Ý KHI THIẾT KẾ LẠI (không phải lỗi port, là lỗi/đặc tính gốc)

1. **Chỉ nhóm cấp 90 chơi được** (`nieshichen.lua:73–120` comment hết) trong khi 160 NPC vẫn spawn.
   Muốn mở lại cấp 20–80 phải bỏ comment 7 hàm + 7 nhánh `ITEM_DropRateItem` (`kill_level.lua:46–66`).
2. **Đồng đội không được điểm hoạt động, không lên bảng xếp hạng, không cộng cống hiến bang** —
   chỉ người ra đòn cuối (xem 2.5).
3. **`lib_killlevel.lua:27`** khai báo `Uworld1217` rồi không dùng — mã chết.
4. **`jiefangri_award`** (`lib_killlevel.lua:110`) là code sự kiện chết từ 23/05/2011, **nên bỏ khi port**.
5. **`killer_sword.lua`** (script của Sát Thủ Giản) chặn cứng `nowDate >= 20051006080000` → luôn báo
   "Hoạt động Boss hoàng Kim đã kết thúc!". Item chỉ còn tác dụng làm **vé Vượt Ải**, không dùng trực tiếp được.
6. **Ladder 10119 bị 2 hệ ghi chung** (xem 8.3) — phải tách id khi port.
7. `killerCoundTakedTask(90, **350**)` — trần 350 là để hở cho nhân vật chuyển sinh; JX1 cần kiểm
   `GetLevel()` có vượt 150 hay không tuỳ cấu hình máy chủ.
8. **`TSKID_KILLTASKID_SPE` (1793) / `TSKID_KILLERDATE_SPE` (1794)** khai báo nhưng 0 điểm dùng —
   dấu vết của bản "sát thủ cao cấp" đã gỡ.

---

## PHỤ LỤC PHẢN BIỆN (tác tử độc lập)

> Vòng phản biện thứ 2, chạy độc lập với người viết báo cáo. Nguyên tắc: **mặc định coi mọi khẳng định là SAI
> cho tới khi tệp gốc / binary chứng minh ngược lại.** Mọi ô "Bằng chứng gốc" dưới đây đều là kết quả chạy thật
> hôm nay (đọc tệp, đếm bằng máy, dịch ngược ELF), không chép lại từ thân bài.
> Đã kiểm **36 khẳng định** (bảng PL.1): **ĐÚNG hoàn toàn 26** · **SAI (toàn phần hoặc một phần) 9** · **ĐÚNG MỘT PHẦN 1** · **KHÔNG XÁC MINH ĐƯỢC 0**.
> 9 chỗ SAI đều **đã sửa trong thân bài** và đánh dấu `[đã sửa theo phản biện 2]`.
> Tìm thêm **11 chỗ bỏ sót**.

### PL.1 Bảng kiểm chứng

| # | Khẳng định (thân bài) | Bằng chứng gốc (tệp:dòng / địa chỉ ELF) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | **(a)** Bản Linux `AddNpc` gán `nSeries = rand() % 5` | Dịch ngược `jx_linux_y` **0x0811BB10**: `0x0811BBDF call 0x804b28c` (PLT, không đẩy đối số ⇒ `rand()`) → `edi`; `0x0811BC56 mov edx,0x66666667` + `0x0811BC74 imul edx` + `0x0811BC85 sar edx,1` + `0x0811BC89 lea eax,[edx+edx*4]` + `0x0811BC8C sub edi,eax` = `edi % 5`; `0x0811BC8E mov [esp+4],edi` → **đối số hình thức thứ 1** của `0x809fb10` (`[esp]` là `this` = `0x8bacac0`) | **ĐÚNG** | — |
| 2 | **(a)** Ô `[esp+4]` đó chắc chắn là `nSeries` (đối chiếu `AddNpcEx`) | `AddNpcEx` **0x0811BF40**: `0x0811C04D` = `lua_tonumber(L,3)` rồi `0x0811C0AB fistp [esp+4]` — **tham số 3 của AddNpcEx đi vào ĐÚNG ô đó**. Kiểm chéo phía Lua: `battles\boss\bossset.lua:112 AddNpcEx(nNpcID, nNpcLevel, **nNpcSeries**, SubWorld, …)`; `activitysys\config\12\carriage.lua:32` và `config\2\snowman_battle.lua:159` truyền **`random(0,4)`**; `event\laborday\laborday.lua:144` truyền `GoldenBossSeries` (202 điểm gọi `AddNpcEx` trong cây Linux) | **ĐÚNG** | — |
| 3 | **(a)** Bản Linux dùng **tham số 6** làm một cờ BYTE ở `KNpc+0x1824` | `0x0811BD70` = `lua_tonumber(L,6)` → `0x0811BDB0 test eax,eax` → `0x0811BDB2 setne byte ptr [edx+0x1824]`. `AddNpcEx` làm y hệt với **tham số 7** (`0x0811C192 setne [edx+0x1824]`) | **ĐÚNG** | Bổ sung: tên `bNoRevive` **đã được chính dự án chốt** ở đợt PORT5 23/08 — `Sources\Core\Src\ScriptFuns.cpp:6925–6930` ánh xạ ô này sang `Npc[].m_bNoRevive`. Không còn là "chưa xác minh". |
| 4 | **(a)** JX1 `LuaAddNpc` lấy **tham số 6 = nSeries** ⇒ **khác ngữ nghĩa** | `Sources\Core\Src\ScriptFuns.cpp:6814` `int LuaAddNpc(Lua_State* L)`; **`:6834` `int nSeries = (int)lua_tonumber(L, 6);`**; `:6835` `NpcSet.AddNpcSet2(nNpcIdxInfo, nSeries, …)`; `:6836` tham số 7 = tên; tham số 8 **không đọc** | **ĐÚNG (nội dung) / SAI (số dòng)** | `ScriptFuns.cpp:6833` → **`:6834`** (6833 là `nNpcIdxInfo = MAKELONG(nLevel,nId)`) |
| 5 | **(b)** Bảng `addkillertasknpc` có **160 dòng**, 8 nhóm cấp × 20, **40 bản đồ**, 5 map/nhóm × 4 boss/map | Đếm bằng máy trên `killbosshead.lua` (vùng dòng **4→180**, dữ liệu 160 dòng): 160/160 dòng khớp regex 10 cột; cấp NPC `{25,35,45,55,65,75,85,95}` mỗi cấp **đúng 20** dòng; **40** map khác nhau; mỗi nhóm đúng 5 map, mỗi map đúng 4 boss | **ĐÚNG** | — |
| 6 | **(b)** id mẫu NPC 761–820 **thiếu 769** (59 id); `nTaskIndex` 1..160 liên tục; cột 6 luôn 0; cột 8 luôn 1; X 919..2065; Y 2569..3642 | Cùng lần đếm trên: tập id = 59 phần tử, thiếu đúng `{769}`; chỉ số = `range(1,161)`; `{cột6} == {'0'}`; `{cột8} == {'1'}`; X∈[919,2065], Y∈[2569,3642]; script cột 9 duy nhất `kill_level.lua` | **ĐÚNG** | — |
| 7 | **(c)** `1082/1192/1193` khai báo ở `newtask_head.lua:14–16`, `KILLER_MAXCOUNT=8` ở `:20` | `script\task\newtask\newtask_head.lua:14` `TSKID_KILLTASKID = 1082`, `:15` `TSKID_KILLERDATE = 1192`, `:16` `TSKID_KILLERMAXCOUNT = 1193`, `:20` `KILLER_MAXCOUNT = 8`; `nt_setTask` `:31–34`, `nt_getTask` `:37–39` | **ĐÚNG** | — |
| 8 | **(c)** `1217` chỉ dùng ở `kill_level.lua:38/41/44` + `lib_killlevel.lua:27` (mã chết) | `grep -rn "1217" --include=*.lua` toàn cây `D:\ServerLinux\server1`: đúng 4 điểm đó (các hit còn lại là id vật phẩm/ngày/toạ độ, kể cả `killbosshead.lua:176` có X=1217) | **ĐÚNG** | — |
| 9 | **(c)** `1793/1794` khai báo nhưng **0 điểm dùng** | `grep TSKID_KILLTASKID_SPE` → chỉ `newtask_head.lua:17`; `grep TSKID_KILLERDATE_SPE` → chỉ `:18`; các hit `1793/1794` thô đều là id vật phẩm hoặc toạ độ | **ĐÚNG** | — |
| 10 | **(c)** Danh mục §3.1 "AI GHI / AI ĐỌC" của 1082/1192/1193 là **đầy đủ** | `grep 1082` (53 hit) lòi thêm `global\login_old.lua:57 SyncTaskValue(1082)`, `event\teachersday06_v\head.lua:74,77,78 GetTask(1082)` (dải 1..120), `activitysys\config\1050\extend.lua:81 GetTask(1082)`. `grep 1192/1193` lòi thêm `global\general\lenhbaitanthu.lua` và `global\mel\baivip.lua` | **SAI (thiếu)** | Xem PL.2 mục ① và ② |
| 11 | **(d)** 5 hàm `Add120SkillExp` / `NPCINFO_GetSeries` / `GetNpcScript` / `ITEM_DropRateItem` / `AddStatData` **KHÔNG có** ở JX1 | `grep -rn` cả `D:\GAMEDEVNEW\Sources` (`*.cpp` + `*.h`), không chỉ `ScriptFuns.cpp`: **0 hit / 5 hàm** | **ĐÚNG** | — |
| 12 | **(d)** JX1 có `GetNpcSeries` cùng ngữ nghĩa, chỉ cần alias | `ScriptFuns.cpp:7195 int LuaGetNpcSeries` (đọc `Npc[nNpcIndex].m_Series`), đăng ký `:14504`. Dịch ngược Linux `NPCINFO_GetSeries` **0x081C08E0**: kiểm `0 < idx < [0x830ca58]`, `imul edx,eax,0x1a4c`, `cmp [edx+4],eax` (NPC còn sống), rồi `0x081C0959 fild [edx+0x28]` ⇒ đọc đúng một trường int trong `KNpc` | **ĐÚNG** | Bổ sung: Linux có thêm bước kiểm `Npc[idx].id == idx`, JX1 không có (vô hại). |
| 13 | **(e)** `6,1,399` = Sát Thủ lệnh (Linux) ↔ Sát thủ giản (JX1); `6,1,400` = Sát thủ giản ↔ **Sư đồ thiếp**; `6,1,398` = Thần bí khoáng thạch ↔ **Sát Thủ lệnh** | Đọc song song `D:\ServerLinux\server1\settings\item\004\magicscript.txt` (4997 dòng dữ liệu) và `E:\…\bin\server\settings\item\magicscript.txt` (4867 dòng), tra theo bộ 3 `(Genre,DetailType,ParticularType)`: (6,1,398) dòng 400 `Thần bí khoáng thạch` ↔ dòng 400 `Sát Thủ lệnh`; (6,1,399) dòng 401 `Sát Thủ lệnh` ↔ dòng 401 `Sát thủ giản`; (6,1,400) dòng 402 `Sát thủ giản` ↔ dòng 402 `Sư đồ thiếp` (script `\script\item\card\card_shitu.lua`) | **ĐÚNG** | — |
| 14 | **(e)** `2346/2347/2348` và `2355/2356`, `6,0,3`, `6,0,6`, `30528–30557`, `2823/2825/2826`, `3810/3811` đều lệch | Cùng lần tra: (6,1,2346) `Thí Giả Yêu Bài`↔`Kinh Thiên Giáp lễ hộp`; (6,1,2347) `Sát Thủ Bí Bảo`↔`Khấp Địa Quần lễ hộp`; (6,1,2348) `Huyền Thiên Chùy`↔`Sát Thủ Giản lễ hộp`; (6,1,2355)↔`Thí Giả Yêu Bài`; (6,1,2356)↔`Sát Thủ Bí Bảo` (script `\script\item\satthubibao.lua`); `6,0,3`/`6,0,6`/`30528`/`30538`/`30557` **KHÔNG CÓ** ở JX1; `2823/2825/2826`→`越南自定义道具154/156/104`; `3810/3811`→`稽查令`/`手中线` | **ĐÚNG** | — |
| 15 | **(e)** JX1 đã có Sát thủ lệnh/giản ở **genre 2** (`item\mine.txt` dòng 401/402) | `E:\…\settings\item\mine.txt` (1639 dòng, header kiểu Linux): dòng 400 `(2,1,398)` `Thần bí quáng thạch` script `huangjin_tupu.lua`; dòng 401 `(2,1,399)` `Sát thủ lệnh` script `\script\item\killer_token.lua`; dòng 402 `(2,1,400)` `Sát thủ giản` script `\script\item\killer_sword.lua` | **ĐÚNG** | Bổ sung có ích: ánh xạ **Linux `(6,1,P)` → JX1 `(2,1,P)` giữ nguyên P** cho cả 398/399/400 ⇒ bảng đổi item cho hệ này chỉ cần đổi Genre 6→2 ở 3 id lõi. |
| 16 | **(f)** `killer20()`…`killer80()` bị comment ở `nieshichen.lua:73–120`, chỉ `killer90()` (:122) còn sống | Đọc thật `nieshichen.lua`: dòng 73–120 là 7 khối `--function killerNN()` liền nhau; dòng 121 trống; **dòng 122** `function killer90()`; `:123 killerCoundTakedTask(90, 350)`; `:126 showboss(140)`; `:127 tbLog:PlayerActionLog(…)` | **ĐÚNG** | — |
| 17 | **(f)** Vẫn sinh đủ **160** boss, 140 boss cấp 20–80 đứng trên map mà không ai giao được nhiệm vụ | Đường sinh NPC **không lọc theo cấp**: `autoexec.lua:3` Include `killbosshead.lua`; `autoexec.lua:158 add_killertasknpc(addkillertasknpc)`; `killbosshead.lua:183–194` duyệt `1..getn(Tab3)` = 160. Không có nhánh nào bỏ qua cấp 20–80. `kill_level.lua:25` vẫn nhận dải `1..160` | **ĐÚNG** | — |
| 18 | 7 NPC Nhiếp Thí Trần (id 769) ở 7 thành 11/1/37/176/162/78/80 | `autoexec_npc.lua:26–32` — 7 dòng `{769, map, X, Y, "\script\task\tollgate\killer\nieshichen.lua", "Nhiếp Thí Trần"}` với đúng 7 toạ độ ở §4.6; nạp bởi `autoexec.lua:151 add_dialognpc(adddialognpc)` | **ĐÚNG** | — |
| 19 | `killbosshead.lua` + `addtollgatenpc.lua` **giống byte** với bản Linux; `tinsu_addnpc.lua:45` **không** gọi `add_killertasknpc(addkillertasknpc)` | So byte sau khi bỏ CR: `killbosshead.lua` 381 399 B ≡ 381 399 B → **True**; `addtollgatenpc.lua` 693 B ≡ 693 B → **True**. `E:\…\script\task\tollgate\tinsu_addnpc.lua:44–47` chỉ có `add_alltollgatenpc()` + `tinsu_add_dialognpc(...)`. `grep add_killertasknpc` cả cây JX1 → chỉ các dòng trong 2 tệp trên | **ĐÚNG** | — |
| 20 | `NpcFunLib:CheckKillerdBoss` ở `npcfunlib.lua:142`, dùng bởi **18 cấu hình + 5 bản comment**, và 2 tệp `vng_event` | `npcfunlib.lua:142` def, `:144 GetNpcScript`, `:147` so chuỗi, `:148 GetTask(1082)`, `:149 GetNpcParam`, `:150` dải 1..160, `:151` công thức `(N-20)*2+1 .. +20`. `grep` = 27 hit/27 tệp: **23 config** trong đó **5 bị comment** (1003:170, 1005:113, 1016:189, 1035:251, 1046:252) ⇒ còn **18 sống**; `vng_event` có 3 tệp nhưng `20110225_8_thang_3\npcdeath_award.lua:14` **bị comment** ⇒ còn **2 sống** (`lunaryear2011:13`, `nskt_dropitem:10`) | **ĐÚNG (chính xác từng con số)** | — |
| 21 | `OnPlayerEvent` **không tồn tại** trong bản Linux: 0 định nghĩa Lua, **82 điểm gọi / 60 tệp**, không có chuỗi trong ELF; JX1 đã tự thêm | `eventsys.lua` Linux có đúng 6 hàm (`NewType:9, GetType:23, Reg:27, UnReg:36, GetProcParam:46, OnEvent:62`); `grep "function.*OnPlayerEvent\|OnPlayerEvent *="` = **0**; `grep -c OnPlayerEvent` = **82**, `grep -rl` = **60 tệp**; `grep -c "OnPlayerEvent" jx_linux_y` (tìm chuỗi thô trong ELF) = **0**; `E:\…\script\misc\eventsys\eventsys.lua:74` chú thích + **`:79 function EventSys:OnPlayerEvent(...)`** | **ĐÚNG** | — |
| 22 | Ladder **10119** bị 2 hệ ghi chung (`bangxephang.lua:80` ghi CẤP nhân vật) | `bangxephang.lua:27 Ladder_ClearLadder(10119) -- săn boss sát thủ`; `:78–82` nhánh `szTongName == nil/""` **và** `Player:GetCamp() == 4` → `:80 Ladder_NewLadder(10119, Player:GetName(), Player:GetLevel(), 1)`; `kill_level.lua:44` ghi `Uworld1217+1`; `missions\challengeoftime\npc.lua:125,:230` đọc 10119 | **ĐÚNG** | — |
| 23 | `huoyuedu.txt` dòng 12 = `11 杀手任务 2871 2 3 3` (MaxCount 2, +3 rồi +3) | Đọc thật `settings\huoyuedu\huoyuedu.txt`: dòng 12 (dữ liệu ActivityId=11) = `11	杀手任务	2871	2	3	3	0…`; dòng 11 = `10	信使任务	2870	2	5	2` (đúng như thân bài nói). `huoyuedu.lua:26 ["shashourenwu"] = 11`; `:12 TSK_TotalHuoYueDu = 2880`, `:13 TSK_Award = 2882`. JX1 **không có** `script\huoyuedu` lẫn `settings\huoyuedu` | **ĐÚNG** | — |
| 24 | `npcs.txt`: JX1 **87 cột** / Linux **103 cột**, JX1 thiếu `PasstSkillId`/`PasstSkillLevel`/`AuraSkillId`/`DropRateFile`; 60/60 id lệch (`*ResistMax` 85→25, 10 id lệch `LifeParam3`); cột chung khớp `LevelScript`/`ReviveFrame=16200`/`Kind=0`/`Camp=5` | Đọc song song 2 `npcs.txt`, dòng = id+2: header Linux 103 cột, JX1 87 cột; 4 cột trên có ở Linux (85=ReviveFrame, 87=DropRateFile, 99=AuraSkillId, 101/102=PasstSkill*) và **vắng hẳn** ở JX1. So 87 cột chung trên id 761..820: **6 cột lệch** — `FireResistMax/ColdResistMax/LightResistMax/PoisonResistMax/PhysicsResistMax` lệch **60/60 id** (85→25), `LifeParam3` lệch **10 id** (vd id 768: 20 000 000 → 1 300 000). id 769 `Kind=3 Camp=6` ở cả hai | **ĐÚNG** | — |
| 25 | Trong đó: "`ReviveFrame` JX1 ở **cột 86**, Linux cột 85" | Header hai bên: Linux `[83]=PoisonResistMax [84]=PhysicsResistMax [85]=ReviveFrame [86]=Stature`; JX1 **y hệt** `[85]=ReviveFrame [86]=Stature` | **SAI** | **Cùng vị trí ở cả hai** (chỉ số 85 đếm-từ-0 = cột 86 đếm-từ-1). Không có lệch cột. |
| 26 | `killerbossmianyi.lua` JX1 **956 B**, lệch đúng 1 giá trị `poisontimereduce_p` 180/280 | `diff` sau khi bỏ CR: **đúng 1 hunk** `40c40  Param2String(180…)` → `Param2String(280…)`. Kiểm 5 giá trị: `Getfasthitrecover_v:32=100`, `Getfreezetimereduce_p:36=200`, `Getpoisontimereduce_p:40=180`, `Getstuntimereduce_p:44=200`, `Getfatallystrikeres_p:48=99`. **Cỡ tệp: 1004 B ở CẢ HAI bên** | **ĐÚNG (nội dung) / SAI (cỡ tệp)** | 956 B → **1004 B** |
| 27 | 47/47 bản đồ có ở JX1, **5 chỗ lệch tên**: 14, 83, 91, 12, 162 | So `<id>_name=` giữa `D:\ServerLinux\server1\settings\maplist.ini` và `E:\…\settings\MapList.ini` trên 40 map boss + 7 map NPC: thiếu 0, lệch **đúng 5**: 83 `Hỏa Lang động`/`Khỏa Lang động`; 14 `Mãnh Hổ động`/(hỏng byte); 91 `Mê cung Kê Quán động`/`Mê cung Kế Quán động`; 12 `…Tín Tướng tự`/`…Tín Tướng Tự`; 162 `Đại Lý phủ`/`Đại Lý` | **ĐÚNG (khớp từng map)** | — |
| 28 | `killer.txt` 160 dòng dữ liệu, TCVN3, JX1 **chưa có**; toạ độ `BossInfo` = `(X//8, Y//16)` | `settings\task\tollgate\killer\killer.txt` = 1 dòng header + **160 dòng** (Id 1..160), gbktool báo `[tcvn3]`. `find` cả `E:\…\bin\server\settings` → chỉ có `settings\killer.ini`, **không có** `task\tollgate\killer\`. Kiểm công thức bằng máy trên **cả 160 dòng**: **khớp 157**, lệch 3 (#17, #19, #95) | **ĐÚNG MỘT PHẦN** | Đổi "đã kiểm 5 mẫu" → "**157/160**, 3 dòng dữ liệu `killer.txt` ghi sai toạ độ" (đã sửa ở §9.1) |
| 29 | "Tên boss trong `killbosshead.lua` và `killer.txt` **KHÔNG luôn giống nhau về chính tả**" | So **nguyên văn** (không strip) 160 cặp tên sau khi giải mã: **lệch 0/160** | **SAI** | Tên trùng **160/160**; cái lệch thật là 3 dòng `BossInfo` sai toạ độ (đã sửa ở §4.4) |
| 30 | `settings\killer.ini` **không thuộc** hệ Săn Boss Sát Thủ | `cat` tệp: dòng 1 `;通缉追杀系统专用ini` (= "ini chuyên dụng của hệ TRUY NÃ/TRUY SÁT"), khoá `MoneyPerHour/MinTargetLevel/MaxActiveTaskTime/MinReward` + 2 câu thông báo truy nã. `grep -rni "killer\.ini"` toàn bộ `script\` Linux = **0 hit** | **ĐÚNG** | Bổ sung: **JX1 cũng đang có `E:\…\bin\server\settings\killer.ini`** — càng dễ nhầm, phải ghi rõ khi bàn giao. |
| 31 | `IncludeLib`: JX1 có bảng 21 module, `RELAYLADDER`/`ITEM` trỏ `noop.lua`, **`NPCINFO` không có** → rơi vào nhánh "bỏ qua" | `ScriptFuns.cpp:2480 int LuaIncludeLib`; bảng `szMod[21]` ở **`:2486–2489`** (có `RELAYLADDER`, `ITEM`, **không có** `NPCINFO`); nhánh cuối `:2551 g_DebugLog("[script] IncludeLib: module la [%s] - bo qua")` rồi `PushNumber(0)` | **ĐÚNG** | — |
| 32 | Các số dòng đăng ký hàm engine ở §7 | Kiểm từng dòng trong `ScriptFuns.cpp`: `SyncTaskValue` **14883** ✓ · `SetNpcParam` **14507** ✓ · `GetNpcParam` **14509** ✓ · `SetNpcScript→LuaSetNpcActionScript` **14500** ✓ · `Ladder_NewLadder` **15085** ✓ · `GetTeamSize` **14365** ✓ · `GetTeamMember` **14292** ✓ (alias `LuaGetTeamMem`, nằm trong `#ifdef _SERVER` 14288–14293) · `DynamicExecuteByPlayer` **14308** ✓ · `AddOwnExp` **14617** ✓ · `GiveItemUI` **15214** ✓ · `GetGiveItemUnit` **15215** ✓ · `RemoveItemByIndex` **15216** ✓ · `SubWorldID2Idx` **14477** ✓ · `CalcItemCount→LuaWllsCalcItemCount` **14911** ✓ · `CalcFreeItemCellCount` **14431** ✓ · `ConsumeItem` **14384** ✓ · `AddSkillState` **14349** ✓ · `GetNpcSeries` **14504** ✓ · `GetItemProp→LuaGetItemProp` (`KJx2WarInfra.cpp:769`, trả **6** giá trị) ✓ — **chỉ `TabFile_*` lệch**: thực tế **15284/15285**, `15283` là `GetLocalDateEx` | **ĐÚNG (18/19) / SAI (1)** | `ScriptFuns.cpp:15283–15285` → **`:15284` / `:15285`** |
| 33 | §9.7: **28** tệp JX1 chưa có · **3** tệp có-nhưng-khác | Kiểm sự tồn tại từng tệp trong `E:\…\bin\server\script`: **28/28 đều KHÔNG có**. 3 tệp [B] đều **có** và đều **khác** (`lib\awardtemplet.lua` 1790 B / 1332 B; `misc\eventsys\eventsys.lua` 2544 B / 1562 B; `task\newtask\newtask_head.lua` 2851 B / 1798 B) | **ĐÚNG** | — |
| 34 | §0: 26 + 13 = **39** hàm Lua, **36** hàm engine, **67** tệp bao đóng, **31** tệp đã đọc | Đếm dòng bảng trong chính `01_satthu.md`: §6.1 = 26 dòng ✓, §6.2 = 13 dòng ✓, §7 = 36 dòng ✓. `closure3.json["satthu"]` = **67** ✓ và `src_utf8\satthu` có **67** tệp ✓. Nhưng §1.2 liệt kê **25** tệp (không phải 24) ⇒ tổng đã đọc = **32** | **ĐÚNG (3/4) / SAI (1)** | "7 + 24 = 31" → **"7 + 25 = 32"** |
| 35 | §1.1 số dòng tệp hạt giống | Đếm byte thật: `kill_level.lua` 123 ✓ · `lib_killlevel.lua` 148 ✓ · `addtollgatenpc.lua` 21 ✓ · `killbosshead.lua` 3421 ✓ · **`nieshichen.lua` 232** (báo 233) · **`mibao_head.lua` 22** (báo 23) · **`shashou_mibao.lua` 28** (báo 29) · `eventsys.lua` **72** (§8.2 ghi "73 dòng") | **SAI (4 con số ±1)** | 233→**232**, 23→**22**, 29→**28**, 73→**72** |
| 36 | §2.2: hộp thoại NPC có thêm mục "Quá quan tầm bảo" khi trúng ngày sinh nhật | `nieshichen.lua:58` `tbDialog` có **8** phần tử; `:60 tinsert(tbDialog, 12, ContentList[29])`. `luaB_tinsert` của chính engine (`Sources\Library\LuaLib\src\baselib\lbaselib.c:391–410`): đặt `t.n = n+1 = 9`, vòng dịch `for(; n>=pos; n--)` **không chạy** (8 ≥ 12 sai), rồi `t[12] = v`. ⇒ `getn = 9`, `unpack` trả `t[1..9]` với `t[9] = nil`; `t[12]` không ai đọc | **SAI** | Mục đó **không bao giờ hiện**; ngày sinh nhật chỉ thêm **một ô rỗng** vào `Describe`. Đây là **lỗi thật của bản gốc**, phải bỏ `tinsert` hoặc đổi thành `tinsert(tbDialog, 8, …)` khi port. |

### PL.2 Bỏ sót đã tìm thêm

**① 🔴 Hai hệ khác của bản Việt hoá dùng CHUNG task 1192/1193 — §9.6 nói "không xung đột" là sai.**
`script\global\general\lenhbaitanthu.lua:152,155–159` và `script\global\mel\baivip.lua:148,151–155` (script của 2 vật phẩm
"Lệnh bài tân thủ" / "Bãi VIP") đọc `nt_getTask(1192)`, và **tự làm bước reset theo ngày**:
```lua
if myDateBossST ~= nDate then
    nt_setTask(1193, 0);       -- xoá SỐ LƯỢT SĂN BOSS HÔM NAY của hệ sát thủ
    nt_setTask(1192, nDate);
end
local nBossST = nt_getTask(1193)
```
⇒ chỉ cần người chơi **mở vật phẩm** là hai biến của hệ sát thủ bị hệ khác ghi. Khi port phải quyết định:
mang theo cả hai tệp này, hay tách id task riêng cho JX1.

**② `SoLuongBossSatThuTrongNgay = 100` — hằng cấu hình của chính hệ này mà báo cáo không tìm ra.**
`script\global\pgaming\configserver\configall.lua:36`. Hai tệp ở mục ① in ra `"Boss sát thủ: <nBossST>/<100>"`,
trong khi cửa nhận nhiệm vụ chặn ở `KILLER_MAXCOUNT = 8` (`newtask_head.lua:20`). **Hai con số mâu thuẫn nhau
trong cùng một bản build** — port sang JX1 phải chốt lấy con số nào.

**③ Thêm 3 điểm đọc/ghi task 1082 ngoài danh mục §3.1/§3.4.**
`script\global\login_old.lua:57 SyncTaskValue(1082)` (đồng bộ lúc **đăng nhập** — JX1 phải có dòng tương đương,
nếu không client không thấy nhiệm vụ đang cầm) · `script\event\teachersday06_v\head.lua:74,77,78`
(`GetTask(1082)` với dải **1..120**, tức bản cũ khi chỉ có 120 boss) · `script\activitysys\config\1050\extend.lua:81`.

**④ `killer_sword.lua` kéo theo 2 tệp `laborday` mà §9.7 không liệt kê.**
`script\item\killer_sword.lua:1–8` `Include("\script\event\laborday\labordayhead.lua")` và, khi `LABORDAYSWITCH`,
`Include("\script\event\laborday\laborday.lua")`. Cây JX1 **không có** thư mục `script\event\laborday`
(Linux: `laborday.lua` 7256 B, `labordayhead.lua` 83 B). Hai tệp này không nằm trong danh sách [A] 28 tệp lẫn [D].

**⑤ 🔴 Bảng vật phẩm JX1 đang trỏ tới 3 script KHÔNG TỒN TẠI.**
`magicscript.txt` JX1 gán `(6,1,2356)` → `\script\item\satthubibao.lua`; `mine.txt` JX1 gán `(2,1,399)` → `killer_token.lua`
và `(2,1,400)` → `killer_sword.lua`. Tìm cả `E:\…\bin\server\script` lẫn `…\scriptjx2`: **cả 3 tệp đều không có**.
⇒ Phần "JX1 đã có sẵn" ở §9.3 chỉ đúng ở **dòng bảng**, chưa có **mã**.

**⑥ Hai địa chỉ ELF còn thiếu + chữ ký thật của `GetNpcScript`.**
`jx_linux_y.luamap.full.txt`: **`Add120SkillExp = 0x0811C710`** (dòng 674), **`GetNpcScript = 0x080FFCF0`** (dòng 123)
— §9.4 liệt kê 2 hàm này mà không kèm địa chỉ, khác với 3 hàm còn lại. Dịch ngược:
- `GetNpcScript` (0x080FFCF0): kiểm `0 < idx < [0x830ca58]`, đẩy **chuỗi** ở `KNpc+0x1538` rồi đẩy **số** ở `KNpc+0x1588`,
  `mov eax, 2; ret` ⇒ **trả 2 giá trị**, đúng như `missions\maze\maze.lua:1280 local script, _ = GetNpcScript(...)`.
  Bản JX1 phải `Lua_PushString` + `Lua_PushNumber` rồi `return 2`, không phải 1.
- `Add120SkillExp` (0x0811C710): đòi người chơi hợp lệ (`call 0x8107860` > 0), `imul esi,0x8788` (sizeof KPlayer)
  + `[0x8baee60]` → `call 0x80a9b50(pPlayer, nExp)`, trả 1 số. **Có logic thật, không phải stub.**

**⑦ Chữ ký thật của `ITEM_DropRateItem` — tham số 5 là CỜ, không phải số.**
Dịch ngược **0x08154DE0**: đòi người chơi hợp lệ (`call 0x8107910` > 0) và **≥ 6 tham số** (`cmp eax,5; jg`);
tham số 1 → int (npc idx); tham số 2 → giữ dạng **double** (số lượng); tham số 3 → chuỗi, **bắt buộc khác rỗng**;
tham số 4 → số; **tham số 5 → `0x08154EB1 setne dl` ⇒ chỉ dùng như BOOLEAN (0 / khác 0)**; tham số 6 → số (ngũ hành).
⇒ Ghi lại chữ ký là `(nNpcIdx, nCount, szIniPath, nParam4, bFlag, nSeries)`, thay cho `(…, ?, ?, nSeries)` ở §7 dòng 13.
(Điều này giải thích vì sao 8 lời gọi bị comment truyền 4,5,6,7,8,9,10 ở vị trí 5 — mọi giá trị khác 0 đều như nhau.)

**⑧ `AddExp_Skill_Extend` chỉ cộng MỘT NỬA.**
`task\task_award_extend.lua:6` = `Add120SkillExp(floor(nExp / 2))`. Vậy `lib_killlevel.lua:102 AddExp_Skill_Extend(140000)`
thực tế cộng **70 000** tu luyện độ kỹ năng 120, không phải 140 000 như bảng §2.5 gợi ý.

**⑨ Thêm 2 khác biệt của `LuaAddNpc` JX1 mà bảng §9.2 chưa có — và JX1 ĐÃ CÓ SẴN `AddNpcEx`.**
- `ScriptFuns.cpp:6823–6827`: nhánh `Lua_IsString(L,1)` **bị comment**, nên JX1 `AddNpc` **không nhận tên NPC dạng chuỗi**
  (bản Linux có: `0x0811BB75` `lua_tostring(L,1)` → tra bảng `0x830aec0` → `FindRow - 2`). Bảng sát thủ luôn truyền số nên không chặn, nhưng phải biết.
- `ScriptFuns.cpp:6831–6832`: JX1 ép `nLevel ≥ 128 → 127`; bản Linux chỉ lấy **WORD thấp** (`movzx edx, ax`) và ép `< 0 → 1`.
- **`LuaAddNpcEx` đã tồn tại** (`ScriptFuns.cpp:6874`, đăng ký `:14495`) với đúng chữ ký Linux
  `(nId, nLevel, nSeries, nSubWorldIdx, nX, nY, nCamp, szName, nFlag)` ⇒ phương án "dùng `AddNpcEx` với `nSeries = random(0,4)`"
  ở §9.2 **chạy được ngay hôm nay**, không phải viết hàm mới. Ngoài ra chú thích `:6925–6930` cho thấy dự án
  đã ánh xạ cờ `KNpc+0x1824` sang `Npc[].m_bNoRevive` và **chỉ áp khi `g_IsJx2Script(L)`**.

**⑩ Rủi ro #1 ĐÃ hiện thực ngay trong cây JX1 đang chạy.**
`E:\…\script\task\tollgate\tinsu_addnpc.lua:38` = `AddNpc(itemlist[1], 1, SId, X*32, Y*32, **0**, itemlist[6])`
— chép đúng thói quen "tham số 6 = 0" của bản Linux, nên toàn bộ NPC Tín Sứ hiện đang mang **ngũ hành Kim (0)**.
Đây là bằng chứng sống cho §9.2, nên dẫn ra khi bàn giao.

**⑪ `UWorld1082` trong `nieshichen.lua` là biến TOÀN CỤC, và `havetask()` đọc CACHE chứ không gọi `GetTask` mới.**
`nieshichen.lua:57` `UWorld1082 = nt_getTask(1082)` (**không có `local`**) — chỉ được gán trong `main()`;
`:135–141 havetask()` so `UWorld1082 ~= 0`. Vì cả máy chủ dùng **một Lua_State chung**, giá trị này là trạng thái
dùng chung giữa mọi người chơi và chỉ đúng nếu người chơi luôn đi qua `main()` trước. §2.6 mô tả là
"`havetask()` chặn nếu `GetTask(1082) ~= 0`" — **không chính xác**; khi port nên đổi thành đọc `GetTask` trực tiếp.

**Ghi chú thêm (không tính là bỏ sót):** JX1 đã có tiền lệ đặt tên `NPCINFO_*` cho hàm dịch ngược từ Linux —
`LuaNPCINFO_GetNpcCurrentLife` / `LuaNPCINFO_SetNpcCurrentLife` (`ScriptFuns.cpp:3955` / `:3966`, đăng ký `:15267` / `:15268`,
kèm địa chỉ Linux `0x081C06B0` / `0x081C0070`). Thêm `NPCINFO_GetSeries` (`0x081C08E0`) đi đúng khuôn đó.
