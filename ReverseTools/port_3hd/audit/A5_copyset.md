# A5 — SOÁT BỘ 101 TỆP ĐÃ CHÉP: xung đột / che khuất / phá hệ cũ

> Phạm vi: `ReverseTools\port_3hd\thicong\b1_manifest.txt` (101 tệp) + hệ quả của chúng lên
> cây JX1 đang sống `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`.
> **Chỉ đọc — không sửa gì.** Mọi phát hiện có `tệp:dòng`.
> Công cụ/kịch bản kiểm: `%TEMP%\claude\…\scratchpad\a5\s1..s14*.py` (chạy lại được).

---

## 0. TỔNG KẾT SỐ XUNG ĐỘT

| Mức | Số | Mã |
|---|---|---|
| **CHẶN** | **4** | C1 C2 C3 C4 |
| **NẶNG** | **9** | N1 … N9 |
| **NHẸ** | **6** | L1 … L6 |
| Đã kiểm → **KHÔNG có vấn đề** | 8 mục | mục 5 |

> 2 lỗi CHẶN mà tổ A2 đã báo (`lenhbaiadmin.lua:107` nhãn menu chứa `/`;
> `hd3_driver.lua:23,92,123` `DynamicExecute` vào `\settings\…`) **KHÔNG đếm lại ở đây**
> — nhưng mục 4.C3-bis bổ sung **bằng chứng log runtime** cho cả hai.

**Bằng chứng mạnh nhất của đợt này:** máy chủ đã **thực sự boot lúc 01:48 25/08 với đủ 101 tệp**
và để lại 3 tệp `ScriptError.log` mới + 160 dòng lỗi trong `ScriptError.log` gốc.
Đó không phải suy đoán — đó là chứng cứ chạy thật.

---

## 1. CÂU HỎI 1 — Include trước im lặng, nay BỖNG RESOLVE (rủi ro hồi quy lớn nhất)

Cách quét: giải escape `\\`→`\` của Lua 4 rồi đối chiếu mọi chuỗi `.lua/.txt/.ini` trong
**toàn cây** (3.859 tệp `script\` + `scriptjx2\` + `settings\`) với 101 đường dẫn mới
(`s3_exact.py`). Kết quả: **7 tệp JX1 CŨ có 10 điểm gọi nay trỏ trúng tệp mới**.

| # | Tệp JX1 CŨ (điểm gọi) | Trỏ tới tệp MỚI | Hàm | Mức |
|---|---|---|---|---|
| a | `script\global\特殊用地\宋金报名点\npc\songjin_shophead.lua:2` | `script\battles\battlehead.lua` | `Include` | **N1** |
| b | `script\startgame\tinhnang\tongkim\songjin_shophead.lua:2` | `script\battles\battlehead.lua` | `Include` | **N1** |
| c | `script\task\newtask\branch\branch_bwsj.lua:2` | `script\battles\battlehead.lua` | `Include` | **N1** |
| d | `script\中原北区\朱仙镇\trap\朱仙镇to宋金战场.lua:4` | `script\battles\battlehead.lua` | `Include` | **N1** |
| e | `script\中原南区\襄阳\襄阳\trap\襄阳to宋金战场.lua:3` | `script\battles\battlehead.lua` | `Include` | **N1** |
| f | `script\global\seasonnpc.lua:105` | `script\huoyuedu\huoyuedu.lua` | `DynamicExecuteByPlayer` | **N2** |
| g | `script\missions\leaguematch\head.lua:678` | `script\huoyuedu\huoyuedu.lua` | `DynamicExecuteByPlayer` | **N2** |
| h | `script\missions\tongcastle\tongcastle.lua:519` | `script\huoyuedu\huoyuedu.lua` | (đã **comment** 23/08) | **L1** |
| i | `script\item\forbiditem.lua:137` | `script\vng_feature\forbiditem\vngforbidspecialitem.lua` | `DynamicExecute` | **L2** |
| j | `scriptjx2\lib\droptemplet.lua:31` | `script\item\battles\songjinskill.lua` | `Include` | **N3** |

Chi tiết ở mục 4.

---

## 2. CÂU HỎI 2 — Trùng tên hàm toàn cục / trùng ScriptID

**2.1 Trùng ScriptID (`g_FileName2Id`, `KFilePath.cpp:442`) — KHÔNG CÓ.**
`s12_id.py` tính lại đúng thuật toán (kể cả `char` có dấu = số âm trên MSVC) cho **2.959 tệp**
nạp lúc boot (`\script\**` + `\scriptjx2\tong_vn\**`): **2.959 ID phân biệt, 0 cặp trùng**.
Trần `MAX_SCRIPT_IN_SET = 5000` (`KSortScript.h:13`) → còn dư 41 %. Đợt này thêm **78 tệp
`.lua` dưới `script\`** (23 tệp còn lại nằm ở `settings\`, không bị nạp lúc boot).

**2.2 Trùng tên hàm toàn cục:** vì mỗi tệp một `Lua_State`, chỉ nguy hiểm ở 10 điểm nối ở mục 1.
Đã dựng 2 bao đóng (`s6_collide.py`) cho từng điểm nối. Kết quả:
* Với 5 điểm nối `battlehead.lua`: bao đóng CŨ của 3 tệp (c/d/e) chỉ có 1–3 tệp → **0 trùng tên**;
  2 tệp (a/b) vốn đã kéo cả `activitysys` nên phần "trùng" chỉ là thư viện chung, **không phải va chạm thật**.
* Điểm nối `forbiditem.lua:137` và `droptemplet.lua:31`: phần trùng cũng chỉ là `lib\common.lua`.
* **Thứ tự nạp có lợi**: mọi `Include` đều nằm ở dòng 2–4 đầu tệp, nên định nghĩa của chính
  tệp JX1 nạp SAU và **đè lại** tệp mới → không có chuyện tệp mới cướp hàm của tệp cũ.

⇒ **Rủi ro thật không nằm ở trùng tên, mà ở KHỐI LƯỢNG mã mới bị kéo vào state** (mục 4.N1)
và ở **tác dụng phụ lúc chạy thân tệp** (mục 3).

**2.3 Lệnh gọi hàm ở CẤP CAO NHẤT trong 101 tệp** (`s9_toplevel.py`) — chạy lúc boot **và**
mỗi lần một state khác `Include`:

| Tệp:dòng | Lệnh |
|---|---|
| `script\missions\challengeoftime\chuangguang30.lua:272-275` | `Unregist()` / `SetForbitItem()` / `Init()` / `RegistAll()` → **vỡ ở 273** (C1) |
| `script\huoyuedu\huoyuedu.lua:251-252` | `LoadTable(...)` + `RegisterEvent()` |
| `script\task\metempsychosis\translife_6.lua:310` | `TransLife6:Init()` |
| `script\task\metempsychosis\task_func.lua:220` | `EventSys:GetType("OnLogin"):Reg(...)` |
| `script\task\tollgate\killer\kill_level.lua:123` | `EventSys:GetType("KillerBoss"):Reg(...)` |
| `script\vng_event\thapnienlenhbai\mainfuc.lua:164` | `tbThapnienLenhbai:LoadFile()` → **bảng không có** (N6) |

---

## 3. BẰNG CHỨNG CHẠY THẬT — máy chủ đã boot 01:48 25/08 với đủ 101 tệp

`GameServer.log` cho thấy 2 lần "Khoi dong hoan tat"; `settings\jx2league.txt` ghi 01:49,
`jx2ladder.txt` ghi 01:59. Các tệp `ScriptError.log` **mới sinh trong đợt này**:

```
script\activitysys\config\41\ScriptError.log      25/08 01:48:53
script\missions\challengeoftime\ScriptError.log   25/08 01:48:55
script\missions\challengeoftime\npc\ScriptError.log 25/08 01:48:55
```

Nội dung nguyên văn:

```
error: attempt to index global `pActivity' (a nil value)
   1:  main of string "?" at line 11
2026/08/25 01:48:53.418  ScriptError 4:[1] (\script\activitysys\config\41\extend.lua)

error: attempt to index global `FORBITMAP_LIST' (a nil value)
   1:  method `SetForbitItem' at line 249 [string "?"]
   2:  main of string "?" at line 273
2026/08/25 01:48:55.655  ScriptError 4:[1] (\script\missions\challengeoftime\chuangguang30.lua)

error: attempt to index global `FORBITMAP_LIST' (a nil value)
   3:  function `Include' [C]
   4:  main of string "?" at line 3        ← \script\missions\challengeoftime\npc\transfer.lua
```

