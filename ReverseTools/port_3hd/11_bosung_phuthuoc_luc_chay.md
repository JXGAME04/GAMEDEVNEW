# 11 — Bổ sung: PHỤ THUỘC GỌI LÚC CHẠY (runtime) mà bao đóng `Include()` không thấy

**Vòng 2 · hướng quét 2** · ngày 24/08 · chỉ PHÂN TÍCH, không sửa bất cứ tệp nguồn nào.
Tệp sinh ra trong đợt này: `11_bosung_phuthuoc_luc_chay.md` (tệp này), `closure3_runtime.json`,
và các tệp trung gian `_rt_hits.json` / `_rt_paths.json` / `_rt_sites.json` / `_rt_data.json` /
`_rt_rev.json` / `_rt_luadata.json`.

---

## 0. Tóm tắt cho người đọc vội

| Việc | Kết quả |
|---|---|
| Số điểm gọi lúc chạy tìm được trong tệp lõi + tệp vào | **satthu 28 dòng · phonglangdo 36 dòng · vuotai 65 dòng**  `[đã sửa theo đối chất]` |
| Số cạnh có đường dẫn `.lua` tường minh | 15 · 14 · 13 |
| Bao đóng ĐẦY ĐỦ (Include + goi lúc chạy, đệ quy thêm 1 mức) | **220 · 202 · 214** lượt tệp |
| `closure3.json` (chỉ `Include`) | 67 · 82 · 103 |
| **Tệp MỚI so với `closure3.json`** | **+153 · +120 · +111** |
| Trong đó thuộc **tầng LÕI** (bắt buộc phải port) | **9 · 5 · 8** = **22 lượt / 21 tệp phân biệt** (`huoyuedu.lua` dùng chung satthu+phonglangdo)  `[đã sửa theo đối chất]` |
| Còn lại | fan-out của `script_protocol\protocol_def_gs.lua` (26 bộ khác) + include cấp 2 của chúng — **không bắt buộc** |

> 🔴🔴 **Phát hiện lớn nhất**: bản Linux **không có 2 gốc mà có BA**.
> Gốc thứ ba là `D:\ServerLinux\gateway` — chứa **toàn bộ lớp lập lịch S3Relay**.
> **Điểm vào thật của Phong Lăng Độ và Vượt Ải nằm ở gốc này**, không nằm trong
> `server1\script` lẫn `Patch`. Không tệp nào trong 6 báo cáo vòng 1 nhắc tới chuỗi
> `gateway`, `relaysetting` hay `tasklist.ini`.

---

## 1. Cảnh báo về danh sách hàm trong đề bài

Đếm thật trên toàn bộ `D:\ServerLinux\server1\script` (lệnh `grep -r -a -o "\bTÊN("`):

| Hàm | Số lượt | Ghi chú |
|---|---:|---|
| `CallPlayerFunction` | 708 | |
| `SetNpcScript` | 259 | |
| `DynamicExecuteByPlayer` | 189 | |
| `AddTimer` | 176 | |
| `SetDeathScript` | 161 | |
| `RemoteExecute` | **152** | ❗ không có trong đề bài — chạy script **phía Relay** |
| `SetNpcTimer` | 130 | ❗ không có trong đề bài |
| `DynamicExecute` | 110 | |
| `StopMissionTimer` | **102** | ❗ tên thật của "mission timer" |
| `SetNpcDeathScript` | 96 | |
| `dofile` | **93** | ❗ không có trong đề bài |
| `StartMissionTimer` | **92** | ❗ tên thật |
| `GlobalExecute` | **23** | ❗ Relay ⟶ GameServer |
| `LoadScript` | **23** | ❗ |
| `StartGlbMSTimer` | 6 | ❗ |
| `DynamicExecuteFile` | **0** | ⛔ **không tồn tại** trong mã nguồn này |
| `ExeScript` | **0** | ⛔ không tồn tại |
| `SetTaskScript` | **0** | ⛔ không tồn tại |
| `AddMissionTimer` | **0** | ⛔ không tồn tại — tên thật là `StartMissionTimer` |
| `SetMissionTimer` | **0** | ⛔ không tồn tại |
| `AddGlobalMissionTimer` | **0** | ⛔ không tồn tại — tên thật là `StartGlbMSTimer` |

⇒ Nếu đợt sau đi tìm `AddMissionTimer` sẽ ra **0 kết quả và kết luận nhầm là "không có hẹn giờ"**.
Hàm thật là `StartMissionTimer(nMissionId, nTimerIdx, nTime)` / `StopMissionTimer(nMissionId, nTimerIdx)`
(địa chỉ ELF `0x08138840` và `0x08134720`, `jx_linux_y.luamap.full.txt`).

**Phía JX1 đã đăng ký sẵn** (`D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp`):
`CallPlayerFunction`:14301 · `DynamicExecuteByPlayer`:14308 · `DynamicExecute`:14900 ·
`SetNpcScript`:14500 · `SetDeathScript`:14776 · `SetNpcDeathScript`:15190 · `AddTimer`:15254 ·
`SetNpcTimer`:14555 · `StartMissionTimer`:14750 · `StopMissionTimer`:14751 ·
`StartGlbMSTimer`:15120 · `StopGlbMSTimer`:15121 · `OpenGlbMission`:15119 · `OpenMission`:14746 ·
`CloseMission`:14748 · `DelMSPlayer`:14757 · `GlobalExecute`:14882 · `RemoteExecute`:15258 ·
`LoadScript`:14899 · `AddNpcEx`:14495 · `SetNpcParam`:14507 · `GetNpcParam`:14509 ·
`SetTmpCamp`:15252 · `ClearMapNpc`:15191 · `AddGlobalNews`:14311 · `AddLocalCountNews`:14316 ·
`SubWorldIdx2ID`:14478 · `Ladder_NewLadder`:15085.

⇒ **Không có hàm nạp script nào phải viết mới.** Khối lượng nằm ở dữ liệu và ở lớp lập lịch.

---

## 2. 🔴🔴 GỐC DỮ LIỆU THỨ BA — `D:\ServerLinux\gateway`

```
D:\ServerLinux\
   ├─ server1\   (A — thư mục chạy GameServer)
   ├─ Patch\     (B — lớp cập nhật)
   └─ gateway\   (G — Bishop + S3Relay + BishopConn + KG_SyncD)   ← VÒNG 1 CHƯA BIẾT
```

