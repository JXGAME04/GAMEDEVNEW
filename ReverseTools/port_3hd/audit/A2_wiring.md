# A2 — SOÁT 4 TỆP LUA MỚI + 4 TỆP WIRING (port 3 hoạt động Linux, 25/08)

Phạm vi: `hd3_driver.lua`, `hd3_admin.lua`, `autoexec_npc_hd3.lua`, `hd3_thuyenphu.lua`
(mới) + `startgame.lua`, `timerserver.lua`, `header\cauhinh_hoatdong.lua`,
`item\lenhbaiadmin.lua` (đã sửa). Cây sống `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`.

**CHỈ ĐỌC — không sửa gì.** Lỗi `GetNpcValue()` thiếu tham số mà phiên chính đã tự tìm ra
KHÔNG được nhắc lại trong danh sách dưới (chỉ ghi ở bảng chữ ký, mục 2).

Tổng: **4 CHẶN · 11 NẶNG · 8 NHẸ**.

---

## 0. Tóm tắt theo mức

| # | Mức | Tệp:dòng | Một câu |
|---|---|---|---|
| C1 | CHẶN | `script\item\lenhbaiadmin.lua:107` | Chuỗi menu có 3 dấu `/`, engine cắt ở dấu **ĐẦU TIÊN** ⇒ cả menu 3HD không mở được |
| C2 | CHẶN | `script\item\hd3_admin.lua:23` | Cùng lỗi: `"...(1082/1192/1193/1217)/HD3_ADM_ST_Task"` |
| C3 | CHẶN | `script\tinhnang\3hoatdong\hd3_driver.lua:23,92,123` | `DynamicExecute("\settings\trigger_challengeoftime.lua")` — `\settings` **không nằm trong cây script được nạp** ⇒ Vượt Ải không bao giờ mở |
| C4 | CHẶN | `script\item\hd3_admin.lua:30,33,52,53` | `AddItem` truyền **6** tham số, engine đòi **≥7** ⇒ 4 nút "Nhận vật phẩm" không cho gì |
| N1 | NẶNG | `hd3_driver.lua:81-101` | `HD3_Tick` không có chốt chống chạy lặp/bỏ lượt; `RunTime` KHÔNG bảo đảm 1 lần/phút |
| N2 | NẶNG | `hd3_driver.lua:113` | `Ladder_ClearLadder(10235)` mỗi 00:00 — bản Linux **không hề xoá** bảng này |
| N3 | NẶNG | `hd3_driver.lua:104-114` | Bảng xếp hạng ngày chỉ **thông báo**, không trao thưởng; chú thích config nói ngược lại |
| N4 | NẶNG | `hd3_driver.lua:60` · `autoexec_npc_hd3.lua:22` | Đặt sai **camp** cho NPC chức năng (1 và 0, đáng lẽ 6) |
| N5 | NẶNG | `hd3_thuyenphu.lua:9` | `BOATID` là biến toàn cục dùng chung ⇒ 2 người chơi tranh nhau, lên nhầm bến |
| N6 | NẶNG | `hd3_driver.lua:28-31` | 3 dòng cuối là toạ độ thuyền phu **bờ BẮC** (vốn để "trở về bờ nam"), nay thành điểm lên thuyền và lặp BOATID |
| N7 | NẶNG | `cauhinh_hoatdong.lua:255-303` | **9/14 khoá HD3_ là khoá chết** — không ai đọc, hoặc chỉ dùng để in chữ |
| N8 | NẶNG | `hd3_admin.lua:53` (+`fld_head.lua:274`) | `6,1,2745` trong JX1 là **"Thùng gỗ"**; Lệnh Bài Thủy Tặc thật là `6,1,3363` — sót ánh xạ |
| N9 | NẶNG | `fld_head.lua:197` (đường Include của `hd3_thuyenphu`) | `parttype == 196` → JX1 `6,1,196` = "Thượng Thiên lệnh"; "Mật đồ thần bí" là `6,1,195` |
| N10 | NẶNG | `hd3_admin.lua:27` | Nút "Sinh lại NPC" gọi lại `HD3_DriverInit()` mà không dọn NPC cũ ⇒ chồng NPC |
| N11 | NẶNG | `hd3_driver.lua:93` | Dùng `AddLocalCountNews` (chỉ máy chủ này) thay `AddGlobalCountNews`, và bỏ `Msg2SubWorld` mà bản JX1 gốc có |
| L1..L8 | NHẸ | (mục 4) | thoát `\h` trong chuỗi, biến chết, ngũ hành ngẫu nhiên, tên NPC không dấu, ngữ nghĩa `HD3_VA_GIO_XEPHANG`, chú thích sai nguồn, `Include` không cache 384 KB, tiền tố `g_IsJx2Script` chết |

---

## 1. Kiểm điểm 9 câu hỏi của đề bài

| # | Câu hỏi | Kết luận |
|---|---|---|
| 1 | Mọi hàm được gọi có tồn tại không? | **CÓ hết** (bảng mục 2). `SayEx` **không phải hàm engine** — nó là hàm script ở `script\lib\lib_ham.lua:72`, tới được vì `lenhbaiadmin.lua:5` Include trước `:38` |
| 2 | Số tham số có khớp chữ ký C không? | **KHÔNG** ở 5 chỗ: `GetNpcValue()` (phiên chính đã biết) + **4 lời gọi `AddItem` 6 tham số** (C4) |
| 3 | Bao đóng Include của 4 tệp mới có tồn tại? | **CÓ hết.** `\script\lib\file.lua` không có thật nhưng `sJX2RemapScriptPath` (`ScriptFuns.cpp:1938-1967`) đổi sang `scriptjx2\lib\file.lua` — có. Chi tiết mục 3 |
| 4 | `Include` lúc chạy trong `HD3_DriverInit` chạy được không? | **Được** — `LuaIncludeFile` chỉ là `lua_dofile`, gọi lúc nào cũng được. Nhưng **không cache** (L7) và **gọi lại là sinh NPC lần nữa** (N10) |
| 5 | `HD3_DriverInit()` đặt đúng chỗ chưa? `addnpcphonglangdo` còn ai gọi? | **ĐÚNG.** `startgame.lua:23` Include lúc nạp tệp, `:103` gọi trong `OnGame` sau các `AddNpcNew` khác. `grep` toàn cây: chỉ còn `startgame.lua:102` đã chú thích + `lib_phonglangdo.lua:147` (định nghĩa) + 4 tệp trong `_backup_*` ⇒ **sạch** |
| 6 | `RunTime` có thật sự chạy mỗi phút? | **KHÔNG BẢO ĐẢM** — xem N1. Không phải "2 lần/giờ" mà là "có thể 0, có thể >1 trong cùng phút" |
| 7 | Mục menu `lenhbaiadmin` đúng cú pháp? cùng Lua_State? | Cùng state **ĐÚNG** (`lenhbaiadmin.lua:38` Include `hd3_admin.lua`, hàm chỉ được gọi lúc chạy). **Cú pháp SAI** — C1 |
| 8 | Khối `[6]` có nằm trong `tbCHD`? | **CÓ** — `[6]` ở dòng 246-303, dấu `}` đóng bảng ở **dòng 313**. `HD_CFG` đọc được ✓ (nhưng phần lớn khoá không ai gọi — N7) |
| 9 | `syncheck.exe` 8 tệp | **8/8 `cú pháp OK`** (kết quả nguyên văn ở mục 5) |

---

## 2. Bảng hàm được gọi trong 4 tệp mới — nguồn & chữ ký

| Hàm | Nguồn | Có? | Chữ ký JX1 (tệp:dòng) | Lời gọi trong tệp mới | Khớp? |
|---|---|---|---|---|---|
| `SayEx` | **script** `lib\lib_ham.lua:72` | ✅ | `SayEx(bảng)` → dựng `Say(msg, n, opt...)` | `hd3_admin.lua:9,18,41,56` | ✅ (nhưng nội dung option sai — C2) |
| `Say` (qua SayEx) | `ScriptFuns.cpp:14356 LuaSelectUI` | ✅ | `≥2` tham số; tách hàm bằng **`strstr(pAnswer,"/")` = dấu `/` ĐẦU TIÊN** (`:717`) | gián tiếp | ❌ C1/C2 |
| `Msg2Player` | `:14465 LuaMsgToPlayer` | ✅ | (chuỗi) | `hd3_admin` nhiều chỗ | ✅ |
| `AddItem` | `:14510 LuaAddItem` (thân `:4827`) | ✅ | **`if (nParamNum < 7) → trả 0`** (`:4838`) | `hd3_admin.lua:30,33,52,53` — **6 tham số** | ❌ **C4** |
| `GetTask` / `SetTask` | `:14458/:14459` | ✅ | 1 / 2 tham số | `hd3_admin.lua:35,38` | ✅ |
| `NewWorld` | `:14499 LuaEnterNewWorld` (`:4114`) | ✅ | `≥3` | `hd3_admin.lua:28,50,51` (3) | ✅ |
| `AddNpcEx` | `:14592` (thân `:6937`) | ✅ | `≥6`; `(id,lv,series,swIdx,x,y,[camp],[tên],[bỏ qua])` | `hd3_driver.lua:60`, `autoexec_npc_hd3.lua:22` (8) | ✅ cú pháp / ❌ **giá trị camp — N4** |
| `SetNpcScript` | `:14597 LuaSetNpcActionScript` (`:7128`) | ✅ | `≥2` | `hd3_driver.lua:62`, `autoexec_npc_hd3.lua:24` | ✅ |
| `SetNpcValue` | `:14605 LuaSetNpcParam` (`:7307`) | ✅ | `≥2`; 2 tham số → `m_nNpcParam[0]` | `hd3_driver.lua:63` | ✅ |
| `GetNpcValue` | `:14607 LuaGetNpcParam` (`:7325`) | ✅ | **`if (nParamNum < 1) → trả 0 GIÁ TRỊ`** | `hd3_thuyenphu.lua:9` — **0 tham số** | ❌ *(phiên chính đã tìm ra)* |
| `SubWorldID2Idx` | `:14574` (`:501`) | ✅ | 1 | `hd3_driver.lua:58`, `autoexec_npc_hd3.lua:20` | ✅ |
| `DynamicExecute` | `:14997 LuaDynamicExecute` (`:2403`) | ✅ | `≥2`; **`g_GetScript` KHÔNG tự nạp** — không thấy thì log "script chua nap, bo qua" | `hd3_driver.lua:86,92,120,123` | ✅ PLD / ❌ **VA — C3** |
| `AddLocalCountNews` | `:14413` (`:1264`) | ✅ | `≥2`; `BroadcastLocalServer` | `hd3_driver.lua:93` (2) | ✅ cú pháp / ⚠ N11 |
| `AddGlobalCountNews` | `:14410` (`:1215`) | ✅ | `≥2`; `BroadcastGlobal` | `hd3_driver.lua:110` (2) | ✅ |
| `Msg2SubWorld` | `:14467 LuaMsgToSubWorld` (`:3670`) | ✅ | `≥1`; phát **toàn server**, không phụ thuộc biến `SubWorld` | `hd3_driver.lua:111` | ✅ |
| `Ladder_GetLadderInfo` | `:15184` (`KJx2SharedStore.cpp:599`) | ✅ | `(id, hạng 1..10)` → **4** giá trị; ô trống trả `("",0,-1,0)` **chứ không nil** | `hd3_driver.lua:105` | ✅ (guard `name ~= ""` đúng) |
| `Ladder_ClearLadder` | `:15183` (`KJx2SharedStore.cpp:580`) | ✅ | `(id > JX2LADDER_MIN_ID)` | `hd3_driver.lua:113` | ✅ cú pháp / ❌ **N2** |
| `Describe` | `:14357` | ✅ | — | **không dùng** trong 4 tệp mới | — |
| `HD_CFG` / `HD_NapLaiCauHinh` | **script** `header\cauhinh_hoatdong.lua:318 / :338` | ✅ | — | `hd3_driver.lua:85,91,98`; `hd3_admin.lua:18,67` | ✅ |
| `add_dialognpc_hd3` / `adddialognpc_hd3` | **script** `global\autoexec_npc_hd3.lua:17 / :7` | ✅ | — | `hd3_driver.lua:42` | ✅ |
| `add_killertasknpc` / `addkillertasknpc` | **script** `task\tollgate\killbosshead.lua:183 / :4` | ✅ | — | `hd3_driver.lua:45` | ✅ |
| `fld_wanttakeboat` | **script** `missions\fengling_ferry\fld_head.lua:38` | ✅ | 1 tham số | `hd3_thuyenphu.lua:11` | ✅ |
| `random`/`getn`/`format`/`floor`/`mod`/`print` | thư viện Lua 4.0 | ✅ | — | — | ✅ |
| `SetNpcParam` (killbosshead:191) | `:14604` | ✅ | `≥2` | 3 tham số | ✅ |