Và trong `ScriptError.log` gốc (thư mục `bin\server`), gom theo ngày:

```
2026/08/25 01:49:50  x160  (\script\task\tollgate\killer\kill_level.lua) cFuncName:(OnRevive)  attempt to call a nil value
2026/08/25 01:51:58  x1    (\script\item\tasklink_goods.lua)  cFuncName:( Phong L?g ? / V??? ?): test)
2026/08/25 01:52:05  x1    (\script\item\lenhbaiadmin.lua)    cFuncName:( Phong L?g ? / V??? ?): test)
```

---

## 4. CHI TIẾT TỪNG PHÁT HIỆN

### C1 — CHẶN — `script\missions\challengeoftime\chuangguang30.lua:249`: **Vượt Ải chết ngay lúc boot**

`FORBITMAP_LIST` được định nghĩa ở **bản Linux** `script\item\heart_head.lua:27` (`FORBITMAP_LIST = {}`).
Bản JX1 của `heart_head.lua` (4.660 byte, Linux 9.613 byte) **không có** biến đó — b1 không đè
tệp đã tồn tại nên bản JX1 được giữ. Kết quả `chuangguang30.lua:249` `FORBITMAP_LIST[...] = 1` nổ.

Hệ quả (theo `KSortScript.cpp:175-183`): `Load()` trả FALSE, **thân tệp dừng ở dòng 273**, nên
`ChuangGuan30:Init()` (274) và `ChuangGuan30:RegistAll()` (275) **KHÔNG BAO GIỜ CHẠY** →
Vượt Ải không đăng ký mission/sự kiện nào.

**Đợt 23/08 đã gặp đúng lỗi này và đã vá đúng chỗ khác**, xem `script\missions\tongcastle\game.lua:163-164`:
```lua
-- [TONGCASTLE 23/08] heart_head JX1 khong co FORBITMAP_LIST (ban Linux :27) - tu khoi tao
FORBITMAP_LIST = FORBITMAP_LIST or {}
```
Đợt 25/08 **quên áp cùng cách vá** cho `chuangguang30.lua`.
`s10_api.py` đã quét toàn bộ 8 tệp phụ thuộc DIVERGED: **`FORBITMAP_LIST` là tên duy nhất còn thiếu**
(các tên khác chỉ-Linux — `add_forbitmap`, `del_forbitmap`, `FORBIT_TEMPLATEMAP_LIST`… — không tệp mới nào dùng).

### C2 — CHẶN — `script\missions\challengeoftime\npc\transfer.lua:3`: đổ theo C1

`transfer.lua:3` `Include("\\script\\missions\\challengeoftime\\chuangguang30.lua")` → lỗi lan sang,
state của `transfer.lua` cũng hỏng. Đây là NPC dịch chuyển của Vượt Ải.
(`npc\dragonboat_main.lua` không Include `chuangguang30` nên sống sót.)

### C3 — CHẶN — `script\activitysys\config\41\extend.lua`: **thiếu 3 tệp anh em, lỗi boot mỗi lần khởi động**

`extend.lua` (13.089 byte) được b1 chép qua danh sách `extra` (`b1_copy.py:93`) nhưng **3 tệp nó
Include thì không**:

| `extend.lua` dòng | Include | Có ở bản Linux? | Có ở JX1? |
|---|---|---|---|
| 1 | `\script\activitysys\config\41\head.lua` | **CÓ (296 B)** | **KHÔNG** |
| 2 | `\script\activitysys\config\41\variables.lua` | **CÓ (878 B)** | **KHÔNG** |
| 3 | `\script\activitysys\config\41\data.lua` | **CÓ (1.153 B)** | **KHÔNG** |

`head.lua` là nơi tạo `pActivity`, nên `extend.lua:11` nổ `attempt to index global 'pActivity'`
(đã ghi log 01:48:53). Thư mục `script\activitysys\config\41\` hiện chỉ có **1/8 tệp** của bản Linux.

Đối chiếu: thư mục `32\` được chép **4 tệp** (`dailytask`, `head`, `talkdailytask`, `variables`) —
tức chính b1 cũng biết cần `head.lua`+`variables.lua`; riêng `41\` bị bỏ sót. Bản Linux có 8 tệp
(`config`, `data`, `extend`, `handin_xunzhang`, `head`, `registe`, `update_xunzhang`, `variables`).

### C4 — CHẶN — `script\activitysys\config\1005\variables.lua` thiếu

`script\activitysys\config\1005\check_func.lua:2` Include `…\1005\variables.lua` (bản Linux 5.051 B) —
**không được chép**. Thư mục `1005\` chỉ có 2/23 tệp của bản Linux.
Chưa thấy lỗi boot vì `check_func.lua` chỉ định nghĩa hàm, nhưng mọi hằng số trong `variables.lua` = nil.

### C3-bis — (trùng A2, bổ sung bằng chứng) hai lỗi CHẶN đã báo, nay có log

* **`script\item\lenhbaiadmin.lua:107`** — nhãn menu `"Hoạt động Linux (Săn Boss Sát Thủ / Phong Lăng Độ / Vượt ải): test/HD3_AdminMenu"`.
  Engine tách ở dấu `/` **ĐẦU TIÊN** (`ScriptFuns.cpp:1056` `char* pFunName = strstr(pAnswer, "/");`)
  rồi chép phần đuôi vào `m_szTaskAnswerFun[…][32]` (`KPlayer.h:636`, **32 byte**).
  ⇒ tên hàm thật = `" Phong Lăng Độ / Vượt ải): test"` (31 byte, vừa đúng bộ đệm) — **khớp
  chính xác** chuỗi `cFuncName:( Phong L?g ? / V??? ?): test)` trong `ScriptError.log` 01:52:05.
  Không mục nào trong 8 mục cũ (dòng 93-101) có dấu `/` trong nhãn.
* **`script\tinhnang\3hoatdong\hd3_driver.lua:23/92/123`** —
  `HD3_VA_TRIGGER = "\\settings\\trigger_challengeoftime.lua"` rồi `DynamicExecute(HD3_VA_TRIGGER, "OnTrigger")`.
  `LuaDynamicExecute` (`ScriptFuns.cpp:2403-2419`) tra `g_GetScript(szLow)`, mà `g_IniScriptEngine`
  (`KSortScript.cpp:52-66`) **chỉ nạp `\script` và `\scriptjx2\tong_vn`** — `settings\` không nằm trong đó,
  và `g_GetScript` **không tự nạp** (chú thích `KSortScript.cpp:61`).
  ⇒ Cả `HD3_Tick` lẫn nút admin `HD3_Adm_VA_Now()` **là no-op tuyệt đối**; chỉ ghi
  `[WLLS] DynamicExecute: script chua nap, bo qua`.

### N1 — NẶNG — **`script\battles\` bị HỒI SINH dù chủ game đã cố tình đóng gói tắt nó**

Trước 25/08 thư mục `script\battles\` **chỉ còn 2 tệp nén**:
```
script/battles/battles.zip      34.352 B   (01/08/2021)
script/battles/tongkimxua.zip   21.271 B   (01/08/2021)
```
`battles.zip` chứa **đúng bộ tệp vừa được chép lại**: `battlehead.lua (42.044)`,
`battleinfo.lua (11.813)`, `battle_rank_award.lua (25.526)`, `doubleexp.lua (445)`, `lang.lua (180)`…
⇒ Chủ game đã **gỡ toàn bộ `.lua` khỏi `script\battles\` và nén lại để TẮT** nhánh Tống Kim kiểu JX2
(JX1 chạy Tống Kim riêng ở `script\startgame\tinhnang\tongkim\` và `script\missions\宋金战场pk战_TongKimXua.zip`).

Đợt 25/08 chép trở lại **6 tệp** vào đúng thư mục đó (`battlehead 44.633`, `battleinfo 11.813`,
`battle_rank_award 3.058`, `doubleexp 640`, `lang 409`, `weeklyrank 5.711`) — **bản Linux, khác
revision với bản đã lưu trữ**.

Hệ quả đo được (`s6_collide.py`):

| Tệp JX1 cũ | Bao đóng state TRƯỚC | SAU | Số tệp JX1 cũ bị kéo thêm vào |
|---|---|---|---|
| `朱仙镇to宋金战场.lua` (**bẫy vào Tống Kim**) | 1 tệp | **58 tệp** | 47 |
| `襄阳to宋金战场.lua` (**bẫy vào Tống Kim**) | 1 tệp | **58 tệp** | 47 |
| `branch_bwsj.lua` | 3 tệp | **60 tệp** | 47 |
| 2 × `songjin_shophead.lua` | 36 tệp | **60 tệp** | 14 |

`battlehead.lua:1-17` kéo theo `event\storm\function.lua`, `event\great_night\huangzhizhang\event.lua`,
`missions\boss\bigboss.lua`, `bonusvlmc\head.lua`, `misc\vngpromotion\ipbonus\ipbonus_2_head.lua`,
`event\jiefang_jieri\200904\qianqiu_yinglie\head.lua`, `battles\weeklyrank.lua`… và đặt lại hàng loạt
hằng toàn cục Tống Kim trong state của **bẫy vào chiến trường**:
`FRAME2TIME=18`, `BAOMING_TIME=10`, `FIGHTING_TIME=60`, `SONGJIN_SIGNUP_FEES=200000`,
`JUNGONGPAI=1773`, `JUNGONGPAI_Task_ID=1830`, `sj_InvincibleState={963,1,0,3*18}`, `BOSSINFO`, `BOSSEXHIBITPOSITION`
(`battlehead.lua:19-63`).

Đã kiểm: thân 2 tệp bẫy (chỉ `NewWorld(324,…)` + `SetFightState(0)`) **không dùng** các hằng đó, và
lần boot 01:48 **không sinh lỗi** trong 2 thư mục trap. Nhưng đây vẫn là **hồi quy có chủ đích bị đảo
ngược**: một hệ mà chủ game đã tắt nay chạy lại, trên chính đường vào Tống Kim.
👉 **Cần chủ game xác nhận** có muốn `script\battles\` sống lại không. Nếu không: 3 hoạt động này
chỉ thật sự cần `battlehead.lua` cho `fengling_ferry\boss.lua:2`, `fld_smalltimer`, `challengeoftime\include.lua`
— nên đặt bản Linux ở **thư mục khác** (ví dụ `script\missions\fengling_ferry\lib_battlehead.lua`)
thay vì trả về `script\battles\`.

### N2 — NẶNG — `huoyuedu` (điểm năng động) **âm thầm bật lại** trên Dã Tẩu và Liên Đấu WLLS

Hai lời gọi JX1 CŨ trước đây chỉ ghi log "script chua nap", **nay chạy thật**:

* `script\global\seasonnpc.lua:105`
  `DynamicExecuteByPlayer(PlayerIndex, "\\script\\huoyuedu\\huoyuedu.lua", "tbHuoYueDu:AddHuoYueDu", "yesourenwu")`
  → chạy **mỗi lần người chơi hoàn thành 1 nhiệm vụ Dã Tẩu**.
* `script\missions\leaguematch\head.lua:678`
  `… "tbHuoYueDu:AddHuoYueDu", "wulinliansai"` → chạy **mỗi lần thắng 1 trận Liên Đấu cấp 2**.

`huoyuedu.lua:68-73`:
```lua
function tbHuoYueDu:AddHuoYueDu(szActivity)
    self:TryWeeklyHuoYueAddToTotal()      -- ← CHẠY TRƯỚC, VÔ ĐIỀU KIỆN
    local nActivityId = tbActivity2ID[szActivity]
    if not nActivityId then return end