`gateway\s3relay\` có **1.189 tệp**, trong đó `relaysetting\` có **378 tệp**, riêng `relaysetting\task\` có **227 tệp ở mức 1** (354 kể cả thư mục con) — đây là
**bộ lập lịch của máy chủ**: mỗi tệp là một "task" khai báo `TaskShedule()` (giờ chạy, chu kỳ)
và `TaskContent()` (việc phải làm), rồi bắn lệnh sang GameServer bằng `GlobalExecute` /
`RemoteExecute`.

Sổ đăng ký là `gateway\s3relay\relaysetting\task\tasklist.ini` — `[List] Count=128`,
mỗi mục `[Task_N] TaskFile=<tên>.lua`. **Chỉ tệp có tên trong `tasklist.ini` mới thật sự chạy.**

### 2.1 Ba task đang BẬT có liên quan

| Mục | Tệp | Tính năng | Lịch | Việc |
|---|---|---|---|---|
| `Task_52` (`tasklist.ini:209`) | `challengeoftime.lua` | **Vượt Ải** | mỗi **60 phút**, phút :00 (`:20,:23`) | `GlobalExecute("dwf \settings\trigger_challengeoftime.lua OnTrigger()")` (`:34`) + `AddLocalCountNews(szMsg,2)` (`:36`) báo "báo danh 10 phút, đến **Nhiếp Thí Trần**" |
| `Task_84` (`tasklist.ini:312`) | `challegeoftime-dailyrank.lua` *(tên gõ sai "challegeoftime" — giữ nguyên)* | **Vượt Ải** | `TaskTime(0,0)`, `TaskInterval(1440)` = **00:00 hằng ngày** | `Ladder_GetLadderInfo(10235, 1)` → `AddGlobalNews` + `Msg2SubWorld` → **`Ladder_ClearLadder(10235)`** (`:18-26`) |
| `Task_86` (`tasklist.ini:318`) | `fengling_ferry.lua` | **Phong Lăng Độ** | mỗi **60 phút**, phút :00 (`:4-13`) | `GlobalExecute("dwf \script\missions\fengling_ferry\fldmap_boat1.lua fenglingdu_main()")` (`:21`) |

> 🔴 Vòng 1 (`03_vuotai.md:629,646`) quy bảng xếp hạng ngày cho `rank_perday.lua`.
> Thật ra có **HAI đường**: `rank_perday.lua` (phía GameServer, phát thưởng cá nhân) **và**
> task Relay `challegeoftime-dailyrank.lua` (phía Relay, công bố đội nhanh nhất rồi **xoá sạch
> ladder 10235 lúc 00:00**). Port thiếu vế Relay ⇒ ladder không bao giờ được xoá, bảng ngày sai vĩnh viễn.

### 2.2 Các tệp KHÔNG đăng ký (bẫy đếm thừa)

- `challengeoftime_0100.lua … _2300.lua` (12 tệp) và `fengling_ferry_0200.lua … _2400.lua` (12 tệp):
  bản "chạy theo giờ cố định", **không có trong `tasklist.ini`** ⇒ **không chạy**.
- Hai thư mục Việt hoá `relaysetting\task\phonglangdo\` (25 tệp) và `relaysetting\task\vuotai\` (2 tệp):
  bản sao dàn dựng, **cũng không đăng ký**.
- `yandibaozang.lua:50` có một dòng đã bị chú thích:
  `--GlobalExecute("dw LoadScript([[\\settings\\trigger_challengeoftime.lua]])")` — mã chết.

⇒ Cần port **3 tệp task** (không phải 54 tệp) + hiểu cơ chế `tasklist.ini`.

### 2.3 Phía JX1 tương ứng

JX1 **không có S3Relay task**. Vai trò "đồng hồ treo tường" do **một mình** `script\timerserver.lua`
(40.421 B) đảm nhiệm: `CoreServerShell.cpp:1139` lấy script, `:1170` gọi `CallFunction("RunTime")` mỗi tick.
`settings\TimerTask.txt` + `script\timertask\task01..10.lua` là **cơ chế KHÁC** — bảng khe *mission/player
timer* mà `StartMissionTimer`/`SetTimer` tra (`KTaskFuns.cpp:108` nạp bảng, `:127` + `:177-186` tra khe),
**không** phải bộ lập lịch theo giờ. Đừng gộp hai thứ này.  `[đã sửa theo đối chất]`

🔴 Và JX1 **đã có sẵn chỗ đáp**: `timerserver.lua:15` `Include("\script\tinhnang\vuot_ai\lib_vuotai.lua")`,
`:16` `Include("\script\tinhnang\phonglangdo\lib_phonglangdo.lua")`; hai lời gọi lịch
`-- sukien_vuotai(nHr,nMi)` (`:79`) và `-- sukien_phonglangdo(nHr,nMi)` (`:80`) **đã có, đang bị chú thích**.
⇒ 3 task Relay chuyển thành mục trong `timerserver.lua` (**không** chép nguyên `.lua` của Relay), và với
Phong Lăng Độ / Vượt Ải thì phải **đối chiếu với bản VN đang có** (xem mục **Bỏ sót của chính vòng 2 · A** ở cuối tệp) chứ không viết mới.

---

## 3. BẢNG 1 — SĂN BOSS SÁT THỦ (`satthu`)

**28 dòng có lời gọi lúc chạy**  `[đã sửa theo đối chất]`; hàm dùng: `SetNpcScript`×8, `DynamicExecute`×11,
`dofile`×4, `OpenGlbMission`×2, `OpenMission`×1, `SetNpcDeathScript`×1, `DynamicExecuteByPlayer`×1.

| Điểm gọi (tệp:dòng) | Hàm gọi | Đường dẫn đích | Trong `closure3`? | Có ở Linux? | Có ở JX1? |
|---|---|---|---|---|---|
| `script\task\tollgate\killbosshead.lua:190` | `SetNpcScript` | `Tab3[i][9]` = **cột 9 của bảng `addkillertasknpc`** ⇒ `\script\task\tollgate\killer\kill_level.lua` (**160 dòng SỐNG**) | CÓ | A | CHƯA |
| `killbosshead.lua:3400` | `SetNpcScript` | `Tab6[i][9]` = bảng `AddNpc_turesureboss` ⇒ `…\messenger\qianbaoku\90\tureboss90.lua` (**9 dòng sống**, 300 dòng đã chú thích) | **KHÔNG** | A | CÓ |
| `killbosshead.lua:3411` | `SetNpcScript` | `Tab4[i][9]` = bảng `AddNpc_turesurebug` ⇒ `…\messenger\qianbaoku\90\turebug90.lua` (**9 dòng sống**) | **KHÔNG** | A | CÓ |
| `script\task\tollgate\killer\lib_killlevel.lua:29` | `DynamicExecuteByPlayer` | `\script\huoyuedu\huoyuedu.lua` hàm `tbHuoYueDu:AddHuoYueDu("shashourenwu")` | **KHÔNG** | A | **CHƯA** |
| `script\global\autoexec.lua:158` | *(gọi hàm Lua)* | `add_killertasknpc(addkillertasknpc)` — **ĐIỂM VÀO THẬT**, sinh 160 boss lúc khởi động | **KHÔNG** (autoexec không nằm trong closure3) | A | CHƯA |
| `script\global\autoexec.lua:3-4` | `Include` | `killbosshead.lua`, `addtollgatenpc.lua` | — | A | — |
| `script\global\autoexec.lua:191` | `SetNpcScript` | `\script\event\jiefang_jieri\200904\denggao\npc.lua` | KHÔNG | A | CHƯA |
| `script\global\autoexec.lua:128-139` | `DynamicExecute` ×9 | `tianchimijing\floor1..4\*.lua`, `yuegedao\*`, `changefeature\initmap.lua`, `global\npc\huoke.lua` | KHÔNG | A | CHƯA |
| `script\global\thanh\npc\add_npc.lua:228` | `SetNpcDeathScript` | `TabIndex[i][9]` = **`\script\global\thanh\npc\bosssatthuhead.lua`** (160 dòng) | **KHÔNG** | A | **CHƯA** |
| `script\global\thanh\npc\npc_hotro.lua:12` · `npc_chuyensinh.lua:18` | `dofile` | tự nạp lại chính nó (`dofile("script/global/thanh/npc/…")`) | KHÔNG | A | CHƯA |
| `script\activitysys\npcfunlib.lua:147` | *(so sánh chuỗi)* | `if szScript == "\script\task\tollgate\killer\kill_level.lua"` — hệ hoạt động **nhận diện boss sát thủ bằng CHUỖI ĐƯỜNG DẪN** | KHÔNG | A | CHƯA |

### 3.1 🔴 Bản Việt hoá SONG SONG của "boss sát thủ" — hoàn toàn thiếu ở vòng 1

`script\global\thanh\npc\` (6 tệp, không tệp nào có trong `closure3.json`, không tệp nào được
6 báo cáo vòng 1 nhắc tới):

| Tệp | Cỡ | Vai trò |
|---|---:|---|
| `add_npc.lua` | 23.628 B | bảng `tbNpcSatThu` **160 boss** trên **bản đồ 995**, toạ độ `random(1590,1625)/random(3180,3250)`, hàm `add_boss()` (`:211-232`) dùng `AddNpcEx` + `SetNpcDeathScript` + `SetNpcParam(idx,1,thứ_tự)` |
| `bosssatthuhead.lua` | 2.298 B | `OnDeath` → `kill_level20()` chia 8 bậc theo `GetNpcParam(idx,1)`, mỗi bậc gọi `ITEM_DropRateItem(idx,1,"\settings\droprate\boss\bosstask_lev20..90.ini",0,4..10,nseries)` rồi `add_expforkiller()` cộng 15.000 → 2.000.000 exp |
| `bosssatthudeath.lua` | 327 B | biến thể |
| `npcdeath.lua` | 432 B | `RoiDo(num)` |
| `npc_hotro.lua` | 1.651 B | NPC Hỗ Trợ Tân Thủ (sửa **23/11/2025** — cây này còn sống) |
| `npc_chuyensinh.lua` | 2.474 B | NPC Chuyển Sinh |

Điểm nạp: `script\global\gm\ex_lib_function.lua:77-78` `Include` cả hai.
Hàm `add_npc_thanh()` (`add_npc.lua:203`) **không được tệp nào trong cây `script\` gọi**
⇒ chỉ chạy khi GM kích hoạt bằng tay. **CHƯA XÁC MINH** máy chủ VN thật có bật hay không.

> 🔑 Giá trị: đây là **bản tham chiếu Việt hoá đã đơn giản hoá** của cùng một tính năng —
> cùng 8 bảng rơi đồ `bosstask_lev20..90.ini`, cùng 160 boss, nhưng **gộp hết vào 1 bản đồ 995**
> và bỏ toàn bộ hệ nhiệm vụ. Nếu chỉ cần "săn boss sát thủ" chứ không cần bảng nhiệm vụ,
> port bản này rẻ hơn nhiều.

### 3.2 Đính chính `00_ghichu_dieuphoi.md` §2

Ghi chú vòng 1 nói 8 tệp `Patch\settings\droprate\boss\bosstask_lev20..90.ini` là **"chỉ có ở B"**.
Đo lại: **cả A và B đều có**, cùng kích thước (6.233/6.237/6.252/6.252/6.255/6.251/6.262/6.214 B).
Điều đúng là **JX1 chưa có** (thư mục `bin\server\settings\droprate\boss\` chỉ có
`helianpiaodroprate.ini`).

---

## 4. BẢNG 2 — PHONG LĂNG ĐỘ (`phonglangdo`)

**36 dòng có lời gọi lúc chạy**; hàm dùng: `SetNpcDeathScript`×11, `SetNpcTimer`×6, `SetLogoutRV`×5,
`SetDeathScript`×3, `OpenMission`×3, `CloseMission`×2, `StartMissionTimer`×2, `StopMissionTimer`×2,
`GlobalExecute`×1, `DynamicExecuteByPlayer`×1.

| Điểm gọi (tệp:dòng) | Hàm gọi | Đường dẫn đích | Trong `closure3`? | Có ở Linux? | Có ở JX1? |
|---|---|---|---|---|---|
| `gateway\s3relay\relaysetting\task\fengling_ferry.lua:21` | `GlobalExecute("dwf …")` | `\script\missions\fengling_ferry\fldmap_boat1.lua` hàm **`fenglingdu_main()`** — **ĐIỂM VÀO THẬT, mỗi 60 phút** | CÓ (tệp đích) — **nhưng cạnh gọi thì KHÔNG** | A (đích) / **G** (nguồn) | CHƯA |
| `missions\fengling_ferry\mission.lua:15` | `StartMissionTimer` | `(MISSIONID=15, 29, FLD_TIMER_1)` ⇒ khe **29** = `\script\missions\fengling_ferry\fld_smalltimer.lua` theo `settings\timertask.txt:30` | — | A+B | CHƯA |
| `missions\fengling_ferry\mission.lua:16` | `StartMissionTimer` | `(15, 28, FLD_TIMER_2)` ⇒ khe **28** = `fld_landingtimer.lua` (`timertask.txt:29`) | — | A+B | CHƯA |
| `missions\fengling_ferry\mission.lua:57` | `StopMissionTimer` | `(15, 28)` | — | — | — |
| `missions\fengling_ferry\boss.lua:30,37,46,53,62,69` | `SetNpcDeathScript` | **`\script\global\tamhiep\callbossdeathmini.lua`** | **KHÔNG** | ⛔ **KHÔNG CÓ Ở CẢ A LẪN B** | KHÔNG |
| `missions\fengling_ferry\fld_smalltimer.lua:38,45,61` | `SetNpcDeathScript` | `…\fengling_ferry\bossdeath.lua` | CÓ | A | CHƯA |
| `missions\fengling_ferry\fld_smalltimer.lua:52` | `SetNpcDeathScript` | `…\fengling_ferry\bigbossdeath.lua` | CÓ | A | CHƯA |
| `missions\fengling_ferry\fld_smalltimer.lua:82` | `DynamicExecuteByPlayer` | `\script\huoyuedu\huoyuedu.lua` → `tbHuoYueDu:AddHuoYueDu("fenglingdu")` | **KHÔNG** | A | **CHƯA** |
| `missions\fengling_ferry\fld_head.lua:146` | `SetDeathScript` | `…\fengling_ferry\fld_death.lua` | CÓ | A | CHƯA |
| `missions\fengling_ferry\mission.lua:23` | `SetNpcDeathScript` | `…\fengling_ferry\shuizeideath.lua` | CÓ | A | CHƯA |
| `missions\fengling_ferry\boss.lua:31,38,47,54,63,70` | `SetNpcTimer` | `(nNpcIndex, 900*18)` — không đường dẫn | — | — | — |
| `settings\npcs.txt:676,876-880` | *(cột script NPC)* | `\script\npclevelscript\npc_fenglingdubei.lua` | **KHÔNG** | A+B | **CÓ** |
| `settings\npcs.txt:709,710,726,886-890,1411,1412` | *(cột script NPC)* | `\script\npclevelscript\npc_fenglingdunan.lua` | **KHÔNG** | A+B | **CÓ** |
| `script\global\logout.lua:9` | `Include` | `\script\task\tollgate\messenger\messenger_lievegame.lua` | KHÔNG | A | CHƯA |

### 4.1 `\script\global\tamhiep\callbossdeathmini.lua` — LỖ THẬT

Tìm toàn ổ `D:\ServerLinux` (`find -iname "*tamhiep*"` và `-iname "*callbossdeath*"`):
- **không có thư mục `script\global\tamhiep`** ở bất kỳ gốc nào;
- chỉ có tệp gần giống `script\misc\boss\callbossdeath.lua` (A và JX1 đều có).

Tuy nhiên `boss.lua` là tệp Việt hoá (`---Script -- By-NguyenKhai---`) và **toàn bộ thân
`thuytacdaulinh()` (dòng 19-24) đã bị chú thích**, nên 6 hàm `bossben1A..3B` hiện **không ai gọi**
⇒ **mã chết, không chặn tiến độ**, nhưng nếu đợt sau bỏ chú thích thì phải thay đường dẫn.
`boss.lua` cũng `Include("\script\global\路人_礼官.lua")` — tệp vòng 1 đã ghi là thiếu.

### 4.2 Vật phẩm "Thủy tặc mật báo" — tham chiếu treo

`settings\item\004\magicscriptTQ.txt:2388` → `\script\item\shuizei_mibao.lua`
và `:5314` → `\script\item\shuizei_mibaonew.lua`.
**Cả hai tệp không tồn tại ở A lẫn B** (`script\item\` không có tệp nào chứa `shuizei`).
Hai dòng này **chỉ có ở biến thể `magicscriptTQ.txt` / `magicscript_old.txt`**, bảng đang dùng
`settings\item\004\magicscript.txt` không có ⇒ **CHƯA XÁC MINH** có ảnh hưởng hay không.

---

## 5. BẢNG 3 — VƯỢT ẢI (`vuotai`)

**65 dòng có lời gọi lúc chạy**; hàm dùng: `CallPlayerFunction`×28, `DynamicExecute`×7,
`SetLogoutRV`×6, `SetDeathScript`×5, `GlobalExecute`×4, `DynamicExecuteByPlayer`×4,
`StartMissionTimer`×3, `StopMissionTimer`×3, `RemoteExecute`×1, `AddTimer`×1,
`SetNpcDeathScript`×1, `OpenMission`×1, `CloseMission`×1.

| Điểm gọi (tệp:dòng) | Hàm gọi | Đường dẫn đích | Trong `closure3`? | Có ở Linux? | Có ở JX1? |
|---|---|---|---|---|---|
| `gateway\…\task\challengeoftime.lua:34` | `GlobalExecute("dwf …")` | `\settings\trigger_challengeoftime.lua` hàm `OnTrigger()` — **ĐIỂM VÀO THẬT, mỗi 60 phút** | **KHÔNG** | A+B (đích) / **G** (nguồn) | **CHƯA** |
| `gateway\…\task\challegeoftime-dailyrank.lua:18,26` | *(Relay API)* | `Ladder_GetLadderInfo(10235,1)` / `Ladder_ClearLadder(10235)` lúc **00:00** | **KHÔNG** | **G** | CHƯA |
| `settings\trigger_challengeoftime.lua:22` | `DynamicExecute` | `\script\missions\challengeoftime\chuangguang30.lua` hàm `ChuangGuan30:AddTime` | CÓ | A | CHƯA |
| `settings\trigger_challengeoftime.lua:5-6` | `Include` | `\settings\trigger_include.lua`, `…\challengeoftime\include.lua` | **KHÔNG** (trigger_include) | A+B | CHƯA |
| `settings\trigger_include.lua:5-6` | `Include` | `…\challengeoftime\chuangguang30.lua`, `include.lua` | CÓ | A | CHƯA |
| `script\activitysys\g_npcdeath.lua:72` | `DynamicExecute` | `\script\missions\challengeoftime\chuangguang30.lua` hàm `:OnNpcDeath` — **móc chết NPC toàn cục** | **KHÔNG** (g_npcdeath ngoài closure3) | A | CHƯA |
| `script\activitysys\g_npcdeath.lua:70,74,81,82,85` | `DynamicExecute` | `tianchimijing\floor4\bossdeath.lua`, `task\metempsychosis\npcdeath_translife_4.lua`, `event\jiefang_jieri\201004\main.lua` + `soldier\main.lua`, **`\script\huoyuedu\worldtop10.lua`** | KHÔNG | A | CHƯA |
| `missions\challengeoftime\include.lua:79,80` | *(hằng số)* | `SCRIPT_NPC_DEATH = "…\npc_death.lua"`, `SCRIPT_PLAYER_DEATH = "…\player_death.lua"`, dùng ở `npc.lua:444` (`SetNpcDeathScript`) và `mission_match.lua:79` (`SetDeathScript`) | CÓ | A | CHƯA |
| `missions\challengeoftime\mission_match.lua:12` | `StartMissionTimer` | `(MISSION_MATCH=22, TIMER_MATCH=41, …)` ⇒ khe **41** = `timer_match.lua` (`timertask.txt:42`) | — | A+B | CHƯA |
| `missions\challengeoftime\include.lua:209,214` | `Start/StopMissionTimer` | `(22, TIMER_BOARD=42)` ⇒ khe **42** = `timer_board.lua` (`timertask.txt:43`) | — | A+B | CHƯA |
| `missions\challengeoftime\include.lua:220,225` | `Start/StopMissionTimer` | `(22, TIMER_CLOSE=43)` ⇒ khe **43** = `timer_close.lua` (`timertask.txt:44`) | — | A+B | CHƯA |
| `missions\challengeoftime\chuangguang30.lua:112` | `SetDeathScript` | chính nó `chuangguang30.lua` | CÓ | A | CHƯA |
| `missions\challengeoftime\chuangguang30.lua:265` | `AddTimer` | `AddTimer(nTimeOut*18, "ChuangGuan30:GameTime", 0)` — **định danh bằng CHUỖI TÊN HÀM** | — | — | — |
| `missions\challengeoftime\doubleexp.lua:15` | `DynamicExecuteByPlayer` | `\script\vng_feature\double_mission_award.lua` → `tbVnX2Award:X2ChallengeOfTime` | **KHÔNG** | A | **CHƯA** |
| `missions\challengeoftime\npc\dragonboat_main.lua:30` | `DynamicExecuteByPlayer` | `\script\vng_lib\bittask_lib.lua` → `tbVNG_BitTask_Lib:getBitTask{nTaskID=3071,nStartBit=25,…}` | CÓ | A | CÓ |
| `missions\challengeoftime\item\chuangguanbaoxiang.lua` | `RemoteExecute` | `\script\event\msg2allworld.lua` | **KHÔNG** | A | CÓ |
| `script\global\station.lua:17` | `Include` | `\script\vng_feature\challengeoftime\npcnhiepthitran.lua` — **điểm nạp NPC báo danh Việt hoá** | **KHÔNG** | A | CHƯA |
| `script\missions\challengeoftime\chuangguang30.lua:216` | *(chuỗi dữ liệu)* | `\settings\maps\liandandong\npc_3.txt` | — | A+B | **CHƯA** |
| `script\missions\challengeoftime\npc.lua:12-18` | *(chuỗi dữ liệu)* | `\settings\maps\challengeoftime\lineup{8,16,20,24,32,40,56}.txt` (7 bảng) | — | A+B | **CHƯA** |

---

## 6. Bảng ĐĂNG KÝ dữ liệu — đường vào mà `Include()` không bao giờ thấy

| Tệp dữ liệu | Dòng | Nội dung | Ý nghĩa cho bản port |
|---|---|---|---|
| `settings\task\missions.txt` | `:16` | `15  \script\missions\fengling_ferry\mission.lua` | **MISSION ID 15** — khớp `fld_head.lua:10 MISSIONID = 15` |
| `settings\task\missions.txt` | `:23` | `22  \script\missions\challengeoftime\mission_match.lua` | **MISSION ID 22** — khớp `include.lua:6 MISSION_MATCH = 22` |
| `settings\timertask.txt` | `:29,:30` | khe **28** → `fld_landingtimer.lua`, khe **29** → `fld_smalltimer.lua` | tham số thứ 2 của `StartMissionTimer` là **giá trị ở cột `TASK`**, tra bằng khoá chuỗi (`KTaskFuns.cpp:177-186`: `sprintf(szTaskId,"%d",id)` → `KTabFile::GetString(szTaskId,"SCRIPT")`), **không** phải chỉ số dòng — bảng Linux dày nên khe N nằm ở dòng N+1, còn `TimerTask.txt` của JX1 thưa và **không theo thứ tự** (khe 53 ở dòng 31)  `[đã sửa theo đối chất]` |
| `settings\timertask.txt` | `:42,:43,:44` | khe **41** → `timer_match.lua`, **42** → `timer_board.lua`, **43** → `timer_close.lua` | như trên |
| `settings\maplist.ini` | `:2926,2935,2944` | `337/338/339_name=Bến thuyền 1/2/3` (không có `NewWorldScript`) | 3 bản đồ thuyền Phong Lăng Độ |
| `settings\maplist.ini` | `:3340,3350,3360` | `389/392/395_NewWorldScript=\script\task\tollgate\messenger\newworld.lua` | móc "vào bản đồ" của hệ Tín Sứ dùng chung |
| `settings\maplist.ini` | `:6355,6356` | `995_name=Mật Phòng cửa ải`, `995_NewWorldScript=\script\missions\basemission\mapscript.lua` | bản đồ của bản Việt hoá satthu §3.1 |
| `settings\npcs.txt` | **16 dòng** (bei 6 · nan 10, đúng bằng danh sách ở §4)  `[đã sửa theo đối chất]` | cột script `\script\npclevelscript\npc_fenglingdu{bei,nan}.lua` | script cấp/rơi đồ của quái Phong Lăng Độ |
| `settings\skills.txt` | `:549` | kỹ năng 547 "Sát thủ - Vòng tròn miễn dịch" → `\script\skill\npc\killerbossmianyi.lua` | lá chắn của boss sát thủ (JX1 **đã có** tệp này) |
| `settings\task\taskguide.txt` | `:9` | `8  Nhiệm vụ Sát thủ  0  \UI\taskui_killer.lua  showkillertaskdesc` | mục F11 phía **client** |
| `Patch\ui\taskui_killer.lua` | `:4` | `KILLER = "\settings\task\tollgate\killer\killer.txt"` | bảng 14.033 B, **A và B đều có**, JX1 **chưa có** |
| `settings\item\004\magicscript.txt` | `:401,:402,:2359,:3349` | `killer_token.lua`, `killer_sword.lua`, `task\tollgate\killer\shashou_mibao.lua`, `challengeoftime\item\chuangguanbaoxiang.lua` | script vật phẩm — nạp qua bảng item, không qua `Include` |
| `gateway\…\relaysetting\task\tasklist.ini` | `:209,:312,:318` | `Task_52/84/86` | sổ đăng ký lịch (§2) |

### 6.1 Khe trống phía JX1 (đã đo)

- `bin\server\settings\task\missions.txt`: khe **15** và **22** hiện đều là
  `\script\missions\mission_trong.lua` (giữ chỗ rỗng) ⇒ **dùng lại đúng số, không phải đánh số lại**.
- `bin\server\settings\TimerTask.txt`: các khe đang dùng là 1-10, 12-18, 20, 21, 50-55, 61, 62,
  65-70, 75-77 ⇒ khe **28, 29, 41, 42, 43** đều **TRỐNG** ⇒ giữ nguyên số của bản Linux.

---

## 7. Bao đóng đầy đủ — số tệp TĂNG THÊM

`closure3_runtime.json` (định dạng giống `closure3.json`, thêm 5 trường
`root` / `how` / `tier` / `in_closure3` / `sites`).

| Tính năng | `closure3.json` | Bao đóng đầy đủ | **Tăng thêm** | Tăng thuộc **tầng LÕI** | Tăng thuộc tầng thư viện |
|---|---:|---:|---:|---:|---:|
| satthu | 67 | **220** | **+153** | **9** | 144 |
| phonglangdo | 82 | **202** | **+120** | **5**  `[đã sửa theo đối chất]` | 115 |
| vuotai | 103 | **214** | **+111** | **8** | 103 |
| **Cộng** | 252 | **636** | **+384** | **22 lượt / 21 tệp**  `[đã sửa theo đối chất]` | 362 |

Phân rã cách phát hiện tệp mới:

| Cách | satthu | phonglangdo | vuotai |
|---|---:|---:|---:|
| `seed` (thư mục lõi bổ sung: `global\thanh\npc`, `npclevelscript`, `settings\trigger_*`, `vng_feature\challengeoftime`, task Relay) | 6 | 3 | 5 |
| `runtime` (cạnh `DynamicExecute*` / `SetNpc*Script` / `RemoteExecute` / `GlobalExecute`) | 3 | 7 | 5 |
| `data` (chuỗi `.lua` nằm trong bảng dữ liệu, không có tên hàm trên cùng dòng) | 28 | 25 | 27 |
| `include2` (Include cấp 2 của các tệp vừa tìm được) | 107 | 85 | 74 |
| `include` (Include cấp 1 của các tệp seed mới — **hạng mục bị bỏ quên ở bản đầu**  `[đã sửa theo đối chất]`) | 9 | 0 | 0 |
| **Cộng** (phải bằng cột "Tăng thêm") | **153** | **120** | **111** |

> ⚠️ **Đọc con số cho đúng.** **362**/384 lượt tệp "tăng thêm"  `[đã sửa theo đối chất]` đến từ **một nguồn duy nhất**:
> `script\script_protocol\protocol_def_gs.lua` — bảng phân phối gói tin liệt kê **26 tệp xử lý**
> của **các hệ khác** (bingo, flipcard, petsys, thời trang, exchangeshop, achievement…),
> mỗi tệp lại kéo theo Include riêng. Tệp này nằm sẵn trong `closure3.json` (do một thư viện
> chung `Include` vào), nên bao đóng "đúng về mặt hình thức" nhưng **không phải khối lượng của
> 3 tính năng**. Khi lập kế hoạch port hãy dùng cột **"tầng LÕI" = 22 lượt / 21 tệp phân biệt**  `[đã sửa theo đối chất]`.

### 7.1 Trong 21 tệp lõi mới, phía JX1 có gì

| Tệp | JX1 |
|---|---|
| `script\global\thanh\npc\{add_npc,bosssatthuhead,bosssatthudeath,npcdeath,npc_hotro,npc_chuyensinh}.lua` | ❌ chưa có (6) |
| `script\huoyuedu\huoyuedu.lua` (satthu + phonglangdo dùng chung) | ❌ chưa có |
| `script\task\tollgate\messenger\qianbaoku\90\{tureboss90,turebug90}.lua` | ✅ đã có (đợt Tín Sứ 21/08) |
| `script\npclevelscript\npc_fenglingdu{bei,nan}.lua` | ✅ đã có |
| `script\global\tamhiep\callbossdeathmini.lua` | ⛔ không có ở đâu cả |
| `gateway\s3relay\relaysetting\task\{fengling_ferry,challengeoftime,challegeoftime-dailyrank}.lua` | ❌ (phải chuyển thành mục `timerserver.lua`) |
| `settings\{trigger_challengeoftime,trigger_include}.lua` | ❌ chưa có |
| `script\vng_feature\challengeoftime\npcnhiepthitran.lua` | ❌ chưa có |
| `script\vng_feature\double_mission_award.lua` | ❌ chưa có |
| `script\activitysys\config\41\extend.lua` | ❌ chưa có |
| `script\event\msg2allworld.lua` | ✅ đã có |

---

## 8. 10 tệp/lớp quan trọng nhất bị sót

| # | Tệp / lớp | Vì sao quan trọng |
|---|---|---|
| 1 | **Cả gốc `D:\ServerLinux\gateway\s3relay\relaysetting\`** (`tasklist.ini` + 227 tệp task mức 1) | Lớp lập lịch. Không có nó thì **Vượt Ải và Phong Lăng Độ không bao giờ tự khởi động** |
| 2 | `…\relaysetting\task\fengling_ferry.lua` (`Task_86`) | Điểm vào Phong Lăng Độ: mỗi 60′ gọi `fldmap_boat1.lua : fenglingdu_main()` |
| 3 | `…\relaysetting\task\challengeoftime.lua` (`Task_52`) | Điểm vào Vượt Ải: mỗi 60′ gọi `\settings\trigger_challengeoftime.lua : OnTrigger()`, kèm câu thông báo báo danh 10′ |
| 4 | `…\relaysetting\task\challegeoftime-dailyrank.lua` (`Task_84`) | 00:00 hằng ngày: công bố đội nhanh nhất rồi **`Ladder_ClearLadder(10235)`** — vế mà vòng 1 quy nhầm hết cho `rank_perday.lua` |
| 5 | `script\global\thanh\npc\add_npc.lua` + `bosssatthuhead.lua` | **Bản Việt hoá song song của săn boss sát thủ**: 160 boss trên bản đồ 995, dùng đúng 8 bảng `bosstask_lev20..90.ini` |
| 6 | `script\huoyuedu\huoyuedu.lua` (+ `award.lua`, `worldtop10.lua`) | **Cả satthu lẫn phonglangdo** đều `DynamicExecuteByPlayer` vào đây để cộng "độ hoạt động" (`"shashourenwu"`, `"fenglingdu"`). JX1 chưa có |
| 7 | `script\global\autoexec.lua:158` `add_killertasknpc(addkillertasknpc)` | Điểm vào duy nhất sinh 160 boss sát thủ lúc khởi động; `autoexec.lua` **không nằm trong `closure3.json`** |
| 8 | `script\activitysys\npcfunlib.lua:147` | Hệ hoạt động **so khớp chuỗi** `"\script\task\tollgate\killer\kill_level.lua"` để nhận diện boss ⇒ **đổi đường dẫn khi port là hỏng đếm hoạt động** |
| 9 | `script\activitysys\g_npcdeath.lua:72` + `script\global\station.lua:17` + `script\global\logout.lua:9` | 3 móc toàn cục nạp phần Vượt Ải / NPC báo danh / rời game của Tín Sứ; cả 3 đều ngoài `closure3.json` |
| 10 | `settings\timertask.txt` + `settings\task\missions.txt` + `settings\maplist.ini` | Bảng đăng khe: mission **15** & **22**, timer **28/29/41/42/43**. Không có bảng này thì `StartMissionTimer(15,29,…)` là con số vô nghĩa |

*(ngoài rìa)* `script\global\tamhiep\callbossdeathmini.lua` — tham chiếu treo, không tồn tại ở bất kỳ
gốc nào; hiện vô hại vì `boss.lua` đã bị chú thích hết.

---

## 9. Dương tính giả / điều cần bỏ qua

| Chuỗi bị bắt | Thật ra là |
|---|---|
| `script\Miss Include tong_header.lua` | `activitysys\playerfunlib.lua:126` viết `error("Miss Include tong_header.lua")` — chuỗi **thông báo lỗi**, không phải đường dẫn |
| 3.078 dòng trong `killbosshead.lua` trỏ `messenger\fengzhiqi\*`, `shanshenmiao\*`, `qianbaoku\{6079,8089}\*` | **đã bị chú thích bằng `--`** (đo: **3.078 lượt chú thích / 178 lượt sống**). Chỉ 2 bảng còn sống: `AddNpc_turesureboss`→`tureboss90.lua` (9 dòng, `:3380-3388`, có ghi chú `--只保留9个` = "chỉ giữ 9") và `AddNpc_turesurebug`→`turebug90.lua` (9 dòng) |
| `script\mission\boss\bigboss.lua` (số ít `mission`) | **KHÔNG phải lỗi gõ**: tệp thật, 1.693 B, tồn tại song song với `script\missions\boss\bigboss.lua` (13.447 B). `RemoteExecute` ở `missions\boss\bigboss.lua:172` cố tình gọi bản số ít (phía Relay) |
| 26 tệp `*_gs.lua` từ `script_protocol\protocol_def_gs.lua` | Bảng phân phối gói tin của **toàn máy chủ**, không thuộc 3 tính năng |
| `settings\skills.txt:549` "Sát thủ - Vòng tròn miễn dịch" | Đúng là của boss sát thủ (`killerbossmianyi.lua`), **không** phải hệ "truy sát" PvP |
| `gateway\lang\vn\stringtable_relay.txt` `KKillerData::…` "nhiệm vụ truy sát" | Hệ **truy sát PvP** (追杀) của Relay — **khác** hệ "sát thủ" (杀手任务). **CHƯA XÁC MINH** có liên quan hay không, đừng gộp |

---

## 10. Việc còn treo cho vòng sau

1. **Chưa quét** `gateway\` cho các đường dẫn `.lua` khác ngoài 3 task đã nêu — 1.189 tệp,
   mới soi phần `relaysetting\task\`.
2. **Chưa xác minh** máy chủ VN thật gọi `add_npc_thanh()` ở đâu (chỉ thấy `Include`, không thấy lời gọi).
3. **Chưa xác minh** `magicscriptTQ.txt` có được nạp không ⇒ chưa kết luận được về
   `shuizei_mibao.lua` / `shuizei_mibaonew.lua`.
4. `settings\maps\challengeoftime\lineup*.txt` (7 bảng) và `settings\maps\liandandong\npc_3.txt`
   có ở A+B, **chưa có ở JX1** — cần đưa vào danh sách dữ liệu phải chép
   (bổ sung cho `09_bang_toado_patch.md`).

---

## ĐỐI CHẤT (tác tử độc lập)

Người kiểm chứng: tác tử độc lập, **không** phải người viết báo cáo 11. Nguyên tắc: mặc định coi
mọi khẳng định là SAI cho tới khi tệp gốc / mã máy chủ chứng minh ngược lại. Đã kiểm **33 khẳng định**
cụ thể; **27 ĐÚNG**, **6 SAI** (đã sửa trong thân bài, đánh dấu `[đã sửa theo đối chất]`).
Không sửa gì trong `Sources`, `bin`, `E:\SourceTuanLe`; không đụng `00_`..`09_`.

| # | Khẳng định | Bằng chứng gốc (đo lại) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | Có **gốc thứ ba** `D:\ServerLinux\gateway`; `s3relay` **1.189** tệp, `relaysetting` **378**, `relaysetting\task` **227** mức 1 / **354** tổng | `find -type f`: gateway 1.294 · s3relay **1.189** · relaysetting **378** · task maxdepth1 **227** · task tổng **354** | **ĐÚNG** (cả 4 số) | — |
| 2 | `tasklist.ini` `[List] Count=128`; chỉ tệp có tên trong đó mới chạy | `tasklist.ini:1-2` `[List]`/`Count=128`; đếm `^\[Task_` = **128** khối, `Task_0..Task_127` | **ĐÚNG** | — |
| 3 | `Task_52`:209 `challengeoftime.lua` · `Task_84`:312 `challegeoftime-dailyrank.lua` · `Task_86`:318 `fengling_ferry.lua` | `sed -n '205,215p;309,320p' tasklist.ini` — cả 3 dòng `TaskFile=` đúng số dòng, đúng tên (kể cả lỗi gõ "challegeoftime") | **ĐÚNG** | — |
| 4 | `fengling_ferry.lua:21` `GlobalExecute("dwf …fldmap_boat1.lua fenglingdu_main()")`, mỗi 60′ | Giải mã tệp: `:13 TaskInterval(60)`, `:10 TaskTime(nStartHour,0)`, `:21` đúng nguyên văn. Đích có thật: `fldmap_boat1.lua:4 function fenglingdu_main()` | **ĐÚNG** | — |
| 5 | `challengeoftime.lua:34` `GlobalExecute("dwf \settings\trigger_challengeoftime.lua OnTrigger()")` + `:36` `AddLocalCountNews(...,2)`, mỗi 60′ | Đúng nguyên văn `:20 TaskInterval(60)`, `:23 TaskTime(h,m)`, `:34`, `:36`. Đích: `trigger_challengeoftime.lua:8 function OnTrigger()`, và **dòng 1 của chính tệp đó ghi** `闯关活动触发器，由Relay每小时触发` ("bộ kích hoạt Vượt Ải, do Relay kích mỗi giờ") | **ĐÚNG** (có chứng cứ độc lập từ chính tệp đích) | — |
| 6 | `challegeoftime-dailyrank.lua`: `TaskTime(0,0)`+`TaskInterval(1440)`, `:18 Ladder_GetLadderInfo(10235,1)`, `:26 Ladder_ClearLadder(10235)` | Giải mã tệp: `:7`, `:10`, `:18`, `:26` — khớp từng dòng | **ĐÚNG** | — |
| 7 | Các bản `_HHMM` (12+12) và 2 thư mục Việt hoá `phonglangdo\`(25)/`vuotai\`(2) **không đăng ký ⇒ không chạy** | `grep -n challengeoftime\|fengling tasklist.ini` chỉ ra **2 dòng** (209, 318); `ls phonglangdo` = 25, `ls vuotai` = 2 | **ĐÚNG** (bảng liệt kê thiếu 1 biến thể `challengeoftime_ok.lua`, cũng không đăng ký — kết luận không đổi) | — |
| 8 | Bảng đếm 21 hàm nạp script trên `server1\script` (708/259/189/…/0/0/0) | Chạy lại đúng lệnh `grep -r -a -o "\bTÊN("` cho cả 21 tên: **21/21 số khớp tuyệt đối**, kể cả 6 tên = 0 | **ĐÚNG** | — |
| 9 | JX1 đã đăng ký sẵn `StartMissionTimer`:14750 · `StopMissionTimer`:14751 · `GlobalExecute`:14882 · `LoadScript`:14899 · `DynamicExecute`:14900 · `SetNpcTimer`:14555 · `AddTimer`:15254 · `OpenGlbMission`:15119 · `StartGlbMSTimer`:15120 · `RemoteExecute`:15258 · `Ladder_NewLadder`:15085 | `grep -n` trong `ScriptFuns.cpp` (15.348 dòng): **11/11 số dòng khớp chính xác** | **ĐÚNG** | — |
| 10 | "**Không có hàm nạp script nào phải viết mới**" | Kiểm thân hàm, không chỉ tên: `KTongJX2.cpp:4055 LuaJX2_GlobalExecute` xử lý **cả hai tiền tố `dw ` và `dwf <path> <stmt>`** (`:4064-4086`) — đúng thứ mà 3 task Relay dùng; `KJx2SharedStore.cpp:105 LuaJX2_RemoteExecute` (~40 dòng thật); `KJx2League.cpp:1409 LuaWllsLoadScript`. **Không cái nào là stub** | **ĐÚNG** (đã kiểm mức thân hàm) | — |
| 11 | ELF `StartMissionTimer` `0x08138840`, `StopMissionTimer` `0x08134720` | `jx_linux_y.luamap.full.txt`: cả hai địa chỉ khớp. Bổ sung: `RemoteExecute 0x08100740`, `LoadScript 0x08105020`, `Ladder_ClearLadder 0x08159A80`, `Ladder_GetLadderInfo 0x08159BD0`; **`GlobalExecute` KHÔNG có trong luamap của GS** — đúng với kiến trúc "hàm phía Relay" | **ĐÚNG** | — |
| 12 | §3.1 `add_npc.lua` 23.628 B, `tbNpcSatThu` **160 boss** trên bản đồ **995**, `random(1590,1625)/random(3180,3250)`, `add_boss():211-232` dùng `AddNpcEx`+`SetNpcDeathScript`+`SetNpcParam` | `ls -la` = 23.628 B; đếm dòng chứa `bosssatthuhead` = **160**; mục cuối `:200` có cột 10 = 160, cột 3 = 995; `:211-232` đúng khung hàm, `:225 AddNpcEx`, `:228 SetNpcDeathScript`, `:229 SetNpcParam` | **ĐÚNG** (từng chi tiết) | — |
| 13 | `add_npc_thanh()` (`:203`) **không tệp nào trong cây `script\` gọi**; điểm nạp `ex_lib_function.lua:77-78` | `grep -rn "add_npc_thanh\|add_boss("` toàn `server1\script` → chỉ 3 hit, đều nằm trong chính `add_npc.lua`; `ex_lib_function.lua:77,78` đúng 2 dòng `Include` | **ĐÚNG** | — |
| 14 | `bosssatthuhead.lua`: `OnDeath`→`kill_level20()`, 8 bậc theo `GetNpcParam(idx,1)`, 8 bảng `bosstask_lev20..90.ini`, exp **15.000 → 2.000.000** | Đọc trọn 60 dòng: `:4-6` OnDeath, `:12-35` đúng 8 bậc 1-20…141-160, `:13-34` đúng 8 tên bảng, `:43 AddOwnExp(15000)` … `:57 AddOwnExp(2000000)` | **ĐÚNG** | — |
| 15 | §3.2 đính chính: 8 tệp `bosstask_lev20..90.ini` có ở **cả A lẫn B** (kích thước 6233/6237/6252/6252/6255/6251/6262/6214), JX1 chưa có | `ls -la` A và B: **8/8 tệp, 8/8 kích thước khớp cả hai bên**; JX1 `settings\droprate\boss\` chỉ có `helianpiaodroprate.ini`. Câu bị đính chính có thật ở `00_ghichu_dieuphoi.md:61` và nằm trong đoạn "nằm ở Patch" | **ĐÚNG** (đính chính hợp lệ) | — |
| 16 | 5 móc toàn cục: `autoexec.lua:158`, `npcfunlib.lua:147`, `g_npcdeath.lua:72` (+`:70,74,81,82,85`), `station.lua:17`, `logout.lua:9` | Giải mã và đọc từng dòng: **5/5 đúng số dòng, đúng nguyên văn**; riêng `g_npcdeath.lua` cả 6 số dòng phụ đều khớp | **ĐÚNG** | — |
| 17 | `lib_killlevel.lua:29` → `huoyuedu:AddHuoYueDu("shashourenwu")`; `fld_smalltimer.lua:82` → `…("fenglingdu")` | Đúng nguyên văn cả hai dòng; `script\huoyuedu\` có thật 3 tệp `huoyuedu.lua` 7.712 B · `award.lua` · `worldtop10.lua` | **ĐÚNG** | — |
| 18 | `killbosshead.lua`: **3.078** chú thích / **178** sống, chia 160 + 9 + 9 | Đếm theo đúng thước đo (dòng **chứa `.lua`**): tổng 3.256 = **3.078** dòng `^\s*--` + **178** dòng sống; `uniq -c` trên 178 dòng sống ra đúng **160** `kill_level.lua` · **9** `tureboss90.lua` · **9** `turebug90.lua` | **ĐÚNG** (khớp tới từng đơn vị) | — |
| 19 | `\script\global\tamhiep\callbossdeathmini.lua` **không tồn tại ở bất kỳ gốc nào**; `boss.lua` là mã chết | `find D:\ServerLinux -iname "*callbossdeath*"` → chỉ `server1\script\misc\boss\callbossdeath.lua`; `-type d -iname tamhiep` → **rỗng**. `boss.lua:18-25` thân `thuytacdaulinh()` chú thích hết; `grep -rn` toàn cây: **không tệp nào gọi `thuytacdaulinh` và không tệp nào `Include` `fengling_ferry\boss.lua`** ⇒ chết hai lớp | **ĐÚNG** (mạnh hơn báo cáo: cả tệp cũng không được Include) | — |
| 20 | `script\mission\boss\bigboss.lua` (số ít) là tệp thật **1.693 B**, song song bản `missions\` **13.447 B** | `ls -la` hai đường dẫn: 1.693 B và 13.447 B | **ĐÚNG** | — |
| 21 | Khe timer Linux: **28**→`fld_landingtimer.lua`, **29**→`fld_smalltimer.lua`, **41/42/43**→`timer_match/board/close.lua`; mission **15**/**22** | `timertask.txt` dòng 29,30,42,43,44 và `missions.txt` dòng 16, 23 — khớp toàn bộ; đối chiếu ngược `fld_head.lua:10 MISSIONID=15`, `challengeoftime\include.lua:6-9` = 22/41/42/43 | **ĐÚNG** (số khe) | — |
| 22 | "**tham số thứ 2 của `StartMissionTimer` là chỉ số dòng trong bảng này**" | `KTaskFuns.cpp:177-186 GetTimerTaskScript()`: `sprintf(szTaskId,"%d",usTimerTaskId)` rồi `m_TimerTaskTab.GetString(szTaskId,"SCRIPT",…)` ⇒ tra theo **giá trị cột `TASK`**, không theo dòng. Bảng Linux dày nên khe 28 nằm ở **dòng 29** (đã lệch 1); `TimerTask.txt` của JX1 **thưa và trái thứ tự** (khe 53 ở dòng 31, khe 20/21 ở dòng 35/36) | **SAI** | "là **giá trị ở cột `TASK`**, tra bằng khoá chuỗi (`KTaskFuns.cpp:177-186`), không phải chỉ số dòng" |
| 23 | JX1: khe mission **15** và **22** đều `mission_trong.lua`; `TimerTask.txt` dùng 1-10, 12-18, 20, 21, 50-55, 61, 62, 65-70, 75-77 ⇒ 28/29/41/42/43 **trống** | `E:\…\bin\server\settings\task\missions.txt` dòng 16 và 23 = `mission_trong.lua`; đọc trọn `TimerTask.txt` (37 mục): **danh sách khe khớp 100%**, không có 28/29/41/42/43 | **ĐÚNG** (lưu ý: đường dẫn phải ghi rõ cây **E:\SourceTuanLe\…\bin\server**, vì `D:\GAMEDEVNEW\bin\server\settings\` **không có** hai tệp này) | — |
| 24 | `maplist.ini` `:2926,2935,2944` = `337/338/339_name=Bến thuyền 1/2/3`; `:3340,3350,3360` = `389/392/395_NewWorldScript=…messenger\newworld.lua`; `:6355,6356` = `995_name=Mật Phòng cửa ải` + `NewWorldScript=…basemission\mapscript.lua` | `sed -n` từng dòng: **8/8 dòng khớp nguyên văn** | **ĐÚNG** | — |
| 25 | §6: `settings\npcs.txt` — "**13 dòng**" cột script `npc_fenglingdu{bei,nan}.lua` | `grep -c` = **16** dòng (bei **6**: 676, 876-880; nan **10**: 709, 710, 726, 886-890, 1411, 1412) — đúng bằng danh sách §4 của chính báo cáo, tức §6 tự mâu thuẫn với §4 | **SAI** | **16 dòng** (bei 6 · nan 10) |
| 26 | Bảng dữ liệu: `magicscript.txt:401,402,2359,3349`; `skills.txt:549` (kỹ năng **547**→`killerbossmianyi.lua`); `taskguide.txt:9`; `taskui_killer.lua:4`; `killer.txt` **14.033 B** ở A+B, JX1 chưa có | `sed -n` từng dòng: 401 `killer_token.lua` · 402 `killer_sword.lua` · 2359 `shashou_mibao.lua` · 3349 `chuangguanbaoxiang.lua`; `skills.txt:549` id **547** "Sát thủ - Vòng tròn miễn dịch" và **có** chuỗi `killerbossmianyi.lua` (JX1 **đã có** tệp, 1.004 B); `taskguide.txt:9` = `8 Nhiệm vụ Sát thủ 0 \UI\taskui_killer.lua showkillertaskdesc`; `killer.txt` 14.033 B ở **cả A và B**, JX1 **không có** | **ĐÚNG** (toàn bộ) | — |
| 27 | §4.2 `magicscriptTQ.txt:2388,5314` trỏ `shuizei_mibao.lua`/`shuizei_mibaonew.lua`, **cả hai tệp không tồn tại ở A lẫn B** | `sed -n '2388p;5314p'` khớp; `find … -iname "*shuizei*"` trên `server1\script\item` và toàn bộ `Patch` → **0 kết quả** | **ĐÚNG** | — |
| 28 | §10.4 `maps\challengeoftime\lineup{8,16,20,24,32,40,56}.txt` (7 bảng) + `maps\liandandong\npc_3.txt` có ở A+B, **chưa có ở JX1** | `ls` A và B: đủ **7/7** tệp lineup ở cả hai + `npc_3.txt` ở cả hai; JX1 **không có cả hai thư mục** | **ĐÚNG** | — |
| 29 | `closure3.json` = 67/82/103; bao đóng đầy đủ 220/202/214; mới +153/+120/+111 | Nạp JSON: `closure3.json` **67/82/103**; `closure3_runtime.json` **220/202/214**; `in_closure3==False` = **153/120/111** | **ĐÚNG** | — |
| 30 | "Tăng thuộc **tầng LÕI**: **9 · 4 · 8**, Cộng **21**" | Đếm trên chính `closure3_runtime.json` (`tier=='core' and not in_closure3`): satthu **9**, phonglangdo **5**, vuotai **8** = **22 lượt**. Con số 21 là số **tệp phân biệt** (vì `script\huoyuedu\huoyuedu.lua` là lõi-mới của **cả satthu lẫn phonglangdo**) — đúng bằng 21 dòng của bảng §7.1. Cột phonglangdo "4" mâu thuẫn dữ liệu của chính tác tử (mục thứ 5 là `script\global\tamhiep\callbossdeathmini.lua`) | **SAI** | **9 · 5 · 8 = 22 lượt / 21 tệp phân biệt**; cột "lib" phonglangdo là **115**, cộng **362** |
| 31 | §7 bảng "Phân rã cách phát hiện tệp mới": seed 6/3/5 · runtime 3/7/5 · data 28/25/27 · include2 107/85/74 | Đếm lại theo trường `how` (chỉ tệp mới): satthu `{include2:107, data:28, **include:9**, seed:6, runtime:3}` — 4 hàng của bảng chỉ cộng ra **144 ≠ 153**; thiếu hẳn hạng mục `include` (9 tệp, chỉ satthu). phonglangdo 3+7+25+85=**120** ✓ và vuotai 5+5+27+74=**111** ✓ | **SAI** (riêng cột satthu) | Thêm hàng `include` = **9 · 0 · 0**, và hàng **Cộng 153 · 120 · 111** để bảng tự kiểm |
| 32 | "Số điểm gọi lúc chạy: satthu **26** · phonglangdo 36 · vuotai 65" | `_rt_sites.json` của chính tác tử: satthu **28** · phonglangdo 36 · vuotai 65. Và bảng hàm ở §3 (`SetNpcScript`×8 + `DynamicExecute`×11 + `dofile`×4 + `OpenGlbMission`×2 + `OpenMission`×1 + `SetNpcDeathScript`×1 + `DynamicExecuteByPlayer`×1) cộng ra đúng **28**, tức tiêu đề tự mâu thuẫn với bảng ngay dưới. Số "cạnh có `.lua` tường minh" 15/14/13 thì **đúng** | **SAI** (riêng satthu) | **28** dòng |
| 33 | §2.3 "vai trò [lập lịch Relay] do `timerserver.lua` + `settings\TimerTask.txt` + `script\timertask\task01..10.lua` đảm nhiệm" | Hai cơ chế khác nhau bị gộp: (a) đồng hồ treo tường = `CoreServerShell.cpp:1139` `g_GetScript("\script\timerserver.lua")` → `:1170 CallFunction("RunTime")`; (b) `TimerTask.txt` + `timertask\task01..10.lua` = bảng khe **mission/player timer** do `KTaskFuns.cpp:108/127/177-186` tra — chính §6 của báo cáo mô tả đúng như vậy. Ngoài ra `timerserver.lua:79,80` **đã có sẵn** `-- sukien_vuotai` / `-- sukien_phonglangdo` (đang chú thích) | **SAI** | Tách hai cơ chế; ghi rõ chỗ đáp `timerserver.lua:15,16` và `:79,:80` đã có sẵn |

**Ghi nhận thêm (ĐÚNG nhưng cần nói rõ phạm vi)**: bảng đếm §1 chỉ tính `server1\script`.
Đếm lại trên chính gốc thứ ba mà báo cáo đề cao: `gateway` có **315** lượt `GlobalExecute(` và **106** lượt
`RemoteExecute(` (so với 23 và 152 ở `server1`), còn `Patch` có **0** cả hai. Mới soi **3/315** cạnh.

---

## Bỏ sót của chính vòng 2

### A. 🔴🔴 JX1 **ĐÃ CÓ SẴN** bản Việt của cả ba tính năng — báo cáo 11 không hề nhắc

`grep -ci "tinhnang|vuot_ai|boss_satthu"` trên `11_bosung_phuthuoc_luc_chay.md` = **0**, trong khi
máy chủ JX1 đang chạy có:

| Thư mục JX1 (`E:\…\TESTLOFFF_ONLINE\bin\server\script\tinhnang\`) | Nội dung |
|---|---|
| `phonglangdo\` (**8 tệp**) | `lib_phonglangdo.lua` 9.995 B (sửa 17/12/2025) · `thuyenphu.lua` · `thuyenphubac.lua` · `bossthuytacdaulinhpld.lua` · `quaipld.lua` · `dietrenthuyen.lua` · `drop.lua` · `logout.lua` |
| `vuot_ai\` (**6 tệp**) | `lib_vuotai.lua` 8.869 B · `sugiasatthu.lua` 13.348 B · `die_boss.lua` · `die_normal.lua` · `ondeath.lua` · `drop.lua` |
| `boss_satthu\` (**3 tệp**) | `lib_boss_st.lua` · `death.lua` · `drop.lua` |

Và chúng **đã được nối vào bộ lập lịch**: `timerserver.lua:15,16` `Include` hai thư viện trên;
`timerserver.lua:79,80` là `-- sukien_vuotai(nHr,nMi)` / `-- sukien_phonglangdo(nHr,nMi)` (chú thích).
`lib_vuotai.lua:12-31` cho thấy **cùng một khái niệm** với `challengeoftime`: `MSTIME_VUOT_AI_BD = 10`
(báo danh 10 phút — trùng đúng câu thông báo của `challengeoftime.lua:35`), đội trưởng, boss cuối,
bảng giờ `TAB_TIME_VUOT_AI = {13:00, 15:00, 17:00, 19:00, 23:00}`.

**Hệ quả**: cột "Có ở JX1?" của báo cáo 11 ghi **CHƯA** ở gần như mọi dòng là đúng *ở mức tệp*,
nhưng **sai ở mức tính năng** — và §2.3 dựng kế hoạch "chuyển 3 task Relay thành mục trong
`timerserver.lua`" mà không biết chỗ đáp đã có sẵn, thậm chí đã có bản cài đặt VN đang nằm đó.
Đây là **bước lùi so với vòng 1**: `06_phia_jx1.md:168,169,494,495,499,500,754,779-783` đã mô tả
đầy đủ ba thư mục này (kể cả việc `startgame.lua:100` vẫn gọi `addnpcphonglangdo()` nên 6 NPC
Thuyền phu vẫn spawn dù lịch bị tắt). Báo cáo 11 không dẫn lại một dòng nào.

### B. 🔴 `yandibaozang` (炎帝宝藏) — task Relay **ĐANG BẬT**, là bản song sinh của Vượt Ải, và **tiếng Việt cũng gọi là "vượt ải"**

Báo cáo 11 chỉ nhắc `yandibaozang.lua` đúng một lần, ở §2.2, như nguồn của **một dòng mã chết**.
Thực tế:

- `tasklist.ini:351 TaskFile=yandibaozang.lua` ⇒ **có đăng ký, đang chạy** (khác hẳn 24 tệp `_HHMM`).
- `yandibaozang.lua:51` (dòng **sống**, ngay dưới dòng chết mà báo cáo trích):
  `GlobalExecute("dwf \script\missions\yandibaozang\yandibaozang_trigger.lua YDBZ_OnTrigger()")`
  — **cùng khuôn hệt** điểm vào Vượt Ải, chỉ khác tệp đích.
- `yandibaozang.lua:52` thông báo tiếng Việt: *"Hoạt động **vượt ải** bảo tạng Viêm Đế đã bắt đầu báo danh…
  đến Bình Bình cô nương ở Biện Kinh… thời gian báo danh là 5 phút."* ⇒ **trùng tên gọi tiếng Việt**
  với tính năng đang port. Người đọc bàn giao rất dễ port nhầm hoặc bỏ sót một nửa.
- `script\missions\yandibaozang\` là **bản sao cấu trúc** của `challengeoftime\`:
  `include.lua`, `mission.lua`, `npc.lua`, `npc_death.lua`, `player_death.lua`, `timer_match.lua`,
  `doubleexp.lua`, `trigger_include.lua`, `item\`, `npc\`, cộng thêm `readymap\`, `trap\`, `saizi.lua`.
- Khe đã đăng ký (nên **không** đụng nhau với 22/41/42/43): `head.lua:13-15` `YDBZ_MISSION_MATCH=50`,
  `YDBZ_TIMER_MATCH=87`, `YDBZ_TIMER_FIGHTSTATE=89`; `readymap\include.lua:14-15` = 51 / 88;
  đối chiếu `missions.txt` dòng 51,52 và `timertask.txt` dòng 88,89,90 — khớp.

⇒ Cần một mục **"phân biệt tên gọi"** trong bàn giao: *Vượt Ải* = `challengeoftime` (mission 22),
còn *"vượt ải bảo tạng Viêm Đế"* = `yandibaozang` (mission 50/51) là **tính năng khác**.

### C. Bốn điểm nhỏ hơn

1. **`Ladder_*` phía JX1**: §1 chỉ dẫn `Ladder_NewLadder`:15085. Hai hàm mà `Task_84` thật sự cần
   **cũng đã có**: `Ladder_ClearLadder`:15086 và `Ladder_GetLadderInfo`:15087
   (`KJx2SharedStore.cpp:580,599`) — nên nói ra, vì `06_phia_jx1.md:299-307` khẳng định **S3Relay của JX1
   không có ladder**; tức task này bắt buộc chạy ở **phía GameServer** (`timerserver.lua`), không phải Relay.
2. **Sai hoa/thường có thật**: `station.lua:17` `Include("…\npcNhiepThiTran.lua")` nhưng tệp trên đĩa là
   `npcnhiepthitran.lua`. Báo cáo âm thầm viết thường. Trên hệ tệp phân biệt hoa/thường đây là lỗi nạp —
   **CHƯA XÁC MINH** engine Linux có tự hạ chữ thường hay không (JX1 `LuaJX2_RemoteExecute` **có** hạ
   chữ thường, `KJx2SharedStore.cpp:118-122`).
3. **Lỗi thật trong tệp mà §3.1 khuyên "port rẻ hơn"**: `bosssatthuhead.lua:13-34` truyền `nseries` làm
   tham số cuối của **cả 8** lời gọi `ITEM_DropRateItem`, nhưng `nseries` **không được khai báo ở đâu**
   trong tệp (hệ ngũ hành thật nằm ở `add_npc.lua:216 local nSeries = random(0,4)`, một state khác).
   Port nguyên văn sẽ truyền `nil`.
4. **§2.2 đếm thiếu 1 biến thể**: ngoài 12 tệp `challengeoftime_0100..2300.lua` còn
   `challengeoftime_ok.lua` — cũng không có trong `tasklist.ini`, nên kết luận không đổi, nhưng con số
   "12 tệp" nên sửa thành "13 tệp".