Ghi chú: cả `GameScriptFuns[]` từ `ScriptFuns.cpp:14354` đến `:15467` được đăng ký cho **mọi**
Lua_State (`KSortScript.cpp:160 RegisterFunctions`) ⇒ không có hàm nào "chỉ dành cho state khác".

---

## 3. Bao đóng Include của 4 tệp mới — trạng thái từng đích

| Tệp gọi | Đích Include | Có thật? |
|---|---|---|
| `hd3_driver.lua:17` | `\script\header\cauhinh_hoatdong.lua` | ✅ |
| `hd3_driver.lua:38` (lúc chạy) | `\script\global\autoexec_npc_hd3.lua` | ✅ |
| `hd3_driver.lua:39` (lúc chạy) | `\script\task\tollgate\killbosshead.lua` (384 861 byte) | ✅ |
| `hd3_admin.lua:6` | `\script\tinhnang\3hoatdong\hd3_driver.lua` | ✅ |
| `hd3_thuyenphu.lua:6` | `\script\missions\fengling_ferry\fld_head.lua` | ✅ |
| ↳ `fld_head.lua:2` | `\script\lib\file.lua` | ⚠ **không có ở `script\lib\`**, nhưng `sJX2RemapScriptPath` đổi sang `scriptjx2\lib\file.lua` — **có** ⇒ OK |
| ↳ `fld_head.lua:3,4` | `\script\lib\log.lua`, `\script\lib\awardtemplet.lua` | ✅ (bản ở `script\lib\` thắng; đã kiểm có `tbLog` `:55` và `tbAwardTemplet` `:9`) |
| ↳ `fld_head.lua:6,7,8` | `\script\activitysys\playerfunlib.lua` (`PlayerFunLib` `:10`), `...\fengling_ferry\lang.lua`, `\script\misc\eventsys\type\func.lua` | ✅ |
| đích `DynamicExecute` PLD | `\script\missions\fengling_ferry\fldmap_boat1.lua` (`fenglingdu_main` `:4`) | ✅ **nằm trong `\script` nên có trong cây nạp** |
| đích `DynamicExecute` VA | `\settings\trigger_challengeoftime.lua` (`OnTrigger` `:8`) | ❌ **tệp có nhưng KHÔNG được nạp vào cây script — C3** |
| `hd3_driver.lua:24` | `\script\missions\challengeoftime\rank_perday.lua` | ✅ tệp có, **nhưng biến `HD3_VA_RANK` không được dùng ở đâu (L2)** |

`autoexec_npc_hd3.lua` không Include gì. `hd3_thuyenphu.lua` chỉ Include `fld_head.lua`.

---

## 4. Chi tiết từng phát hiện

### C1 — CHẶN — `script\item\lenhbaiadmin.lua:107`: cả menu 3HD không bấm được

```
"Ho?t ??ng Linux (S?n Boss S?t Th? / Phong L?ng ?? / V??t ?i): test/HD3_AdminMenu",
```

Chuỗi này có **3 dấu `/`**. Engine tách nhãn/hàm ở `ScriptFuns.cpp:717`:

```c
char* pFunName = strstr(pAnswer, "/");          // DẤU / ĐẦU TIÊN, không phải strrchr
...
g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(...[0]));
*pFunName = 0;                                   // nhãn bị cắt tại đây
```

`m_szTaskAnswerFun[MAX_ANSWERNUM][32]` (`KPlayer.h:636`) ⇒ tên hàm callback trở thành
`" Phong Lăng Độ / Vượt ải): tes"` (cắt còn 31 byte), còn nhãn hiện trên hộp thoại chỉ còn
`"Hoạt động Linux (Săn Boss Sát Thủ "`.

Hậu quả: bấm vào là gọi một hàm không tồn tại ⇒ **không mở được `HD3_AdminMenu`, toàn bộ
yêu cầu #5 của chủ game (test từng hoạt động qua Lệnh Bài Admin) chết ngay ở cú bấm đầu tiên.**
Nhãn cụt còn là dấu hiệu nhìn thấy được ngay khi mở lệnh bài.

Đối chiếu: mục kế bên `lenhbaiadmin.lua:100` của đợt 24/08 viết
`"Hoạt động 23-24.08 (BC-BN-TV-TB): bộ test/HD_AdminMenu"` — dùng gạch nối, **không có `/`** ⇒
đợt trước đã né đúng bẫy này.

### C2 — CHẶN — `script\item\hd3_admin.lua:23`: mục "Xem biến nhiệm vụ" chết cùng lý do

```
"Xem bi?n nhi?m v? (1082/1192/1193/1217)/HD3_ADM_ST_Task",
```

Dấu `/` đầu tiên nằm giữa `1082` và `1192` ⇒ callback = `"1192/1193/1217)/HD3_ADM_ST_Task"`.

Đã rà **toàn bộ 18 chuỗi option** còn lại trong `hd3_admin.lua`: chỉ dòng 23 sai. Các chuỗi
`"...trần 8 lần/ngày"` (dòng 18) và `"...10/14/16/18/20h"` (dòng 41) **không sao** vì chúng là
`strSay[1]` — phần **thông điệp**, `SayEx` không tách `/` ở đó.

### C3 — CHẶN — `hd3_driver.lua:23,92,123`: Vượt Ải không bao giờ mở

```lua
HD3_VA_TRIGGER = "\\settings\\trigger_challengeoftime.lua"
...
DynamicExecute(HD3_VA_TRIGGER, "OnTrigger")
```

`LuaDynamicExecute` (`ScriptFuns.cpp:2403`) chỉ tra **cây script đã nạp**:

```c
KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
if (!pScript) { g_DebugLog("[WLLS] DynamicExecute: script chua nap, bo qua: ..."); return 0; }
```

Còn cây script được nạp là **duy nhất hai thư mục** (`KSortScript.cpp:56` + `:66`):

```c
unsigned long nLoaded = LoadAllScript("\\script");
nLoaded = LoadAllScript("\\scriptjx2\\tong_vn");
```

`\settings\` **không** được quét (`g_GetScript` cũng không tự nạp — chú thích ngay tại
`KSortScript.cpp:60`). Cách duy nhất đưa nó vào cây là `ReLoadScript("\\settings\\...")`, và
không tệp nào trong 8 tệp làm việc đó.

Hậu quả: **cả lịch (`HD3_Tick` mỗi giờ :00) lẫn nút admin `HD3_Adm_VA_Now` đều là no-op** —
Vượt Ải không bao giờ mở mission, chỉ để lại một dòng DebugLog. Đối chiếu: mọi `DynamicExecute`
đang chạy tốt trong cây (`startgame.lua:107-111`: `\script\leaguematch\gsdriver.lua`,
`\script\missions\tong\tong_driver.lua`, …) đều trỏ vào **`\script`**.

*(Hệ quả kéo theo: tiền tố `"\\settings\\trigger_"` mới thêm ở `KSortScript.cpp:130` cho
`g_IsJx2Script` là **mã chết** — không Lua_State nào mang tên đó. Xem L8.)*

### C4 — CHẶN — `hd3_admin.lua:30,33,52,53`: 4 nút "Nhận vật phẩm" không cho gì

```lua
for i = 1, 5 do AddItem(6, 1, 398, 90, 0, 0) end          -- :30  (6 tham số)
AddItem(6, 1, 399, 90, 0, 0)                              -- :33
AddItem(4, 489, 0, 0, 0, 0)                               -- :52
AddItem(6, 1, 2745, 0, 0, 0)                              -- :53
```

`LuaAddItem` (`ScriptFuns.cpp:4827`), ngay dòng `:4837-4841`:

```c
int nParamNum = Lua_GetTopIndex(L);
if (nParamNum < 7) { Lua_PushNumber(L,0); return 1; }      // <-- 6 tham số là RA LUÔN
```

Cả 4 lời gọi trả 0, người chơi **không nhận được gì**, nhưng `Msg2Player` ngay sau đó vẫn báo
"Đã nhận …" ⇒ tưởng thành công.

Đây đúng là lỗi mà đợt 24/08 đã gặp và đã ghi chuẩn dự án ngay trong tệp anh em
`script\item\hoatdong_admin.lua:183`:

> `-- bam "nhan item" khong duoc gi. Chuan cua du an: AddItem(genre, detail, particular, ...`

và toàn bộ `hoatdong_admin.lua:198-242` dùng **7 tham số** (`AddItem(6, 1, 4864, 1, 0, 0, 0)`).

Đã kiểm: **không có** hàm Lua nào tên `AddItem` che hàm C (grep `^function AddItem(` toàn
`script\` + `scriptjx2\` = 0 kết quả) ⇒ chắc chắn dùng hàm engine.

### N1 — NẶNG — `hd3_driver.lua:81-101`: `HD3_Tick` không có chốt, `RunTime` không bảo đảm 1 lần/phút

`RunTime` được gọi từ `CoreServerShell.cpp:1161-1176`:

```c
if (!(g_SubWorldSet.GetGameTime() % GAME_FPS))   // GAME_FPS = 18 (KNpc.h:26)
{
    GetSystemTime(&aSysTime);
    if (aSysTime.wSecond == 0)  pTimeScript->CallFunction("RunTime", 0, "");
}
```

`GetGameTime()` là **bộ đếm khung** (`KSubWorldSet.h:34 return m_nLoopRate;`), không phải đồng
hồ thật. Điều kiện là "khung chia hết 18" **giao** với "giây hệ thống = 0":

* Máy chủ **tụt khung** (đúng cảnh 1000 bot đã ghi trong ký ức dự án): mốc chia-hết-18 có thể
  **nhảy qua trọn cửa sổ 1 giây** wSecond==0 ⇒ **RunTime bị bỏ hẳn phút đó** ⇒ lượt Phong Lăng
  Độ / Vượt Ải của giờ đó **không mở**, và không có gì bù.
* Máy chủ chạy **nhanh hơn 18 khung/giây**: mốc rơi nhiều lần trong cùng giây ⇒ `HD3_Tick` chạy
  **nhiều lần trong cùng phút** ⇒ `fenglingdu_main` (Close+OpenMission, reset `MS_STATE`) và
  `OnTrigger` (`ChuangGuan30:KickOutAll()` + `ClearMapNpc`) chạy lại, đá người chơi ra lần nữa.

`HD3_Tick` **không có bất kỳ chốt nào** — chỉ dựa vào `nMi == 0`. Mẫu chuẩn của chính dự án
(`script\timerserver_ctc.lua:194-213`) dùng chốt theo ngày:

```lua
local nToday = tonumber(GetLocalDate("%Y%m%d"))
if (nHr == 18 and g_CTC6_D18 ~= nToday) then g_CTC6_D18 = nToday ... end
```

Cần một biến kiểu `g_HD3_LastHHMM` cho cả 3 nhánh (PLD, VA, xếp hạng).

### N2 — NẶNG — `hd3_driver.lua:113`: xoá bảng xếp hạng mà bản Linux không hề xoá

`Ladder_ClearLadder(10235)` chạy mỗi 00:00. Grep toàn bộ bản giải mã
`ReverseTools\port_3hd\src_utf8\**`: ladder **10235** chỉ xuất hiện ở

* `vuotai\missions\challengeoftime\npc_death.lua:88` — `Ladder_NewLadder(DailyRankLadderId, teamname, -1*laddertime, 1)` (ghi),
* `vuotai\missions\challengeoftime\rank_perday.lua:9,80` — `get_top5team()` **đọc**.

**Không nơi nào trong bản Linux xoá nó.** Chỗ duy nhất có `Ladder_ClearLadder` là
`battlehead.lua:1091` và nó xoá ladder **10250**, hệ khác.

Hậu quả: từ 00:00 mỗi ngày, NPC Nhiếp Thí Trần → "xem bảng xếp hạng" trả
*"Bảng xếp hạng tạm thời chưa có thông tin!"* (`rank_perday.lua:83`) cho tới khi có đội đầu tiên
về đích. Đây là **đổi hành vi so với gốc**, không phải port.

### N3 — NẶNG — `hd3_driver.lua:104-114`: "trao bảng xếp hạng ngày" không trao gì

`HD3_VA_DailyRank()` chỉ đọc hạng 1 rồi `AddGlobalCountNews` + `Msg2SubWorld`. Không có
`AddItem`/`GiveAward` nào. Trong khi `cauhinh_hoatdong.lua:296` ghi:

```
-- Gio bang xep hang ngay (trao "Thien Nien Linh Duoc", HHMM = 0 -> 00:00). [LIVE]
```

Thực tế bản Linux **không có driver phát thưởng**: người chơi tự tới NPC bấm `rank_award()`
(`rank_perday.lua:15-29`), hệ tự kiểm qua `gb_GetTask(nTIMERANK,…)` + task 2636-2639 rồi
`tbAwardTemplet:GiveAwardByList(tbQiannianlingyao, …)`. Vậy:

* chú thích config **sai** (nút không trao gì),
* và `HD3_VA_RANK` (`hd3_driver.lua:24`) trỏ đúng tệp `rank_perday.lua` nhưng **không bao giờ
  được dùng** — phiên trước tự viết lại phần thông báo thay vì gọi mã gốc.

*(Điểm cộng: `rank_perday.lua:13` trên cây sống đã được ánh xạ đúng `6,1,2116 → 6,1,2125` =
"Thiên Niên Linh Dược" — kiểm tận mắt, ✅.)*

### N4 — NẶNG — đặt sai **camp** cho NPC chức năng

`settings\npcs.txt` (id = dòng+2 theo `LuaAddNpcEx`: `nId = FindRow(pName) - 2`):

| id | Tên | Kind | **Camp mặc định** |
|---|---|---|---|
| 240 | Thuyền phu Giáp | 3 | **6** |
| 769 | **Nhiếp Thí Trần** | 3 | **6** |

Bản JX1 gốc cũng truyền 6: `lib_phonglangdo.lua:150-156`
`AddNpcEx1({240},1,{0},...,"Thuyền phu Nam ất",**6**,1)` (tham số 10 = `nCurCamp` →
`SetNpcCurCamp`, `lib\lib_map.lua:91,107`).

Bản port đặt khác:

* `hd3_driver.lua:60` → `AddNpcEx(240, 1, random(0,4), nIdx, x, y, **1**, "Thuyen phu")`
  ⇒ `SetCurrentCamp(1)` = `camp_justice` (chính phái) — `GameDataDef.h:487`.
* `autoexec_npc_hd3.lua:22` → `AddNpcEx(it[1], 1, random(0,4), SId, x, y, **0**, it[6])`
  ⇒ `SetCurrentCamp(0)` = `camp_begin`.

`camp_event = 6` (`GameDataDef.h:492`) là phe "NPC chức năng" mà **toàn bộ** NPC chức năng khác
trong cây đang dùng (`startgame.lua:85-96`: `AddNpcNew(...,6,...)` ở mọi dòng). Camp quyết định
bảng quan hệ `m_RelationTable[..][camp][camp][..]` (`KNpcSet.h:178`) ⇒ 6 thuyền phu và 7 Nhiếp
Thí Trần rơi vào quan hệ phe khác hẳn NPC chức năng bình thường.

Mức thiệt hại chính xác trong game **CHƯA XÁC MINH** (cần thử tại chỗ), nhưng chênh lệch so với
`npcs.txt` **và** so với bản JX1 gốc là chắc chắn và sửa rất rẻ (đổi 2 số thành `6`).

### N5 — NẶNG — `hd3_thuyenphu.lua:9`: `BOATID` là biến toàn cục dùng chung, đua nhau

`main()` đặt `BOATID` rồi gọi `fld_wanttakeboat(BOATID)`. Nhưng `fld_wanttakeboat` **chỉ mở hộp
thoại**; việc dịch chuyển xảy ra ở callback về sau:

```
use_lingpai  →  GiveItemUI(...)  →  (người chơi đặt đồ, bấm OK)  →  exchange_lingpai_1
             →  exchange_lingpai  →  fld_TakeBoat(PlayerIndex)
```

và `fld_TakeBoat` (`fld_head.lua:90-101`) đọc lại **`BOATID`**, cũng như `fld_haveroom`
(`:154-161`). JX1 dùng **một Lua_State cho mỗi tệp script**, dùng chung cho mọi người chơi
(`PlayerIndex` mới là thứ đổi theo lời gọi). Vì cả **6** thuyền phu đều trỏ về cùng
`hd3_thuyenphu.lua`, chỉ cần người B bấm thuyền phu khác trong lúc người A đang mở khung
GiveItemUI là `BOATID` bị ghi đè ⇒ **người A nộp lệnh bài xong bị đẩy sang bến của người B**
(và bị tính suất thuyền của bến đó).

Bản Linux gốc cũng dùng biến toàn cục `BOATID` — nhưng nó tách theo NPC script; ở đây gộp 6 NPC
vào một tệp nên xác suất đụng cao hơn hẳn. Cách chữa an toàn: trong `exchange_lingpai`/
`fld_TakeBoat` lấy lại từ `SetTaskTemp`/`GetTaskTemp` của **người chơi**, hoặc đặt lại `BOATID`
ngay trước khi gọi `fld_TakeBoat`.

### N6 — NẶNG — `hd3_driver.lua:28-31`: 3 thuyền phu bờ Bắc bị đổi vai + trùng BOATID

```lua
HD3_PLD_BOAT = {
    {1147, 3018, 336, 1}, {1280, 2907, 336, 2}, {1535, 2808, 336, 3},
    {1324, 2886, 336, 1}, {1493, 2809, 336, 2}, {1173, 2981, 336, 3},
}
```

Sáu toạ độ này đúng bằng `lib_phonglangdo.lua:79-86 TAB_NPCCHUCNANG[1..6]`. Nhưng trong JX1,
**3 dòng đầu** dùng `TAB_NPCCHUCNANG_SCRIPT[1] = thuyenphu.lua` (bờ Nam, lên thuyền) còn
**3 dòng cuối** dùng `TAB_NPCCHUCNANG_SCRIPT[2] = thuyenphubac.lua` (bờ **Bắc**: "Trở về bờ
nam" + "Trả nhiệm vụ Tiêu diệt thuỷ tặc" — `thuyenphubac.lua:16-31`), và gốc **không**
`SetNpcValue` cho 3 NPC bờ Bắc.

Bản port cho cả 6 chạy cùng wrapper "lên thuyền" và gán lại BOATID 1/2/3 ⇒
(a) mất hẳn lối "trở về bờ nam" của bản Việt, (b) mỗi BOATID có **2** NPC ⇒ càng dễ dính N5,
(c) người chơi đã sang bờ Bắc có thể lên thuyền lại ngay tại chỗ.

Bản Linux không có NPC bờ Bắc (chỉ có `fld_landingpos`/`fld_landingtimer` tự đóng mission), nên
việc đặt gì ở 3 toạ độ đó là **quyết định thiết kế**, cần chủ game chốt — nhưng ít nhất **không
nên lặp BOATID**.

### N7 — NẶNG — `cauhinh_hoatdong.lua:255-303`: 9/14 khoá `HD3_*` là khoá chết

`grep -rn "HD3_" --include=*.lua` toàn cây sống cho thấy **nơi đọc duy nhất** là
`hd3_driver.lua:85,91,98` và `hd3_admin.lua:18`. Bảng thực tế:

| Khoá | Dòng | Ai đọc | Có tác dụng? |
|---|---|---|---|
| `HD3_PLD_GIO` | 272 | `hd3_driver.lua:85` | ✅ |
| `HD3_VA_GIO` | 289 | `hd3_driver.lua:91` | ✅ |
| `HD3_VA_GIO_XEPHANG` | 297 | `hd3_driver.lua:98` | ✅ (nhưng xem L5) |
| `HD3_ST_CAP_TOITHIEU` | 257 | `hd3_admin.lua:18` | ⚠ **chỉ in ra chữ trong menu** |
| `HD3_ST_MAX_NGAY` | 259 | `hd3_admin.lua:18` | ⚠ **chỉ in ra chữ** |
| `HD3_PLD_CAP_TOITHIEU` | 269 | — | ❌ chết |
| `HD3_PLD_SUC_CHUA` | 274 | — | ❌ chết — `fld_head.lua:153` cứng `>= 100` |
| `HD3_PLD_GIO_TONPHI` | 277 | — | ❌ chết — `fld_head.lua:305-311` cứng `{10,14,16,18,20}` |
| `HD3_VA_CAP_TOITHIEU` | 287 | — | ❌ chết |
| `HD3_VA_PHUT_BAODANH` | 292 | — | ❌ chết |
| `HD3_VA_PHUT_NHIEMVU` | 293 | — | ❌ chết |
| `HD3_VA_NGUOI_TOIDA` | 295 | — | ❌ chết |

BÀN GIAO mục 4 viết *"Đã kiểm: mọi khoá driver/admin dùng đều có trong config (không có khoá
chết)"* — đúng theo chiều đó, nhưng **chiều ngược lại chưa kiểm**: 9/14 khoá (kể cả những khoá
gắn nhãn `[LIVE]`) không nối vào bất kỳ hành vi nào. Yêu cầu #2 của chủ game ("nối
`cauhinh_hoatdong.lua`, config thưởng") coi như **chưa đạt** cho phần lớn số liệu.

### N8 — NẶNG — `hd3_admin.lua:53` + `fld_head.lua:274,280,290,295`: sót ánh xạ `6,1,2745`

Tra `settings\item\magicscript.txt` (Genre 6 / DetailType 1, id = dòng − 2):

| id | Tên trong JX1 |
|---|---|
| 2742 | Bảo Rương Tử Mãng Khô Giới |
| 2743 | Tử Mãng Giám Định Phù |
| 2744 | Tử Mãng Quy Nguyên Phù |
| **2745** | **Thùng gỗ** |
| 3360 | Bảo Rương Vượt Ải |
| 3361 | Bảo Rương Thủy Tặc |
| 3362 | Chìa Khóa Như Ý |
| **3363** | **Lệnh Bài Thủy Tặc** |

`thicong\remap_resolved.json` đã ánh xạ `6,1,2742→3360`, `2743→3361`, `2744→3362` (đều +618)
nhưng **thiếu `2745→3363`**. Vì `fld_head.lua` viết dạng so sánh trần (`particular ~= 2745`),
bộ dò `scan_bareforms.py` bỏ sót.

Hậu quả: trong **giờ tốn phí** (10/14/16/18/20h) thuyền phu đòi **"Thùng gỗ"** thay vì Lệnh Bài
Thủy Tặc; `hd3_admin.lua:53` cũng phát "Thùng gỗ" trong khi nhãn ghi "Lệnh Bài Thủy Tặc"
⇒ vừa hỏng luồng vừa là lỗ hổng (thùng gỗ là vật phẩm phổ thông).

### N9 — NẶNG — `fld_head.lua:197`: sót ánh xạ `6,1,196`

```lua
if (itemgenre == 6 and detailtype == 1 and parttype ==196) then
```

Tra bằng chính chuỗi byte "Mật đồ thần bí" lấy từ `fld_head.lua:59`:
`magicscript.txt` **dòng 197 = `6,1,195` = "Mật đồ thần bí"**. Còn `6,1,196` trong JX1 là
**"Thượng Thiên lệnh"**. Lệch **−1**, đúng chiều mà bảng ánh xạ đã áp cho `6,1,215→214` và
`6,1,399→398` / `6,1,400→399`.

Hậu quả: đường "nộp 200 cuốn Mật đồ thần bí" đòi nhầm Thượng Thiên lệnh.

*(Đã quét toàn bộ các thư mục Linux đã chép — `fengling_ferry`, `challengeoftime`, `boss`,
`killer`, `vng_feature`, `jiefang_jieri` — bằng mẫu `(parttype|particular|detailtype|genre).*[=~]=
[0-9]+`: chỉ còn **2** chỗ sai này; `nieshichen.lua:168 parttype == 398` và
`fld_head.lua:269 detailtype ~= 489` đều đúng.)*

### N10 — NẶNG — `hd3_admin.lua:27`: nút "Sinh lại NPC" chồng NPC

```lua
function HD3_ADM_ST_Boot() HD3_DriverInit() Msg2Player("...") end
```

`HD3_DriverInit()` (`hd3_driver.lua:36-52`) không dọn gì trước khi sinh: mỗi lần bấm là thêm
**7 NPC 769 + toàn bộ bảng `addkillertasknpc` (~160 boss) + 6 thuyền phu** chồng lên bộ đã sinh
lúc boot. Bấm vài lần là NPC chồng nhau tại chỗ và ăn dần `MAX_NPC`.
Nhãn nút ("Sinh lại NPC 769 + 160 boss (boot)") gợi ý là **sinh lại**, không phải sinh thêm.

### N11 — NẶNG — `hd3_driver.lua:93`: thông báo Vượt Ải hẹp hơn bản gốc

* Dùng `AddLocalCountNews(msg, 2)` → `BroadcastLocalServer` (`ScriptFuns.cpp:1350`), trong khi
  `HD3_VA_DailyRank` ngay dưới (dòng 110) lại dùng `AddGlobalCountNews` → `BroadcastGlobal`.
  Không nhất quán.
* Bản JX1 gốc `timerserver.lua:880-881 sukien_vuotai` gửi **cả hai**:
  `AddGlobalCountNews(...,3)` **và** `Msg2SubWorld("<color=green>[Sự Kiện]...")`. Bản port bỏ
  `Msg2SubWorld` ⇒ người chơi không thấy dòng chat.
* Nhánh Phong Lăng Độ (dòng 85-88) **không tự thông báo**, nhưng cái này **không sao**:
  `fldmap_boat1.lua:22 fenglingdu_main()` đã `AddGlobalCountNews(str, 3)` bên trong. ✅

### L1 — NHẸ — `hd3_admin.lua:68`: chuỗi bị nuốt dấu `\`

```lua
Msg2Player("... (script\header\cauhinh_hoatdong.lua).")
```

Lua 4.0 (`Sources\Library\LuaLib\src\llex.c:267`, nhánh `default: save_and_next`) **bỏ dấu `\`**
với escape lạ ⇒ hiện ra `"scriptheadercauhinh_hoatdong.lua"`. Cần `\\`.

### L2 — NHẸ — `hd3_driver.lua:24`: `HD3_VA_RANK` khai báo nhưng không dùng ở đâu (xem N3).

### L3 — NHẸ — `hd3_driver.lua:60` + `autoexec_npc_hd3.lua:22`: `random(0,4)` ngũ hành cho NPC
chức năng. Bản JX1 gốc dùng `{0}` cố định (`lib_phonglangdo.lua:150`). Quy tắc vá
`AddNpc → AddNpcEx + random(0,4)` (BÀN GIAO B2.1) sinh ra để chữa "cả đàn quái hệ Kim" — áp cho
NPC **thoại** là thừa và làm biểu tượng ngũ hành nhảy mỗi lần khởi động.

### L4 — NHẸ — `hd3_driver.lua:60`: cả 6 thuyền phu đặt tên `"Thuyen phu"` — **không dấu** và
**giống hệt nhau**. Gốc JX1: "Thuyền phu Nam ất / bính / giáp", "Thuyền phu Bắc ất / bính /
giáp" ⇒ người chơi không phân biệt được bến nào.

### L5 — NHẸ — `hd3_driver.lua:98`: `if (nHHMM == HD_CFG("HD3_VA_GIO_XEPHANG", 0))` so theo
**HHMM**, còn chú thích `cauhinh_hoatdong.lua:296` gọi là "Giờ". Đặt `3` sẽ khớp **00:03** chứ
không phải 03:00 — bẫy cho người vận hành. (Với giá trị mặc định 0 thì đúng.)

### L6 — NHẸ — `autoexec_npc_hd3.lua:3`: chú thích *"Trích NGUYÊN BYTE từ bản Linux
(autoexec_npc.lua)"* — nhưng trong bộ giải mã `src_utf8\**` **không có tệp `autoexec_npc.lua`**
nào. Nguồn của 7 toạ độ NPC 769 (`{769,11,3210,4974}` …) **CHƯA XÁC MINH**. *(Template 769
trong `settings\npcs.txt` đúng là "Nhiếp Thí Trần", Kind 3 — phần đó ✅.)*

### L7 — NHẸ — `hd3_driver.lua:38-39`: `Include` lúc chạy **không có cache**
(`LuaIncludeFile`, `ScriptFuns.cpp:1969-2026`, luôn `lua_dofile`). Mỗi lần `HD3_DriverInit()`
chạy là đọc lại **384 861 byte** `killbosshead.lua` từ đĩa và đổ toàn bộ bảng vào state đang
gọi. Với đường admin (N10) thì state của `lenhbaiadmin.lua` cũng gánh 384 KB này.
*(Sức chứa cây script vẫn ổn: 2 893 tệp `\script` + 66 tệp `\scriptjx2\tong_vn` = 2 959 /
`MAX_SCRIPT_IN_SET 5000`.)*

### L8 — NHẸ — `KSortScript.cpp:126-130` (khối `[3HD 25/08]` trong `g_IsJx2Script`):
* `"\\settings\\trigger_"` **không bao giờ khớp** — không Lua_State nào mang tên đó (hệ quả của C3).
* `"\\script\\tinhnang\\3hoatdong\\"` (state thật của `hd3_driver.lua`) **không có trong danh
  sách**. Vì `Include` chạy trong state của **tệp gọi**, `killbosshead.lua` khi được Include từ
  `hd3_driver` (hoặc từ `startgame` / `lenhbaiadmin`) sẽ chạy dưới **ngữ nghĩa JX1**, không phải
  JX2. Hiện **vô hại** (chỗ duy nhất phân nhánh là `AddNpcEx` tham số 7, mà `killbosshead.lua:189`
  truyền `Tab3[i][6] = 0`), nhưng là bẫy nếu sau này thêm hàm phân nhánh.

---

## 5. Kết quả `syncheck.exe` (Lua 4.0 của chính engine JX1) — 8/8

```
cu phap OK   ...\script\tinhnang\3hoatdong\hd3_driver.lua
cu phap OK   ...\script\item\hd3_admin.lua
cu phap OK   ...\script\global\autoexec_npc_hd3.lua
cu phap OK   ...\script\missions\fengling_ferry\hd3_thuyenphu.lua
cu phap OK   ...\script\startgame.lua
cu phap OK   ...\script\timerserver.lua
cu phap OK   ...\script\header\cauhinh_hoatdong.lua
cu phap OK   ...\script\item\lenhbaiadmin.lua
```

Cú pháp sạch — **mọi lỗi ở trên đều là lỗi ngữ nghĩa / chữ ký / dữ liệu, `syncheck` không thể
bắt được.**

---

## 6. Những chỗ đã kiểm và **ĐÚNG** (để phiên chính khỏi soát lại)

* `hd3_thuyenphu.lua:8 function main(NpcIndex)` — engine **có** truyền chỉ số NPC:
  `KPlayer.cpp:8322` và `:9351` đều `ExecuteScript(Npc[nIdx].m_ActionScriptID, "main", nIdx)`
  ⇒ bản vá `GetNpcValue(NpcIndex)` của phiên chính sẽ chạy đúng.
* `SetNpcValue(npc, v)` 2 tham số → `m_nNpcParam[0]`; `GetNpcValue(idx)` 1 tham số → cùng ô ⇒
  **cùng slot**, không lệch. `killbosshead.lua:191 SetNpcParam(idx,1,…)` dùng ô 1, không đụng.
* `Ladder_GetLadderInfo` ô trống trả `("",0,-1,0)` **chứ không nil** ⇒ guard
  `hd3_driver.lua:106 if (name ~= nil and name ~= "")` là **đúng**.
* `Msg2SubWorld` phát toàn server (`KPlayerChat::SendSystemInfo(0,0,…)`), **không** phụ thuộc
  biến `SubWorld` mà `sukien_tongkim` để lại ⇒ `hd3_driver.lua:111` an toàn.
* Khối `[6]` nằm **trong** bảng `tbCHD` (đóng `}` ở `cauhinh_hoatdong.lua:313`) ⇒ `HD_CFG` đọc được.
* `startgame.lua:23` Include (lúc nạp tệp) → `:103` gọi `HD3_DriverInit()` trong `OnGame`:
  thứ tự đúng. `-- addnpcphonglangdo()` (`:102`) đã tắt; **không còn nơi nào gọi** ngoài các
  thư mục `_backup_*`.
* `hd3_admin.lua` và `lenhbaiadmin.lua` **cùng một Lua_State** (Include ở `:38`), `SayEx`
  (`lib\lib_ham.lua:72`) đã có sẵn từ `:5` ⇒ gọi được (nếu sửa C1/C2).
* `DynamicExecute` cho **Phong Lăng Độ** (`\script\missions\fengling_ferry\fldmap_boat1.lua`,
  hàm `fenglingdu_main` dòng 4) **hợp lệ** — nằm trong `\script`.
* Item `4,489` = "Lệnh bài Phong Lăng Độ" (`questkey.txt` dòng 491) ✅;
  `6,1,398` = "Sát Thủ lệnh", `6,1,399` = "Sát thủ giản" ✅ (ánh xạ đúng);
  `rank_perday.lua:13` đã đổi `2116 → 2125` = "Thiên Niên Linh Dược" ✅.
* Template NPC khớp `settings\npcs.txt`: 240 "Thuyền phu Giáp" (Kind 3), 769 "Nhiếp Thí Trần"
  (Kind 3), 724 "Thủy tặc", 725 "Boss Thủy tặc đầu lĩnh", 1692 "Thủy Tặc Đại Đầu Lĩnh" ✅.
* `add_dialognpc_hd3(adddialognpc_hd3)` và `add_killertasknpc(addkillertasknpc)`: **cả hàm lẫn
  bảng đều tồn tại** (`autoexec_npc_hd3.lua:17/:7`, `killbosshead.lua:183/:4`) — không phải lỗi
  sao chép như thoạt nhìn.
* Không tệp Lua nào trong cây định nghĩa đè `AddItem`/`AddNpcEx`/`SayEx` gây nhiễu.
* `killbosshead.lua` không đụng tên toàn cục nào với các tệp mà `startgame`/`lenhbaiadmin` đã
  Include (đối chiếu `tinsu_addnpc.lua`: chỉ có `tinsu_*`).

---

## 7. Thứ tự đề nghị sửa (rẻ → đắt)

1. **C1, C2** — bỏ dấu `/` khỏi nhãn (đổi thành `-` hoặc `,`). 2 dòng.
2. **C4** — thêm tham số thứ 7 (`, 0`) cho 4 lời gọi `AddItem`. 4 dòng.
3. **N8, N9** — `2745 → 3363` (5 chỗ trong `fld_head.lua` + 1 trong `hd3_admin.lua`),
   `196 → 195` (1 chỗ). Nhớ cập nhật `thicong\remap_resolved.json` để lần chạy lại không hoàn nguyên.
4. **N4** — đổi camp `1`/`0` thành `6`. 2 dòng.
5. **C3** — hoặc `ReLoadScript("\\settings\\trigger_challengeoftime.lua")` một lần trong
   `HD3_DriverInit`, hoặc dời tệp trigger vào `\script\...` rồi trỏ lại `HD3_VA_TRIGGER`
   (khuyến nghị: dời — đồng bộ với 4 driver port trước, và bỏ luôn tiền tố chết ở
   `KSortScript.cpp:130`, thay bằng đường mới).
6. **N1** — thêm chốt `g_HD3_LastHHMM` cho cả 3 nhánh của `HD3_Tick`.
7. **N10** — `HD3_ADM_ST_Boot` cần dọn NPC cũ, hoặc đổi nhãn thành "sinh THÊM (chỉ dùng 1 lần)".
8. **N2, N3, N7, N11** — quyết định thiết kế, cần chốt với chủ game trước khi sửa.
9. **N5, N6** — sửa BOATID theo người chơi + chốt vai trò 3 NPC bờ Bắc.
10. **L1..L8** — dọn khi tiện.


---

## ĐỐI CHẤT (tác tử độc lập)

Người kiểm chứng: tác tử độc lập, **không** phải người viết A2, **không** phải phiên thi công.
Nguyên tắc: mặc định coi mỗi phát hiện của A2 là SAI cho tới khi tệp gốc / mã C / nhị phân chứng minh ngược lại.
Đã kiểm **17 phát hiện** (C1–C4, N1–N11, L1, L8). Kết quả: **2 SAI · 3 THỔI PHỒNG · 1 HẠ THẤP · 11 ĐÚNG**.
Ngoài ra tìm thêm **6 chỗ chính vòng soát A2 bỏ sót**.

**Lỗ hổng phương pháp của A2 (gốc của cả 2 phát hiện SAI):** A2 đối chiếu với `ReverseTools\port_3hd\src_utf8\**`
(chỉ là *tập con đã chép*) và với script JX1-VN cũ, nhưng **chưa bao giờ mở tầng relay Linux**
`D:\ServerLinux\gateway\s3relay\relaysetting\task\*` — đúng cái tầng mà `hd3_driver.lua` được viết ra để thay thế.
N2 và N11 đều nằm ở đó, và cả hai đều chứng minh bản port là **trung thành**, không phải lệch.

### Bảng đối chất

| Phát hiện | Bằng chứng gốc (tệp:dòng) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|
| **C1** menu 3HD có 3 dấu `/` | `ScriptFuns.cpp:717` `strstr(pAnswer,"/")` (dấu ĐẦU); `KPlayer.h:636` `m_szTaskAnswerFun[..][32]`; `lenhbaiadmin.lua:107` xác nhận có 3 dấu `/`; `lib_ham.lua:72-88 SayEx` dựng `Say(msg,n,opt…)` ⇒ nhánh `bStringTab=FALSE`, tách đúng như A2 mô tả | **ĐÚNG** (nhưng thu hẹp phạm vi) | Giữ **CHẶN** — nhưng ghi rõ chỉ chặn **đường test bằng Lệnh Bài Admin** (yêu cầu #5), **KHÔNG** chặn 3 hoạt động khi chạy theo lịch. Cách sửa (đổi `/` thành `-`) ĐÚNG. |
| **C2** `hd3_admin.lua:23` "(1082/1192/1193/1217)" | Cùng cơ chế `:717`. Đã tự quét lại **toàn bộ 105 tệp** (101 tệp chép + 4 tệp mới + `killbosshead` + `nieshichen` + `lenhbaiadmin`) bằng mẫu "chuỗi có >1 dấu `/` và đuôi là định danh": **chỉ ra đúng 2 chỗ C1+C2**, không sót chỗ nào | **THỔI PHỒNG** | Hạ xuống **NẶNG**: chỉ chết **một mục con** "Xem biến nhiệm vụ" trong `HD3_ADM_ST()`; các mục còn lại của menu vẫn bấm được. Cách sửa ĐÚNG. |
| **C3** `DynamicExecute("\settings\trigger_challengeoftime.lua")` | `KSortScript.cpp:53-66` `g_IniScriptEngine` chỉ `LoadAllScript("\\script")` + `LoadAllScript("\\scriptjx2\\tong_vn")`; `ScriptFuns.cpp:2411-2419` `g_GetScript` trả NULL → log rồi `return 0`; grep toàn cây: **không tệp nào** `ReLoadScript` đường `\settings\` | **ĐÚNG — CHẶN** | Giữ CHẶN. **Nhưng đảo lại thứ tự khuyến nghị của A2 (mục 7.5).** Đã kiểm `KSortScript.cpp:347 ReLoadScript` → `UnLoadScript` + **`LoadScriptToSortListA`** (`:246`) = **THÊM MỚI vào cây**, không đòi hỏi tệp phải có sẵn ⇒ **phương án 1 (`ReLoadScript("\\settings\\trigger_challengeoftime.lua")` một lần trong `HD3_DriverInit`) chạy được ngay, KHÔNG cần build lại DLL**. Phương án "dời tệp vào `\script`" mà A2 khuyến nghị lại **bắt buộc sửa `KSortScript.cpp:130` ⇒ build + swap CoreServer.dll + restart** — đắt hơn hẳn. Thân `trigger_challengeoftime.lua` đã kiểm: chỉ có 2 `Include` + 1 `function`, dòng `--OnTrigger();` đã chú thích ⇒ nạp vào cây **không có tác dụng phụ lúc boot**. |
| **C4** `AddItem` 6 tham số | `ScriptFuns.cpp:4827 LuaAddItem`, `:4837-4841` `if (nParamNum < 7) { Lua_PushNumber(L,0); return 1; }`; tham số 7 = `nItemLevel[0]` (`:4863`) | **ĐÚNG — CHẶN** | Giữ. Cách sửa (thêm `, 0`) ĐÚNG. **Bổ sung**: dòng `:53` phải sửa **đồng thời** `2745 → 3363` (xem N8), nếu không nút vẫn phát "Thùng gỗ". |
| **N1** `HD3_Tick` không có chốt | `CoreServerShell.cpp:1164-1171`; `KNpc.h:26 #define GAME_FPS 18`; `KSubWorldSet.h:34 GetGameTime(){return m_nLoopRate;}` + `KSubWorldSet.cpp:91 m_nLoopRate++` ⇒ đúng là **bộ đếm khung**, không phải đồng hồ | **ĐÚNG** (kèm đính chính) | Giữ **NẶNG**, nhưng phải ghi: rủi ro này **có sẵn cho MỌI mốc trong `RunTime`**, không riêng 3HD — `timerserver.lua:40` (`nHr==0 and nMi==00`, thông báo ngày mới + `UpdateNgayMoiAllPlayer`) cũng **không có chốt**. Chốt `g_HD3_LastHHMM` vẫn là cách sửa đúng, nhưng đừng trình bày như lỗi do phiên 25/08 gây ra. |
| **N2** "bản Linux **không hề xoá** ladder 10235" | 🔴 **`D:\ServerLinux\gateway\s3relay\relaysetting\task\challegeoftime-dailyrank.lua:26` = `Ladder_ClearLadder(10235);`** (và bản Việt `…\task\vuotai\challegeoftime-dailyrank.lua:19` y hệt). Tệp đó: `TaskTime(0,0)` + `TaskInterval(1440)` = **00:00 mỗi ngày** — đúng bằng `HD3_VA_GIO_XEPHANG = 0`. Thân hàm gốc: `Ladder_GetLadderInfo(10235,1)` → `value*(-1)` → `format("%s phút %s giây",…)` → `AddGlobalNews(msg,10)` + `Msg2SubWorld(msg)` → `Ladder_ClearLadder(10235)` — **trùng từng bước với `hd3_driver.lua:104-114`** | 🔴 **SAI** | **XOÁ N2.** Bản port là bản sao **trung thành** của relay gốc. A2 chỉ grep `src_utf8\**` (tập con đã chép) nên không thấy tầng relay. |
| **N3** "trao bảng xếp hạng ngày không trao gì" | Cùng bằng chứng N2: relay gốc **cũng chỉ thông báo**, thưởng do người chơi tự lấy ở NPC (`rank_perday.lua:15 rank_award`, `:80 get_top5team`) | **THỔI PHỒNG** | Hạ xuống **NHẸ**, và đổi nội dung: lỗi thật **chỉ là chú thích sai** ở `cauhinh_hoatdong.lua:296` ('trao "Thiên Niên Linh Dược"'). Bỏ câu "phiên trước tự viết lại phần thông báo thay vì gọi mã gốc" — sai sự thật. (`HD3_VA_RANK` chết vẫn giữ, đã có ở L2.) |
| **N4** camp NPC 240 / 769 | `npcs.txt`: 240 "Thuyền phu Giáp" **Kind 3 / Camp 6**, 769 "Nhiếp Thí Trần" **Kind 3 / Camp 6** ✓; `lib_phonglangdo.lua:150-156` truyền 6 ✓; `lib_map.lua:91-109 AddNpcEx1` → `SetNpcCurCamp` ✓. **NHƯNG**: `GameDataDef.h:1372 kind_dialoger = 3`, và `KNpcSet.cpp:129-131` — **luật ĐẦU TIÊN** của `GenOneRelation` là `if (Kind1 == kind_dialoger \|\| Kind2 == kind_dialoger) return relation_dialog;` — **chạy TRƯỚC mọi phép thử camp**. `KNpc.cpp:6124` bảng màu theo camp bị bọc trong `if (m_Kind == kind_player)` | **THỔI PHỒNG** | Hạ xuống **NHẸ**. Với NPC `Kind=3`, camp **hoàn toàn trơ** — không đổi quan hệ, không đổi màu tên. Vẫn nên đổi `1`/`0` → `6` cho nhất quán, nhưng **không phải NẶNG**, và A2 tự ghi "CHƯA XÁC MINH" thì đáng lẽ phải xếp NHẸ ngay từ đầu. |
| **N5** `BOATID` toàn cục đua nhau | `hd3_thuyenphu.lua:9` (1 state cho cả 6 NPC); `fld_head.lua:90-101 fld_TakeBoat` và `:152-165 fld_haveroom` đọc lại `BOATID`; chuỗi callback `use_lingpai → exchange_lingpai_1 → exchange_lingpai → fld_TakeBoat` xác nhận có độ trễ người chơi | **ĐÚNG — NẶNG** | Giữ. **Củng cố cách sửa**: bản JX1 gốc `thuyenphu.lua:13` dùng đúng `SetTaskTemp(TMP_INDEX_NPC, NpcIndex)` (theo **người chơi**) ⇒ `SetTaskTemp/GetTaskTemp` là chuẩn có sẵn của dự án, nên chọn phương án đó chứ không phải "đặt lại BOATID ngay trước khi gọi". |
| **N6** 3 toạ độ bờ Bắc + trùng BOATID | `lib_phonglangdo.lua:79-86` TAB_NPCCHUCNANG khớp 6 toạ độ ✓; `:150-152` dùng `SCRIPT[1]` + `SetNpcValue 1/2/3`; `:154-156` dùng `SCRIPT[2]` và **không** `SetNpcValue` ✓; `thuyenphubac.lua:16` "Trở về bờ nam", `:18` "Trả nhiệm vụ", `:34-43 vebonam()` → `Pay(MONEY_VETHANH)` + `NewWorld(336,…)` ✓ | **ĐÚNG — NẶNG** | Giữ nguyên, kể cả kết luận "phải để chủ game chốt vai trò 3 NPC bờ Bắc". |
| **N7** 9/14 khoá `HD3_*` chết | grep toàn cây: ngoài `cauhinh_hoatdong.lua` và `hd3_driver.lua`, chỉ `hd3_admin.lua:18` đọc `HD3_ST_*` ✓; `fld_head.lua:153` cứng `>= 100` ✓; `fld_head.lua:302-317 check_new_shuizeitask` cứng `{10,14,16,18,20}` ✓ | **ĐÚNG — NẶNG** | Giữ. |
| **N8** `6,1,2745` = "Thùng gỗ" | `settings\item\magicscript.txt` dòng **2747** = `6,1,2745` **"Thùng gỗ"**; dòng **3365** = `6,1,3363` **"Lệnh Bài Thủy Tặc"** (đã kiểm tên là **duy nhất** trong bảng); `thicong\remap_resolved.json` (42 mục) có `2742→3360`, `2743→3361`, `2744→3362` nhưng **KHÔNG có `2745`**; `src_utf8\phonglangdo\…\fld_head.lua:274,280,290,295` **giống hệt từng byte** với bản sống ⇒ đúng là chưa ánh xạ | **ĐÚNG** — và nên **NÂNG mức** | Nâng lên **CHẶN có điều kiện**: trong 5 khung "giờ tốn phí" (10/14/16/18/20h) `fld_wanttakeboat` chỉ chào **một** lựa chọn `use_suizeilingpai`, mà `exchange_lingpai(…,2)` lại đòi `particular == 2745` ⇒ **không ai lên thuyền được trong 5 khung giờ đó**. Danh sách chỗ sửa đầy đủ: `fld_head.lua:274, 280, 290, 295` + `lang.lua:2 MSG_NOTENOUGH_ITEM2745` (tên biến) + `hd3_admin.lua:46` (nhãn) + `:53` (AddItem). Nhớ thêm `"6,1,2745":"6,1,3363"` vào `remap_resolved.json`. |
| **N9** `parttype == 196` | `magicscript.txt` dòng **197** = `6,1,195` **"Mật đồ thần bí"**; dòng **198** = `6,1,196` **"Thượng Thiên lệnh"**; tên "Mật đồ thần bí" **duy nhất** trong bảng (đã quét cả 4867 dòng); `src_utf8\…\fld_head.lua:197` giống hệt bản sống | **ĐÚNG — NẶNG** | Giữ. Sửa `196 → 195` ở `fld_head.lua:197` + thêm `"6,1,196":"6,1,195"` vào `remap_resolved.json`. |
| **N10** nút "Sinh lại NPC" chồng NPC | `hd3_admin.lua:27` gọi `HD3_DriverInit()`; `hd3_driver.lua:36-52` **không** có bất kỳ lệnh dọn/`DelNpc` nào trước khi sinh | **ĐÚNG — NẶNG** | Giữ. |
| **N11** "phải dùng `AddGlobalCountNews` + `Msg2SubWorld` như bản JX1 gốc" | 🔴 **`D:\ServerLinux\gateway\s3relay\relaysetting\task\challengeoftime.lua:34-36`**: `GlobalExecute("dwf \\settings\\trigger_challengeoftime.lua OnTrigger()")` rồi **`GlobalExecute(format("dw AddLocalCountNews([[%s]], 2)", szMsg))`** — **đúng hàm, đúng tham số `2`, KHÔNG có `Msg2SubWorld`**. Bản port sao y | 🔴 **SAI** | **XOÁ N11.** A2 đối chiếu nhầm với `timerserver.lua:880 sukien_vuotai` (tính năng **JX1-VN cũ**, không phải nguồn đang port). Phần còn giữ lại được, hạ xuống **NHẸ**: (a) bản Linux gói trong `dw` nên chạy trên **mọi** GameServer — ở đây 1 GS nên tương đương; (b) chuỗi thông báo của bản port là **ASCII không dấu** ("Nhiem vu 'Thach thuc thoi gian'…") trong khi gốc có dấu đầy đủ — nên chép lại nguyên văn gốc. |
| **L1** `\h` bị Lua 4.0 nuốt `\` | `Sources\Library\LuaLib\src\llex.c:267-268` `default: save_and_next(...)` — nhánh `\\` ngoài đã `next(LS)` nuốt dấu `\` | **ĐÚNG — NHẸ** | Giữ. |
| **L8** "`\script\task\tollgate\` thiếu trong `g_IsJx2Script` — **hiện vô hại** vì `killbosshead.lua:189` truyền `Tab3[i][6] = 0`" | 🔴 Xem mục "HẠ THẤP" ngay dưới bảng | 🔴 **HẠ THẤP (nghiêm trọng)** | Xem dưới. |

### 🔴 HẠ THẤP — L8: chỗ A2 gọi là "vô hại" chính là một CHẶN

A2 mở đúng dòng `killbosshead.lua:189`, đọc đúng giá trị `Tab3[i][6] = 0`, rồi kết luận **"Hiện vô hại"**.
Kiểm ngược lại bằng mã C và tệp gốc Linux thì ngược hẳn:

* Bản Linux gốc (`src_utf8\satthu\task\tollgate\killbosshead.lua:189`) viết
  `AddNpc(Tab3[i][1],Tab3[i][2],Tid,x,y,Tab3[i][6],Tab3[i][7],Tab3[i][8])`.
* Chữ ký `AddNpc` của **chính JX1** (`ScriptFuns.cpp:6877-6907`): tham số **6 = `nSeries`**, tham số 7 = tên,
  và **không hề đụng tới camp** (NPC giữ camp của `npcs.txt`). `lib_map.lua:102` cũng dùng đúng nếp đó.
* Bản sống đã đổi thành `AddNpcEx(…,random(0,4),Tid,x,y,Tab3[i][6],…)` ⇒ giá trị `0` vốn là **NGŨ HÀNH**
  bị đẩy sang **ô thứ 7 của `AddNpcEx` = `SetCurrentCamp`** (`ScriptFuns.cpp:6981-6986`).
* `npcs.txt`: 761/770/771/772/818 (và cả dải 761–820) đều **Kind 0 (`kind_normal`) / Camp 5 (`camp_animal`)**.
* `KNpcSet.cpp:139-143`: `(camp_begin & camp_animal) → relation_enemy` **nhưng** ngay sau đó
  `if (Camp1 == camp_begin || Camp2 == camp_begin) return relation_ally;`
  ⇒ boss bị ép về **camp 0** thì với **mọi** phe người chơi (0/1/2/3/4) đều ra **`relation_ally`**.
* `KPlayer.cpp:9426` `if(!(NpcSet.GetRelation(...) == relation_enemy)) continue;` ⇒ **không chọn được mục tiêu, không đánh được**.

⇒ 160 boss Sát Thủ mà `hd3_driver.lua:45` vừa bật lên **không thể bị đánh** ⇒ **hoạt động (A) chết hoàn toàn**.
Cùng cơ chế: `fengling_ferry\mission.lua:22` truyền **0** cho 30 con Thuỷ Tặc mỗi bến (**CHẶN cho hoạt động B**),
`boss.lua:28/35/44/51/60/67` và `fld_smalltimer.lua:37/44/51/60` truyền **1** (`camp_justice`) — ally với người chơi
**chính phái** và với người **chưa vào môn phái** (`camp_begin`).
Đây đúng là loại lỗi mà A2 mô tả ở **N4** — nhưng A2 gắn NẶNG cho **2 NPC đối thoại (camp trơ)** và gắn "vô hại"
cho **~200 con quái/boss thật**. Xếp hạng bị **đảo ngược**.

*(Ghi nhận: báo cáo anh em `audit\A3_addnpc.md` đã bắt trọn vẹn lỗi này ở mức CHẶN-1 với 3 bằng chứng độc lập —
mục này chỉ để đính chính đánh giá của A2, không tính là phát hiện mới.)*

---

## Bỏ sót của chính vòng soát

### BS1 — 🔴 NẶNG — Ladder **10235** đụng độ với **Liên Đấu WLLS** (`hd3_driver.lua:105,113` · `hd3_admin.lua:64`)

A2 đã kiểm `Ladder_GetLadderInfo` / `Ladder_ClearLadder` về **chữ ký**, nhưng **không kiểm ID 10235 có bị ai khác dùng không**.

* `script\leaguematch\head.lua:75-81`: hạng mục **"Đơn đấu tự do"** có `ladder = 10235`, `mtypes = 2`.
* `script\leaguematch\task.lua:193` `for n_ladder = WLLS_TYPE[n].ladder, …ladder + mtypes - 1` ⇒ WLLS **chiếm 10235 và 10236**;
  `:153 Ladder_NewLadder(n_ladder, str_lgname, -n_lastorder, 0)` là chỗ ghi bảng xếp hạng giải.
  (`script\missions\leaguematch\macthtype\single.lua:15` cũng ghi `ladder = 10235`.)
* `KJx2SharedStore.cpp:440-441` + `s_LadderMap`: **một kho duy nhất** khoá theo id, dùng chung cho mọi hệ.

Hậu quả:
1. `hd3_driver.lua:113 Ladder_ClearLadder(10235)` chạy **00:00 mỗi ngày** ⇒ **xoá sạch bảng xếp hạng
   Đơn Đấu Tự Do của Liên Đấu** (tính năng đã chạy thật từ 20-21/08).
2. Nút admin `hd3_admin.lua:64` "Trao bảng xếp hạng ngày" ⇒ **xoá bảng Liên Đấu ngay lập tức**, bất kể giờ.
3. Chiều ngược lại: `Ladder_NewLadder(10235, <tên bang/đội giải>, -n_lastorder, 0)` của WLLS **chen vào**
   bảng Vượt Ải ⇒ `HD3_VA_DailyRank` có thể phát loa "Chúc mừng đội <tên đội Liên Đấu> hoàn thành
   Thách thức thời gian với thời gian ngắn nhất <…>" với số phút/giây **bịa** (vì `value` là thứ hạng giải, không phải giây),
   và NPC `rank_perday.lua:80 get_top5team()` cũng hiện tên sai.

*(Bản Linux gốc cũng có trùng số này — `D:\ServerLinux\gateway\s3relay\script\leaguematch\head.lua:78` — nhưng ở
Linux hai hệ nằm ở hai relay khác nhau; ở JX1 chúng dùng chung `KJx2SharedStore`, nên trùng số trở thành lỗi thật.)*

**Sửa**: cấp cho Vượt Ải một id còn trống (ví dụ **10400**) và đổi **đồng bộ 4 chỗ**:
`hd3_driver.lua:105` · `hd3_driver.lua:113` · `script\missions\challengeoftime\rank_perday.lua:9 DailyRankLadderId`
· `script\missions\challengeoftime\npc_death.lua:88` (chỗ ghi). Tuyệt đối **không** đổi mỗi `hd3_driver` —
sẽ thành đọc một bảng, ghi một bảng khác.

### BS2 — NHẸ (quy trình) — `settings\task\missions.txt` bị sửa nhưng **không có trong manifest**, và nằm ngoài phạm vi A2

A2 tự khai phạm vi là "4 tệp mới + **4 tệp wiring**". Nhưng còn một tệp wiring thứ **năm** đã bị sửa:

```
E:\...\bin\server\settings\task\missions.txt        (mtime 25/08 01:55)
E:\...\bin\server\settings\task\missions.txt.truoc_3hd_2508   (bản lưu)
```
Khác biệt (đã diff):
* dòng 16: `15  \script\missions\mission_trong.lua` → `15  \script\missions\fengling_ferry\mission.lua`
* dòng 23: `22  \script\missions\mission_trong.lua` → `22  \script\missions\challengeoftime\mission_match.lua`

**Bản thân thay đổi là ĐÚNG** (hai ô 15 và 22 trước đó đều trỏ `mission_trong.lua` = ô trống; `fld_head.lua:10 MISSIONID = 15`
và `challengeoftime` dùng `MISSION_MATCH = 22` cần đúng hai ô này; `ScriptFuns.cpp:11220` đọc `g_MissionTabFile`
để tìm script mission). Lỗi là **quy trình**: tệp **không có** trong `thicong\b1_manifest.txt` (100 dòng) và không được
nhắc trong BÀN GIAO ⇒ chạy lại `b1_copy.py` / `restore_manifest.py` sẽ **không tái lập** được, và ai đó khôi phục
`settings\` từ bản lưu sẽ **âm thầm giết cả hai mission**. Phải thêm vào manifest.

### BS3 — NHẸ — nút `/#tbCOT_Party:CheckCondition()` của NPC Vượt Ải là **nút chết** (`nieshichen.lua:66`)

A2 kết luận "Mọi hàm được gọi có tồn tại: **CÓ hết**", nhưng chỉ soát **4 tệp mới**. Trong 101 tệp chép có một dạng
callback JX2 mà engine JX1 không đỡ được:

* `KPlayer.cpp:7489-7490`: `if (m_szTaskAnswerFun[..][0] == '#') ExecuteScript(script, fun + 1, sel);`
  — chỉ **bỏ dấu `#`**, không thực thi biểu thức.
* `KPlayer.cpp:7013-7051`: `ExecuteScript` có tách `"("` để lấy danh sách tham số, rồi gọi
  `pScript->CallFunction(szFunName, …)`.
* `Engine\Src\KLuaScript.cpp:218`: `CallFunction` = **`Lua_GetGlobal(m_LuaState, cFuncName)`** — tra **toàn cục theo
  chuỗi nguyên**, không hiểu dấu `:`.

`nieshichen.lua:66` (menu "Thử luyện sát thủ" của NPC 769) có mục
`"<#> Kiểm tra điều kiện tổ đội/#tbCOT_Party:CheckCondition()"`. Hàm **có thật**
(`script\vng_feature\challengeoftime\npcNhiepThiTran.lua:3 function tbCOT_Party:CheckCondition()`) nhưng
`Lua_GetGlobal("tbCOT_Party:CheckCondition")` trả **nil** ⇒ `Lua_Call` lỗi ⇒ in `-->Error Execute` và **không làm gì**.

Đã quét toàn bộ 105 tệp: các mục `/#givetask(…)`, `/#showboss()`, `/#showbossnext()`, `/#VnCOTBoxNewAward()`,
`/#storm_end` đều là **hàm toàn cục** nên chạy bình thường — **chỉ duy nhất** dòng 66 dùng dạng phương thức `:`.
**Sửa**: bọc một hàm toàn cục, ví dụ `function COT_CheckCondition() tbCOT_Party:CheckCondition() end`
rồi đổi nhãn thành `…/COT_CheckCondition`.

### BS4 — NHẸ (chú thích sai) — `hd3_driver.lua:49-50` mô tả sai đường vào của NPC báo danh Vượt Ải

```lua
-- (C) Vuot Ai: NPC bao danh Nhiep Thi Tran (npcNhiepThiTran.lua) da gan qua
--     station.lua khi client bam Dich Quan 7 thanh - khong can sinh rieng.
```
grep toàn cây: **không tệp nào** tên `station.lua` (hoặc Dịch Quán) nhắc tới `npcNhiepThiTran` hay `challengeoftime`.
Đường thật là `script\task\tollgate\killer\nieshichen.lua:19 Include("\\script\\vng_feature\\challengeoftime\\npcNhiepThiTran.lua")`
— tức chức năng báo danh nằm **ngay trên 7 NPC 769** do `add_dialognpc_hd3` sinh. Chức năng **chạy được**, chỉ chú thích sai;
nhưng A2 đã chấp nhận câu này mà không kiểm, trong khi nếu nó đúng như viết thì Vượt Ải sẽ **không có NPC báo danh nào** —
đúng loại giả định phải bác bỏ trước.

### BS5 — NHẸ — `HD3_VA_DailyRank` đổi hàm loa so với bản gốc dù JX1 **có** hàm gốc

Bản Linux dùng `AddGlobalNews(msg, 10)`; bản port đổi thành `AddGlobalCountNews(szMsg, 10)` (`hd3_driver.lua:110`).
Hai hàm **khác kiểu loa**: `ScriptFuns.cpp:1133 LuaAddGlobalNews` → `NEWSMESSAGE_NORMAL`, còn
`:1215 LuaAddGlobalCountNews` → `NEWSMESSAGE_COUNTING` (lặp `nTime` lần). Và **`AddGlobalNews` có đăng ký trong JX1**
(`ScriptFuns.cpp:14408`) nên hoàn toàn dùng được. Đây là lệch **cố ý hay vô ý cần chốt**; nếu tiêu chí là "logic 100%
bản Linux" thì phải dùng `AddGlobalNews`.

### BS6 — NHẸ — lệch giữa chú thích và số liệu lịch Phong Lăng Độ (`cauhinh_hoatdong.lua:270-272`)

Chú thích ghi *"Bản Linux relay chạy **mỗi giờ** đúng phút :00"* nhưng `HD3_PLD_GIO` chỉ liệt kê **12 mốc giờ chẵn**
(0,2,4,…,22). Tra relay gốc: có **cả hai** — `fengling_ferry.lua` (`TaskInterval(60)` = mỗi giờ) **và** 12 tệp
`fengling_ferry_0200.lua … _2400.lua` (giờ chẵn). Danh sách hiện tại khớp bộ 12 tệp và khớp `lib_phonglangdo.lua:88-106
TAB_TIME_PLD` của bản JX1 — **số liệu hợp lý**, nhưng chú thích mâu thuẫn với chính nó. Nên sửa chú thích
(và chốt với chủ game: mỗi giờ hay 2 giờ/lượt). **CHƯA XÁC MINH** relay Linux bật bộ nào.

---

### Những chỗ A2 nói ĐÚNG và tôi đã kiểm lại độc lập (không cần soát nữa)

* `Ladder_GetLadderInfo` trả **4 giá trị**, ô trống = `("",0,-1,0)` — `KJx2SharedStore.cpp:596-625` ✓ (guard `name ~= ""` đúng).
* `Msg2SubWorld` phát toàn server, không phụ thuộc biến `SubWorld` — `ScriptFuns.cpp:3670-3677`
  `KPlayerChat::SendSystemInfo(0, 0, …)` ✓.
* `SubWorld` **là** biến toàn cục engine đọc được — `KPlayerDef.h:13 #define SCRIPT_SUBWORLDINDEX "SubWorld"`,
  `ScriptFuns.cpp:482-499 GetSubWorldIndex` ✓ ⇒ `fenglingdu_main` (đặt `SubWorld = boatidx` rồi `OpenMission`) chạy đúng.
* `LuaIncludeFile` = `lua_dofile`, gọi lúc chạy được, có log khi hỏng — `ScriptFuns.cpp:1969-2026` ✓;
  `sJX2RemapScriptPath` (`:1937-1967`) đúng là đổi `script\lib\` → `scriptjx2\lib\` ✓.
* `ReLoadScript` **thêm mới** vào cây script — `KSortScript.cpp:347-357` ✓ (đã dùng để đảo khuyến nghị C3).
* `IncludeLib`/`IL` **có thật** trong engine — `ScriptFuns.cpp:2480 LuaIncludeLib`, đăng ký ở `:14373` và `:14379`.
  3 module mà tệp chép gọi nhưng bảng 21 module **không có** (`NPCINFO`, `TASKSYS`, `PET`) chỉ ghi log
  *"module la […] - bo qua"*; đã kiểm hàm thật của chúng **nằm ở C**
  (`ScriptFuns.cpp:15434 NPCINFO_GetSeries`, `:15433 ITEM_DropRateItem`) ⇒ `kill_level.lua:37` chạy được. **Không phải lỗi.**
* Không tệp nào trong 105 tệp có chuỗi option >1 dấu `/` ngoài C1 và C2 (đã quét bằng máy).
* `settings\task\missions.txt` dòng 15/22 đã trỏ đúng `fengling_ferry\mission.lua` và `challengeoftime\mission_match.lua` ✓
  (nhưng xem BS2 về manifest).

---

### DANH SÁCH CHỐT sau đối chất (14 lỗi thật)

| Mức | Lỗi | Tệp:dòng | Sửa |
|---|---|---|---|
| **CHẶN** | 160 boss Sát Thủ + 90 Thuỷ Tặc bị ép `camp 0` ⇒ `relation_ally` ⇒ không đánh được (A2 gọi nhầm là "vô hại") | `killbosshead.lua:189` · `fengling_ferry\mission.lua:22` (+ `boss.lua`×6, `fld_smalltimer.lua`×4 ở camp 1) | Xem `A3_addnpc.md`. Ngắn gọn: trả ô thứ 7 về đúng nghĩa — hoặc bỏ tham số 7, hoặc đổi `AddNpcEx` để **không** `SetCurrentCamp` khi `g_IsJx2Script(L)` |
| **CHẶN** | Vượt Ải không bao giờ mở: `\settings\` ngoài cây script | `hd3_driver.lua:23,92,123` | `ReLoadScript("\\settings\\trigger_challengeoftime.lua")` một lần trong `HD3_DriverInit` (**không cần build DLL**) |
| **CHẶN** | 4 nút "Nhận vật phẩm" cho 6 tham số | `hd3_admin.lua:30,33,52,53` | thêm `, 0` (tham số 7 = `nItemLevel[0]`) |
| **CHẶN** | Menu test 3HD không mở được (chỉ ảnh hưởng đường Lệnh Bài Admin) | `lenhbaiadmin.lua:107` | bỏ 2 dấu `/` thừa trong nhãn |
| **CHẶN** *(5 khung giờ)* | Giờ tốn phí đòi "Thùng gỗ" thay Lệnh Bài Thủy Tặc ⇒ 10/14/16/18/20h không ai lên thuyền | `fld_head.lua:274,280,290,295` · `lang.lua:2` · `hd3_admin.lua:46,53` | `2745 → 3363`, cập nhật `remap_resolved.json` |
| **NẶNG** | Ladder 10235 đụng Liên Đấu WLLS (xoá bảng giải mỗi 00:00 + lẫn dữ liệu 2 chiều) | `hd3_driver.lua:105,113` · `hd3_admin.lua:64` | đổi sang id trống (vd 10400) **đồng bộ** với `rank_perday.lua:9` + `npc_death.lua:88` |
| **NẶNG** | `BOATID` toàn cục ⇒ 2 người tranh nhau, lên nhầm bến | `hd3_thuyenphu.lua:9` | dùng `SetTaskTemp/GetTaskTemp` theo người chơi (nếp có sẵn: `thuyenphu.lua:13`) |
| **NẶNG** | 3 NPC bờ Bắc bị đổi vai + trùng BOATID 1/2/3 | `hd3_driver.lua:28-31` | tối thiểu bỏ trùng BOATID; vai trò 3 NPC bờ Bắc để chủ game chốt |
| **NẶNG** | "Mật đồ thần bí" tra nhầm `6,1,196` (= Thượng Thiên lệnh) | `fld_head.lua:197` | `196 → 195` |
| **NẶNG** | 9/14 khoá `HD3_*` không nối vào hành vi nào (yêu cầu #2 chưa đạt) | `cauhinh_hoatdong.lua:255-303` | nối `HD3_PLD_SUC_CHUA` vào `fld_head.lua:153`, `HD3_PLD_GIO_TONPHI` vào `:305-311`, v.v. |
| **NẶNG** | Nút "Sinh lại NPC" sinh **thêm**, không dọn ⇒ chồng ~170 NPC mỗi lần bấm | `hd3_admin.lua:27` | dọn trước khi sinh, hoặc đổi nhãn |
| **NẶNG** | `HD3_Tick` không có chốt phút (rủi ro chung của `RunTime`, không riêng 3HD) | `hd3_driver.lua:81-101` | chốt `g_HD3_LastHHMM` cho cả 3 nhánh |
| **NẶNG** | Mục "Xem biến nhiệm vụ" chết vì 3 dấu `/` trong nhãn | `hd3_admin.lua:23` | đổi `/` thành `-` |
| **NHẸ** (gộp) | `\h` nuốt dấu `\` (`hd3_admin.lua:68`) · `HD3_VA_RANK` chết · camp NPC đối thoại 1/0 → 6 (trơ nhưng nên nhất quán) · `random(0,4)` ngũ hành cho NPC thoại · 6 thuyền phu trùng tên không dấu · chú thích sai ở `cauhinh_hoatdong.lua:296` và `hd3_driver.lua:49-50` và `cauhinh_hoatdong.lua:271` · nút `#tbCOT_Party:CheckCondition()` chết · `AddGlobalCountNews` thay `AddGlobalNews` · thông báo Vượt Ải không dấu · `missions.txt` thiếu trong manifest · tiền tố chết `"\\settings\\trigger_"` ở `KSortScript.cpp:130` (sống lại nếu chọn phương án `ReLoadScript`) | — | dọn khi tiện |

**Đã gỡ khỏi danh sách sau đối chất:** N2 (SAI), N11 (SAI), N3 (hạ xuống NHẸ chỉ còn phần chú thích), N4 (hạ xuống NHẸ).