```
Cả `"yesourenwu"` lẫn `"wulinliansai"` **đều không có** trong `tbActivity2ID`
(`huoyuedu.lua:20-42`) → thoát sớm, **nhưng `TryWeeklyHuoYueAddToTotal()` đã chạy rồi**.
Hàm đó (`huoyuedu.lua:225-230`) gọi `HuoYueDuSyncTask()` và
`PlayerFunLib:AddTaskDaily(self.TSK_Add_Week_Total, 1)` ⇒ **ghi biến nhiệm vụ 4156** (và 2880 ở nhánh sau)
vào bản ghi nhân vật thật.

Đồng thời `huoyuedu.lua:251-252` chạy `LoadTable` + `RegisterEvent` lúc boot.

Đã kiểm task id: **2880 / 2882 / 4156 / 2862-2883 / 3151-3169 / 4113-4122 / 4171 đều TRỐNG trên JX1**
(3 kết quả grep còn lại là cột toạ độ/ID khác, không phải biến nhiệm vụ) ⇒ **không hỏng dữ liệu**,
nhưng vẫn là **bật một hệ thống mới lên 2 tính năng đang sống mà không ai yêu cầu**, kèm khả năng
bắn `Msg2Player("Chúc mừng ngươi nhận được … điểm năng động")` và gói đồng bộ client
(`HuoYueDuSyncTask`) mà client JX1 **chưa được kiểm chứng là có UI** — **CHƯA XÁC MINH** phía client.

### N3 — NẶNG — `scriptjx2\lib\droptemplet.lua:31` nay nạp thật `songjinskill.lua`

`droptemplet.lua` là **thư viện rơi đồ dùng chung** của cả nhánh JX2 (bang hội, Tống Kim, Dã Tẩu).
Dòng 31 `Include("\\script\\item\\battles\\songjinskill.lua")` trước đây thất bại im lặng,
nay nạp 7.444 byte bản Linux vào **mọi state đang dùng droptemplet**.
Đã kiểm `s6_collide.py`: **0 hàm trùng tên** ngoài `lib\common.lua`, và không có lệnh cấp cao nhất
trong `songjinskill.lua` ⇒ rủi ro thấp, nhưng đây là thay đổi **ngoài phạm vi 3 hoạt động**.

### N4 — NẶNG — thiếu bảng `settings\maps\liandandong\npc_3.txt` → boss Vượt Ải không sinh

`script\missions\challengeoftime\chuangguang30.lua:216-218`:
```lua
local szFile = "\\settings\\maps\\liandandong\\npc_3.txt"
local x,y = get_file_pos(szFile, random(2,50), 1)
basemission_CallNpc(map_new_Ncp[nBossid], CHUANGGUAN30_MAP_ID, x, y)
```
Tệp **có ở bản Linux** (`D:\ServerLinux\server1\settings\maps\liandandong\npc_3.txt`, 713 B) và
**cũng có ở gốc B** — nhưng **không nằm trong 101 tệp đã chép**. ⇒ `x,y` = nil ⇒ boss mật phòng
không xuất hiện (hoặc sinh ở toạ độ rác).

### N5 — NẶNG — thiếu `settings\task\dailytask\talk.txt` + `talk_pos.txt`

`script\activitysys\config\32\talkdailytask.lua:7` đọc cả hai. Bản Linux có
(`talk.txt` 200 B, `talk_pos.txt` 3.259 B); JX1 **không có**.

### N6 — NẶNG — thiếu `settings\event\thapnienlenhbai\taskthapnien.txt`, mà `mainfuc.lua` vẫn nạp lúc boot

`script\vng_event\thapnienlenhbai\lenhbai_def.lua:11` trỏ tới bảng đó (bản Linux 3.053 B, **không được chép**),
còn `script\vng_event\thapnienlenhbai\mainfuc.lua:164` có lệnh **cấp cao nhất** `tbThapnienLenhbai:LoadFile();`
⇒ chạy mỗi lần boot với bảng rỗng.
(Hệ "Thập niên lệnh bài" bị kéo vào qua `script\missions\challengeoftime\award.lua` — không phải yêu cầu của đợt này.)

### N7 — NẶNG — `missions\yandibaozang\` chỉ được chép **1/16 tệp**, dữ liệu **0/toàn bộ**

Bản Linux `script\missions\yandibaozang\` có 16 mục (`head`, `mission`, `npc`, `npc.lua`,
`npc_death`, `player_death`, `saizi`, `timer_match`, `trap\`, `readymap\`, `item\`, `trigger_include`, …);
JX1 chỉ có `head.lua`. `head.lua:210-214` khai báo `YDBZ_mapfile_trap` trỏ tới
`\settings\maps\yandibaozang\trap\{a,b,c}` + `…\trap\clear\{a,b,c}` — bản Linux có sẵn cả thư mục
(`atrap1..atrap10.txt` + `clear\`), JX1 **không có `settings\maps\yandibaozang\` nào cả**.
`head.lua` không có lệnh cấp cao nhất nên hiện nằm im, nhưng nó **được `huoyuedu.lua` và
`task\metempsychosis\translife_5.lua` tham chiếu** ⇒ nửa hệ thống treo.

### N8 — NẶNG — `killbosshead.lua` bị chép + vá nhưng **KHÔNG có trong `b1_manifest.txt`**

```
script\task\tollgate\killbosshead.lua   384.861 B   mtime 25/08 01:03:20   ← cùng lô chép
```
* Không có trong 101 dòng của `b1_manifest.txt` (đã đối chiếu `s4_mtime.py`).
* **Đã bị B2 vá** (md5 khác bản Linux: `6af2855aac` vs `159b2acec0`) — `AddNpc→AddNpcEx` + ánh xạ item.
* `restore_manifest.py` (kịch bản hoàn tác) chỉ duyệt `b1_manifest.txt` ⇒ **không hoàn tác được tệp này**.
* Bàn giao mục 2-B1 có nhắc `killbosshead.lua` là "bảng dữ liệu" đã chép ⇒ **manifest thiếu 1 dòng**,
  con số "101 tệp" thật ra là **102**.

Kèm theo: `settings\task\missions.txt` cũng bị **SỬA** (không phải thêm mới, có sao lưu
`.truoc_3hd_2508`) — slot 15 và 22 từ `\script\missions\mission_trong.lua` đổi sang
`fengling_ferry\mission.lua` / `challengeoftime\mission_match.lua`; tệp này cũng **không nằm trong manifest**.

### N9 — NẶNG — thư mục gương và cây sống khớp, nhưng manifest không phản ánh 10 tệp

`D:\GAMEDEVNEW\serverscript_jx2\3hoatdong` có **111 tệp**, khớp byte 100 % với cây sống (0 lệch).
10 tệp ngoài manifest: `killbosshead.lua`, `settings\task\missions.txt`, 4 tệp mới tự viết
(`autoexec_npc_hd3`, `hd3_admin`, `hd3_driver`, `hd3_thuyenphu`) và 4 tệp wiring
(`startgame.lua`, `timerserver.lua`, `header\cauhinh_hoatdong.lua`, `item\lenhbaiadmin.lua`).
⇒ Ai đọc bàn giao/manifest để đánh giá phạm vi sẽ **hụt 10 tệp**, trong đó **2 tệp là sửa đè lên tệp JX1 cũ**.

---

### L1 — NHẸ — `tongcastle.lua:519` mâu thuẫn với thực tế mới

Chú thích 23/08 ngay trên đó nói *"he huoyuedu (diem hoat bat) KHONG co tren JX1 - de nguyen se
spam log 'script chua nap'"* nên đã comment lời gọi. Nay `huoyuedu.lua` **đã có** ⇒ chú thích sai và
Bang Hội Thành Bảo là hoạt động duy nhất **không** cộng điểm năng động, trong khi Dã Tẩu/Liên Đấu thì có (N2).
Không hỏng gì, nhưng **thiếu nhất quán**.

### L2 — NHẸ — `forbiditem.lua:137` nay chạy thật nhưng **vẫn vô hiệu**, chỉ tốn công

`Check_ItemUsable` (gọi cho **mọi lần dùng vật phẩm** toàn server):
```lua
if DynamicExecute("\\script\\vng_feature\\forbiditem\\vngforbidspecialitem.lua",
                  "tbVNGForbidItem:CheckItemUsable", nSubWorldID, pack_ItemDetail(...)) == 0 then
```
`LuaDynamicExecute` (`ScriptFuns.cpp:2403-2454`) kết thúc bằng `return 0;` — **không đẩy giá trị trả về**.
⇒ biểu thức luôn là `nil`, `nil == 0` là false ⇒ **danh sách cấm không bao giờ áp dụng** (trước và sau đều thế).
Khác biệt duy nhất: trước đây engine chỉ ghi log "script chua nap"; nay nó **thật sự chạy** thân hàm
mỗi lần dùng đồ (rẻ, nhưng thừa) và nạp thêm 1 state lúc boot (state đó lại `Include forbiditem.lua`
→ chạy `LoadItemType/LoadItemDetail/LoadMapDetail` = 3 lần `TabFile_Load` thừa).

> ⚠️ Nếu ai đó "sửa" `LuaDynamicExecute` cho trả giá trị, lệnh cấm sẽ **bật ngay lập tức**, và bảng
> trong `vngforbidspecialitem.lua:39-47` đang trỏ **map JX1 đang sống**:
> `tbMapSet[4]={975}` = **Cạnh Kỹ Trường** và `tbMapSet[5]={380}` = **"Xung phong (Cao cấp)", `MapType=Battlefield`**
> (`settings\MapList.ini:3283-3287`, `6330-6331`) → sẽ cấm ~70 vật phẩm ở Cạnh Kỹ Trường và
> cấm Băng/Lôi/Hoả/Độc Phòng hoàn (`6,0,7..10`) ở **chiến trường Tống Kim 380**.

### L3 — NHẸ — `kill_level.lua` thiếu `OnRevive` → **160 dòng lỗi mỗi lần sinh boss Sát Thủ**

`ScriptFuns.cpp:7196-7199` — `SetNpcDeathScript` gọi ngay `OnRevive` nếu NPC là `kind_normal`.
`script\task\tollgate\killer\kill_level.lua` chỉ có `OnDeath` (dòng 22) ⇒
`ScriptError.log` nhận đúng **160 dòng** lúc 01:49:50 (khớp 160 boss của `add_killertasknpc`).
Lỗi cùng dạng đã tồn tại sẵn với `turebug90.lua` nên **không phải lỗi mới của engine**, nhưng đợt này
nhân số dòng rác lên 160/lượt sinh.

### L4 — NHẸ — hệ Chuyển Sinh (translife 5/6) bị kéo vào ngoài phạm vi

Chuỗi: `challengeoftime\item\chuangguanbaoxiang.lua` → `translife_6.lua` → `translife_5.lua` →
`metempsychosis\task_func.lua` + `missions\yandibaozang\head.lua`.
Trước đợt này JX1 chỉ có `task_head.lua` (15/08). Nay thêm 4 tệp Linux, trong đó
`translife_6.lua:310 TransLife6:Init()` và `task_func.lua:220 EventSys…Reg("OnLogin", …)`
**chạy lúc boot**. Đã đọc `TransLife6:Init()` (dòng 305-308): chỉ `RegisterEvent()` + `InitDlg()`
thuần Lua ⇒ vô hại, nhưng **không nằm trong yêu cầu 3 hoạt động**.

### L5 — NHẸ — `vngforbidspecialitem.lua:13` `"Tống Kim Chiêu thư " = {6,1,155}` chưa ánh xạ

Bảng ánh xạ 42 ID (`thicong\remap_resolved.json`) **không có** `6,1,155`. Trên JX1 Chiêu Thư Tống Kim
là `6/1/154` (đặc tả `AUTO_TONGKIM_SPEC.md`). Mục này nằm trong `tbForbidItem` (dòng 7-17) mà
`CheckItemUsable` **không đọc tới** (nó chỉ dùng `tbItemList`) ⇒ hiện vô hại. Ghi lại để khỏi sót nếu bảng đó được dùng sau.

### L6 — NHẸ — `\settings\trigger_` trong `g_IsJx2Script` là **mã chết**

`KSortScript.cpp:126-130` thêm tiền tố `"\\settings\\trigger_"`. Nhưng `g_IsJx2Script` so với
`g_ScriptSet[i].m_szScriptName` = đường dẫn tệp **được nạp lúc boot**; `settings\` không bao giờ
được nạp (mục C3-bis) ⇒ tiền tố này **không thể khớp**.
Ngược lại, state thật sự chạy mã JX2 là `\script\tinhnang\3hoatdong\hd3_driver.lua`,
`\script\global\autoexec_npc_hd3.lua`, `\script\item\hd3_admin.lua` — **không tệp nào có trong danh sách**.
(A2 đã ghi cùng ý ở L8; ghi lại vì thuộc bộ tệp chép.)

---

## 5. NHỮNG MỤC ĐÃ KIỂM VÀ **KHÔNG CÓ VẤN ĐỀ** (để phiên chính khỏi soát lại)

| # | Mục | Kết quả |
|---|---|---|
| 1 | **Tệp `.bak` / `.lua.bak` / `.truoc_*` / `.zip` lọt vào bộ chép** | **KHÔNG**. 101 tệp = 80 `.lua` + 13 `.txt` + 8 `.ini`; 78 `.lua` dưới `script\`, 23 tệp còn lại dưới `settings\`. Các tệp `.lua.bak`/`.truoc_*` sẵn có trong cây **không bị nạp** vì `LoadScriptInDirectory` (`KSortScript.cpp:281-285`) chỉ nhận đuôi `.LUA`/`.TXT`. |
| 2 | **Đè lên `scriptjx2\`** | **0 tệp** dưới `scriptjx2\` bị ghi ngày 25/08 (`s13_lib.py`). 24 tệp `lib\`/`tong\` trong bao đóng đều đã có sẵn → b1 `skip_lib` đúng. |
| 3 | **`scriptjx2\lib` lệch bản Linux** | 4/24 lệch: `lib\basic.lua` (3.606 vs 3.518), `lib\string.lua` (7.512 vs 6.965), `tong\log.lua`, `tong\tong_header.lua`. `s10_api.py` xác nhận **không tên toàn cục nào bị thiếu** so với bản Linux ⇒ an toàn. |
| 4 | **Trùng ScriptID** | 0/2.959 (mục 2.1). |
| 5 | **Trần `MAX_SCRIPT_IN_SET`** | 2.959 / 5.000 — dư 41 %. |
| 6 | **`syncheck.exe` (Lua 4.0 của chính engine JX1)** | **89/89 `cú pháp OK`, 0 lỗi** — 80 tệp `.lua` trong manifest + `killbosshead.lua` + 4 tệp mới tự viết + 4 tệp wiring (`s7_syn.py`). 21 tệp `.txt/.ini` bỏ qua. |
| 7 | **Định dạng bảng `settings` đã chép** | `bosstask_lev20..80.ini` = `Count=101` / **101 section**, `lev90` = `Count=100` / **100 section**; khoá `Genre/Detail/Particular/RandRate` đúng y `g_GenItemDropRate` (`KNpcTemplate.cpp:9-46`); genre dùng {0,4,6}; **không ID nào nằm trong bảng ánh xạ 42 ID** ⇒ không cần remap. `killer.txt` 3 cột (Id/BossName/BossInfo, 160 dòng dữ liệu, chữ TCVN3 đọc được). `huoyuedu.txt` 15 cột. `lineup{8,16,20,24,32,40,56}.txt` 2 cột `XPOS/YPOS`, số dòng khớp tên. |
| 8 | **Đường dẫn tên thư mục GBK** | `settings\maps\中原北区\渡船\渡船刷怪点.txt` (893 B, 63 điểm, header `XPOS\tYPOS`, toạ độ đã ×32) **khớp byte-đối-byte** với chuỗi trong `script\missions\fengling_ferry\fld_head.lua:18` — mở được bằng đúng chuỗi đó. |
| 9 | **Bao đóng `settings\` của tệp mới** | 32/42 tham chiếu **tồn tại**; 10 tham chiếu thiếu đã liệt kê ở N4/N5/N6/N7 (`s11b.py`). |
| 10 | **`boss.lua:11`** — Include chết `\script\global\路人_礼官.lua` | **ĐÃ gỡ đúng**, nay là dòng chú thích `-- [3HD 25/08] bo Include tep khong ton tai…`. |
| 11 | **Gương repo `serverscript_jx2\3hoatdong`** | 111 tệp, **lệch 0** so với cây sống. |

---

## 6. THỨ TỰ ĐỀ NGHỊ SỬA (rẻ → đắt)

1. **C1** — thêm 1 dòng `FORBITMAP_LIST = FORBITMAP_LIST or {}` vào đầu
   `ChuangGuan30:SetForbitItem()` (`chuangguang30.lua:244`), y hệt `tongcastle\game.lua:164`.
   *Sửa xong C2 tự khỏi.*
2. **C3 / C4** — chép nốt 4 tệp từ bản Linux:
   `activitysys\config\41\{head,variables,data}.lua` + `activitysys\config\1005\variables.lua`
   (nên chép **cả** `41\config.lua`, `41\registe.lua` để đủ hệ, cần xác minh riêng).
3. **N4 / N5 / N6 / N7** — chép nốt bảng dữ liệu:
   `settings\maps\liandandong\npc_3.txt`, `settings\task\dailytask\{talk,talk_pos}.txt`,
   `settings\event\thapnienlenhbai\taskthapnien.txt`, `settings\maps\yandibaozang\**`.
4. **N8** — thêm `script\task\tollgate\killbosshead.lua` và `settings\task\missions.txt`
   vào `b1_manifest.txt` (hoặc một manifest phụ) để `restore_manifest.py` hoàn tác được.
5. **N1** — **hỏi chủ game**: có cho `script\battles\` sống lại không?
   Nếu không → dời `battlehead.lua` (+5 tệp) sang thư mục riêng và sửa 8 chỗ `Include` trong bộ tệp mới,
   để 5 tệp JX1 cũ quay lại trạng thái "Include hỏng im lặng" như trước.
6. **N2** — quyết định: giữ `huoyuedu` bật (thì bổ sung `"yesourenwu"`/`"wulinliansai"` vào
   `tbActivity2ID` và bỏ comment ở `tongcastle.lua:519`), hay tắt hẳn
   (đổi `huoyuedu.lua` sang thư mục ngoài `script\`).
7. **L3** — thêm `function OnRevive(nNpcIndex) end` rỗng vào `kill_level.lua` để dập 160 dòng rác/lượt.
8. **L6** — bỏ `"\\settings\\trigger_"` khỏi `g_IsJx2Script`, thêm
   `"\\script\\tinhnang\\3hoatdong\\"` (và các state thật sự chạy mã JX2).

---

*Người soát: A5 — bộ 101 tệp đã chép. Không sửa bất kỳ tệp nào; toàn bộ số liệu tái tạo được bằng
`s1..s14*.py` trong scratchpad.*


---

## ĐỐI CHẤT (tác tử độc lập)

> Người đối chất: tác tử kiểm chứng độc lập, **không** phải người viết A5.
> Nguyên tắc: mặc định mọi phát hiện là SAI cho tới khi tệp gốc / nhị phân / mã C chứng minh ngược lại.
> Đã kiểm **24 phát hiện**; kết quả: **1 SAI hẳn**, **1 dòng bảng bịa**, **2 THỔI PHỒNG**, còn lại ĐÚNG.
> Không sửa bất kỳ tệp mã nguồn nào — chỉ ghi thêm vào chính báo cáo này.
> Kịch bản kiểm tái tạo được: `…\scratchpad\dc_scan.py`, `dc_top.py`, `dc_id.py`, `dc_id3.py`, `dc_id4.py`.

### Bảng đối chất

| Phát hiện | Bằng chứng gốc (đã tự mở lại) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|
| **C1** `chuangguang30.lua:249` FORBITMAP_LIST nil | `heart_head.lua` JX1 4.660 B: `grep -c FORBITMAP_LIST` = **0**; Linux 9.613 B có ở `:27,28,31,35,84,197`. `ScriptError.log` (challengeoftime) ghi đúng `SetForbitItem' at line 249` / `main … at line 273`. Thân dừng ⇒ `Init()`(274) + `RegistAll()`(275) không chạy. | **ĐÚNG — CHẶN** | Giữ nguyên, **nhưng bổ sung hệ quả bị bỏ**: `KSortScript.cpp:180-186` khi `Load()` FALSE thì `return FALSE` **TRƯỚC** `g_ScriptBinTree.Insert` ⇒ chuangguang30.lua **không có trong cây ID**, nên mọi `DynamicExecute`/`SetDeathScript(:112)` trỏ tới nó cũng chết, không chỉ mất `Init/RegistAll`. Cách vá A5 đề xuất là **ĐÚNG và đủ**: đã xác minh `tb_MapType` (`forbiditem.lua:12`) và `set_MapType` (`:22`) **có** trên JX1, nên phần còn lại của `SetForbitItem` chạy được sau khi thêm 1 dòng. |
| **C2** `npc\transfer.lua:3` đổ theo C1 | `npc\ScriptError.log` có nguyên vẹn traceback 4 tầng kết thúc `function 'Include' [C] / main … at line 3`. | **ĐÚNG — CHẶN** | Giữ nguyên. |
| **C3** `activitysys\config\41\extend.lua` thiếu 3 tệp anh em | Lỗi có thật (`41\ScriptError.log` 01:48:53). **NHƯNG**: `41\head.lua` bản Linux `:3-6` = `pActivity.nId = 41`, `szName = "2012 april_zhushuai"`, `nStartDate = 201204020000`, `nEndDate = 201205010000` — **hoạt động 2012 đã hết hạn**. `grep -rn "config.41" ` toàn cây JX1 **và** toàn cây Linux ⇒ **0 tham chiếu**. Đăng ký chỉ xảy ra ở `41\registe.lua` (`G_ACTIVITY:AddActivity`) — tệp này **không có** ở JX1. `G_ACTIVITY.tbSet = nil` (`g_activity.lua:12`). | **THỔI PHỒNG (CHẶN → NHẸ) + SAI CÁCH SỬA** | Mức thật = **NHẸ**: 1 dòng ScriptError lúc boot + 1 ô script bị đốt; không tính năng nào hỏng. Cách sửa của A5 (“nên chép **cả** `41\config.lua`, `41\registe.lua` để đủ hệ”) là **sai hướng** — nó sẽ nhét một hoạt động 2012 hết hạn vào `G_ACTIVITY`. Sửa đúng: **xoá `41\extend.lua` vừa chép** (hoàn tác), hoặc nếu muốn giữ thì chỉ chép `head/variables/data.lua` và **tuyệt đối không** chép `registe.lua`. (Hai lời gọi duy nhất vào `extend.lua` là `challengeoftime\doubleexp.lua:7` + `battles\doubleexp.lua:7`, mà cả hai đi qua `DynamicExecuteByPlayer` **không trả giá trị** ⇒ có hay không có `extend.lua` cũng cùng kết quả, xem “Bỏ sót #1”.) |
| **C4** `activitysys\config\1005\variables.lua` thiếu | `grep -vn "^\s*--" check_func.lua` ⇒ **rỗng hoàn toàn**: cả 1.162 byte của `check_func.lua` là chú thích, dòng 2 là `--Include("\\script\\activitysys\\config\\1005\\variables.lua")`. Include **không bao giờ chạy**. | **SAI** | **Xoá mục C4.** Số CHẶN tụt từ 4 → (C1, C2) + C3 hạ cấp = **2**. |
| **C3-bis** `lenhbaiadmin.lua:107` nhãn menu chứa `/` | `ScriptFuns.cpp:1056` `char* pFunName = strstr(pAnswer, "/")`, `:1058` `g_StrCpyLen(m_szTaskAnswerFun[i], pFunName+1, 32)`; `KPlayer.h:636` `char m_szTaskAnswerFun[MAX_ANSWERNUM][32]`. `ScriptError.log` 01:52:05 khớp từng byte. | **ĐÚNG — CHẶN** | Bổ sung: dòng log 01:51:58 gán cho `script\item\tasklink_goods.lua` **không phải điểm tiêm thứ hai** — tệp đó mtime 20/08, `grep HD3` = 0. Tên hàm nằm trên **NGƯỜI CHƠI** (`m_szTaskAnswerFun`) chứ không trên script, nên chỉ cần bỏ dấu `/` trong nhãn ở `lenhbaiadmin.lua:107`, không phải sửa `tasklink_goods.lua`. |
| **C3-bis** `hd3_driver.lua` `DynamicExecute` vào `\settings\…` | `ScriptFuns.cpp:2413-2419` `g_GetScript(szLow)` → NULL → log “script chua nap”; `KSortScript.cpp:52-66` `g_IniScriptEngine` chỉ `LoadAllScript("\\script")` + `("\\scriptjx2\\tong_vn")`. | **ĐÚNG — CHẶN** | Giữ nguyên. |
| **N1a,b,d,e** `battlehead.lua` bỗng resolve ở 4 tệp JX1 cũ | `grep -rn battlehead` cây sống: `global\ÌØÊâÓÃµØ\ËÎ½ð±¨Ãûµã\npc\songjin_shophead.lua:2`, `startgame\tinhnang\tongkim\songjin_shophead.lua:2`, `ÖÐÔ­±±Çø\ÖìÏÉÕò\trap\…:4`, `ÖÐÔ­ÄÏÇø\ÏåÑô\ÏåÑô\trap\…:3` — **4 dòng `Include` thật**. `battles.zip` (01/08/2021) chứa 17 tệp, chỉ 6 được chép lại. | **ĐÚNG — NẶNG (câu hỏi chính sách)** | Sửa số: **4 điểm gọi cũ**, không phải 5. |
| **N1c** `branch_bwsj.lua:2` | `cat -A` dòng 2: `--Include("\\script\\battles\\battlehead.lua")^I-- [BW 23/08] chi phuc vu branchTask_JoinSJend1 (0 call site), keo 26 tep Tong Kim JX2` — **đã comment từ 23/08**. | **SAI (dòng bảng bịa)** | Bỏ dòng `c` khỏi bảng mục 1 **và** bỏ dòng `branch_bwsj.lua 3 → 60 tệp` khỏi bảng bao đóng N1 (con số đó không tồn tại). Bộ quét của A5 **không bỏ qua chú thích Lua** — đây là cùng lỗi đã tạo ra C4. |
| **N2** `huoyuedu` bật lại trên Dã Tẩu + Liên Đấu | `huoyuedu.lua:68-73` đúng như trích (gọi `TryWeeklyHuoYueAddToTotal()` trước guard); `:225-229` ghi task **4156**, `:247` ghi **2880**. Hai điểm gọi `seasonnpc.lua:105` và `leaguematch\head.lua:678` **không bị comment** (đã `cat -v`). Kiểm thêm điều A5 không kiểm: `settings\huoyuedu\huoyuedu.txt` có **42 dòng = 41 bản ghi**, phủ đủ id lớn nhất trong `tbActivity2ID` (=41) ⇒ `HuoYueDuSyncTask()` **không nổ**, nên đúng là có ghi task thật. | **ĐÚNG — NẶNG (biên dưới)** | Giữ NẶNG nhưng ghi rõ **tác động nhìn thấy được với người chơi = 0** (thoát sớm trước mọi `Msg2Player`); chi phí thật chỉ là 2 biến nhiệm vụ + gói `SyncTaskValue`. Nếu chủ game không quan tâm 2 biến đó thì đây là NHẸ. |
| **N3** `droptemplet.lua:31` | Dòng 31 `Include("\\script\\item\\battles\\songjinskill.lua");` — không comment. | **ĐÚNG — NẶNG (rủi ro thấp)** | Giữ nguyên. |
| **N4** thiếu `settings\maps\liandandong\npc_3.txt` | JX1: **MISSING**; Linux: 713 B. `get_file_pos` là hàm **Lua** (`challengeoftime\include.lua:280`) gọi `GetTabFileData` ⇒ trả nil chứ không sập. | **ĐÚNG — nhưng phải ghi phụ thuộc** | `ChuangGuan30:OnAddBoss` chỉ chạy từ `GameTime` ← timer đặt trong `AddTime` ← **chỉ** `settings\trigger_challengeoftime.lua:22` (không bao giờ nạp). Vậy N4 **chưa phát tác**; nó thành lỗi thật **sau khi** vá C3-bis. Ghi là “NẶNG (tiềm ẩn — phụ thuộc vá C3-bis)”. |
| **N5 / N6 / N7** thiếu bảng `talk.txt`/`talk_pos.txt`/`taskthapnien.txt`/`settings\maps\yandibaozang\` | Đã `stat` từng tệp: **JX1 MISSING, Linux có** (200 B / 3.259 B / 3.053 B); `settings\maps\yandibaozang` không tồn tại; `script\missions\yandibaozang\` JX1 chỉ có `head.lua` còn Linux 16 mục. | **ĐÚNG** | Giữ nguyên. |
| **N8** `killbosshead.lua` ngoài manifest | Manifest = **101 dòng**, `grep killbosshead` ⇒ **không có**; `settings\task\missions.txt` cũng không có. | **ĐÚNG về sự kiện, SAI về khung** | Tệp này **không thuộc lô chép 25/08**: `D:\GAMEDEVNEW\serverscript_jx2\tinsu\script\task\tollgate\killbosshead.lua` (**21/08 22:58**, md5 `159b2ace…` = **giống hệt bản Linux**) và `addtollgatenpc.lua` (mtime **21/08 22:19**) đã `Include` nó từ đợt Tín Sứ. Ngày 25/08 nó bị **SỬA TẠI CHỖ** thành md5 `6af2855a…`. Hệ quả đúng phải ghi: **không có bản sao lưu `.truoc_*` nào trên cây sống** (khác `missions.txt` vốn có `.truoc_3hd_2508`); muốn hoàn tác phải lấy từ `serverscript_jx2\tinsu\`. Đề nghị sửa: tạo `.truoc_3hd_2508` + manifest phụ cho **tệp SỬA**, tách khỏi manifest **tệp CHÉP**. |
| **N8-phụ** `settings\task\missions.txt` bị sửa | `diff` với `.truoc_3hd_2508`: đúng 2 dòng, slot 15 và 22, cả hai từ `\script\missions\mission_trong.lua` (chỗ trống) → `fengling_ferry\mission.lua` / `challengeoftime\mission_match.lua`. | **ĐÚNG — vô hại** | Giữ nguyên. |
| **N9** gương repo lệch 0 | Đếm lại thư mục `serverscript_jx2\3hoatdong`: **111 tệp**. | **ĐÚNG** | Giữ nguyên. |
| **L1** `tongcastle.lua:519` | Đã đọc: lời gọi bị comment kèm chú thích 23/08 “he huoyuedu … KHONG co tren JX1”. | **ĐÚNG — NHẸ** | Giữ nguyên. |
| **L2** `forbiditem.lua:137` chạy thật nhưng vô hiệu | `forbiditem.lua:137` **không** bị comment; `LuaDynamicExecute` (`ScriptFuns.cpp:2454`) `return 0` ⇒ biểu thức là `nil`, `nil == 0` sai. `vngforbidspecialitem.lua:39-47` đúng `tbMapSet[4]={975}`, `[5]={380}`. | **ĐÚNG — NHẸ** | Giữ nguyên. **Nhưng chính mục này là chìa khoá A5 bỏ rơi** — xem “Bỏ sót #1”. |
| **L3** `kill_level.lua` thiếu `OnRevive` | `ScriptFuns.cpp:7196-7198` gọi `ExecuteScript(szScript,"OnRevive",…)` khi `m_Kind == kind_normal`; `kill_level.lua` chỉ có `OnDeath` ở **:22**. Đếm thật trong `ScriptError.log`: **160** dòng `kill_level` ngày 25/08 (và 27 dòng `turebug90` — đã có từ trước, tổng 153 dòng mọi ngày). | **ĐÚNG — NHẸ** | Giữ nguyên. |
| **L5** `"Tống Kim Chiêu thư " = {6,1,155}` | `vngforbidspecialitem.lua:13` đúng nguyên văn. | **ĐÚNG — NHẸ** | Giữ nguyên. |
| **L6** `"\\settings\\trigger_"` là mã chết | Đã đọc `KSortScript.cpp:126-130`: mảng có `"\\settings\\trigger_"`; **không** có `\script\tinhnang\3hoatdong\`, `autoexec_npc_hd3`, `item\hd3_admin`. | **ĐÚNG — NHẸ** | Giữ nguyên. |
| **Mục 5.4** “0 trùng ScriptID / 2.959 tệp” | Tự cài lại `g_FileName2Id` (`KFilePath.cpp:441-455`, `char` có dấu, DWORD 32-bit) và quét `\script\**` + `\scriptjx2\tong_vn\**`: **2.959 tệp**, trong đó **2.953** mã hoá được sang ACP; **2.953 ID, 0 cặp trùng**. | **ĐÚNG** | Bổ sung cảnh báo **không thuộc đợt này**: 6 tệp `script\skill\special\{毒攻击,生命内力双补,轻功,近程物理攻击,远程物理攻击,长兵物理攻击}.lua` (mtime **17/05/2025**) có tên Unicode Trung thật, mà ACP máy này = **1252** ⇒ API ANSI (`_findfirst`/`fopen`) thấy `??????.lua` và **không mở được**. Đã quét toàn cây: **chỉ 6 tệp này**, đợt 25/08 **không** tạo thêm tên nào ngoài ACP ⇒ khẳng định mục 5.8 (`渡船刷怪点.txt` mở được) là **ĐÚNG**. |
| **Mục 5.6** syncheck 89/89 | Chạy lại `syncheck.exe` trên 5 tệp nặng nhất (`chuangguang30`, `hd3_driver`, `hd3_thuyenphu`, `killbosshead` 384 KB, `battlehead` 44 KB): **5/5 “cu phap OK”**. | **ĐÚNG (mẫu)** | Giữ nguyên. |

**Tổng kết đối chất:** ĐÚNG 20 · SAI 2 (C4, dòng N1c) · THỔI PHỒNG 2 (mức C3 + khung N8) · HẠ THẤP 0 (nhưng C1 bị **kể thiếu hệ quả**).
**Số CHẶN thật của mục này = 2** (C1, C2), cộng 2 CHẶN của A2 mà A5 chỉ bổ sung bằng chứng.

---

### Bỏ sót của chính vòng soát

#### Bỏ sót #1 — **CHẶN** — A5 tìm ra “`DynamicExecute` không trả giá trị” (L2) rồi **không quét lại 101 tệp vừa chép bằng chính phát hiện đó**

`LuaDynamicExecuteByPlayer` (`ScriptFuns.cpp:2398`) và `LuaDynamicExecute` (`:2454`) đều kết thúc bằng `return 0` — **không đẩy giá trị trả về nào lên stack**. Trong Lua 4, `local x = f()` với hàm C trả 0 giá trị ⇒ `x = nil`.
Quét lại bộ 101 tệp + 5 tệp tự viết (`dc_scan.py`) ⇒ **7 điểm dùng giá trị trả về**, trong đó **2 điểm nằm trên đường sống của Vượt Ải**:

| Tệp:dòng | Mã | Hậu quả thật |
|---|---|---|
| `script\missions\challengeoftime\npc\dragonboat_main.lua:30` | `local nFlag1 = DynamicExecuteByPlayer(nPlayerIndex,"\\script\\vng_lib\\bittask_lib.lua","tbVNG_BitTask_Lib:getBitTask",{…})` | `nFlag1 = nil` ⇒ `:31 if nFlag1 ~= 0` **luôn đúng** ⇒ `:35 Say("Xin lỗi! Trong đội bạn có thành viên … không thỏa điều kiện")` + `return`. **KHÔNG tổ đội nào báo danh Vượt Ải được — bao giờ cũng bị từ chối.** Đường sống: `autoexec_npc_hd3.lua:8-14` sinh 7 NPC 769 → `killer\nieshichen.lua:5` `Include(dragonboat_main.lua)`. |
| `script\vng_feature\challengeoftime\npcNhiepThiTran.lua:14` | như trên | `:15-17` liệt **mọi** thành viên vào danh sách “không thỏa điều kiện”. Đường sống: `nieshichen.lua:19`. |
| `script\missions\challengeoftime\doubleexp.lua:12` và `:15` | `nExp = DynamicExecuteByPlayer(…)` ×2 rồi `return nExp` | `Chuangguan_checkdoubleexp()` **trả nil** ⇒ `award.lua:203-205`: `point = nil` → `AddOwnExp(nil)` và `Msg2Player("… " .. point)` → **lỗi “attempt to concatenate”, cả khối thưởng vỡ** ⇒ vượt ải xong **không nhận được kinh nghiệm**. Cũng vỡ ở `award.lua:113-114` (`nCount = nil` trong bảng thưởng “xiaonieshichen”). |
| `script\battles\doubleexp.lua:12,15` | như trên | `battlehead.lua:711 point = Songjin_checkdoubleexp(point)` → nil (chỉ phát tác nếu nhánh `battles` được dùng — xem N1). |
| `script\task\metempsychosis\task_func.lua:137` | `local bResult = DynamicExecuteByPlayer(…"\\script\\vng_event\\20130107_den_bu_chinh_do\\head.lua"…)` | `bResult = nil` **và** tệp đích **không tồn tại trên JX1**. |

**Đây là lỗi CHẶN nặng nhất của cả đợt port và A5 đã đi ngang qua nó**: mục C2 còn viết “`npc\dragonboat_main.lua` không Include `chuangguang30` nên **sống sót**” — tức đã mở tệp đó và vẫn không thấy dòng 30.
**Sửa:** hoặc (a) sửa engine: cho `LuaDynamicExecute`/`…ByPlayer` lấy giá trị trả về sau `lua_dostring` và `return` số giá trị đẩy lên (đổi hành vi toàn cục — phải rà 378 điểm dùng `/#` và mọi điểm gọi cũ), hoặc (b) **an toàn hơn**: vá tại 7 điểm gọi — `nFlag1 = nFlag1 or 0`, `nExp = DynamicExecuteByPlayer(...) or nExp`. Với `dragonboat_main.lua` và `npcNhiepThiTran.lua` thì (b) là bắt buộc dù chọn gì.

#### Bỏ sót #2 — **NẶNG** — bao đóng phía `script\` của bộ tệp chép **chưa hề được soát** (A5 chỉ soát bao đóng `settings\`, mục 5.9)

`dc_scan.py` giải escape `\\`→`\`, bỏ dòng chú thích, áp đúng remap engine (`sJX2RemapScriptPath`, `ScriptFuns.cpp:1938-1965`: `script\tong\`→`scriptjx2\tong_vn\`, `script\lib\`→`scriptjx2\lib\`) trên **85 tệp `.lua`** ⇒ **4 đích Include/SetNpcDeathScript CÓ ở bản Linux nhưng KHÔNG được chép**, ngoài 3 đích của C3:

| Đích thiếu | Điểm gọi | Mức |
|---|---|---|
| `script\battles\bossdeath.lua` | `battles\battlehead.lua:80` `SetNpcDeathScript(npcindex, …)` | **NẶNG** — bộ `battles` được phục hồi **nửa vời 6/17 tệp** (`battles.zip` có `bossdeath.lua` 905 B, Linux cũng có). Nếu chủ game đồng ý cho `script\battles\` sống lại (N1) thì boss chiến trường **không có script chết**. |
| `script\vng_event\tanthu\giveskill60.lua` | `global\thanh\npc\npc_hotro.lua:8` và `:18` | NHẸ (tệp mồ côi — Bỏ sót #3) |
| `script\global\thinh\hotro\functions_points.lua` | `global\thanh\npc\npc_chuyensinh.lua:8` | NHẸ (mồ côi) |
| `script\vng_event\20130107_den_bu_chinh_do\head.lua` | `task\metempsychosis\task_func.lua:137` | NHẸ |

(`script\global\tamhiep\callbossdeathmini.lua` — 6 điểm ở `fengling_ferry\boss.lua:30,37,46,53,62,69` — **không có cả ở bản Linux**, `b1_copy.py:88-90` đã ghi nhận và cố ý bỏ qua ⇒ **không tính là bỏ sót**, nhưng nên ghi vào báo cáo rằng boss thuyền 511/513 vì thế **không có script chết** trên JX1.)

#### Bỏ sót #3 — **NHẸ** — 6 tệp trong bộ chép là **mồ côi tuyệt đối**, kèm 1 task id vượt trần

`script\global\thanh\npc\{add_npc, bosssatthuhead, bosssatthudeath, npc_chuyensinh, npc_hotro, npcdeath}.lua` (đều mtime 25/08 01:03).
`grep -rn` toàn cây JX1 (`script\` + `settings\`) ⇒ **0 tham chiếu** tới bất kỳ tệp nào trong số đó. Trên bản Linux chúng chỉ được `script\global\gm\ex_lib_function.lua:78` `Include` — mà tệp GM đó **không** nằm trong bộ chép. Bảng `tbNpcSatThu` trong `add_npc.lua:26+` là bản Sát Thủ **khác** với bản đang dùng (`killbosshead.lua` + `settings\task\tollgate\killer\killer.txt` 160 dòng, do `hd3_driver.lua:44-45` gọi).
Kèm theo: `npc_chuyensinh.lua:23,37,45` dùng **task 5100**, trong khi `KPlayerTask.h:18 #define MAX_TASK 4200` ⇒ `GetTask(5100)` luôn 0 và `SetTask(5100,…)` bị nuốt ⇒ nếu NPC 2530 từng được bật thì **chuyển sinh không giới hạn số lần** (mỗi lần `AddProp(100)` + `ST_LevelUp(-190)`). Hiện **vô hại** vì 8 dòng NPC 2530 ở `add_npc.lua:13-20` đều **đã comment**.
**Sửa:** xoá 6 tệp này khỏi bộ chép (và khỏi `b1_manifest.txt`).

#### Bỏ sót #4 — **NHẸ** — A5 đếm đúng 10 tệp ngoài manifest (N9) nhưng **gộp nhầm “tệp CHÉP” với “tệp SỬA”**

Đã kiểm ngược bằng mtime (`dc_mtime.py`, mốc 25/08 00:00, bỏ `ScriptError.log`): **125 tệp** bị đụng, trong đó **24 tệp ngoài manifest**. Trừ 14 tệp thuộc **đợt khác cùng ngày** (00:11–00:33: `event\tongwar\head.lua`, `missions\tongwar\match\head.lua`, `item\hoatdong_admin.lua`, `settings\PlayerTitle.txt` + các bản `.truoc_cauoi_2508` / `.truoc_vongsang_2508` / `.truoc_themdanhhieu_2508`, và `settings\jx2ladder.txt` / `jx2league.txt` do máy chủ tự ghi lúc 02:50) ⇒ đúng **10 tệp** của đợt 3HD — **khớp N9**.
Điểm A5 bỏ sót là **phân loại**: trong 10 tệp đó, `killbosshead.lua`, `settings\task\missions.txt`, `startgame.lua`, `timerserver.lua`, `header\cauhinh_hoatdong.lua`, `item\lenhbaiadmin.lua` là **tệp SỬA ĐÈ lên tệp JX1 có sẵn**, chỉ 4 tệp còn lại (`hd3_driver`, `hd3_admin`, `autoexec_npc_hd3`, `hd3_thuyenphu`) là **tệp MỚI**. Trong 6 tệp SỬA thì **chỉ `missions.txt` có bản `.truoc_3hd_2508`**; `header\cauhinh_hoatdong.lua` may mắn có `.truoc_vongsang_2508` (00:33, do **đợt khác** tạo — vẫn dùng làm mốc trước-3HD được vì 3HD sửa lúc 01:09); còn **`startgame.lua`, `timerserver.lua`, `item\lenhbaiadmin.lua`, `task\tollgate\killbosshead.lua` KHÔNG có bản sao lưu nào trên cây sống** (đã `ls` từng tệp). Đề nghị: tạo `.truoc_3hd_2508` cho 4 tệp đó, và tách `b1_manifest.txt` (tệp CHÉP → xoá khi hoàn tác) khỏi một `b1_patched.txt` (tệp SỬA → phục hồi từ backup), vì `restore_manifest.py` hiện chỉ xử lý được loại thứ nhất.

---

*Đối chất xong. Không sửa mã nguồn; chỉ ghi thêm mục này vào `A5_copyset.md`.*
