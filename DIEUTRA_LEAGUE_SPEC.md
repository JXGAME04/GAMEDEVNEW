# DAC TA HE LEAGUE/GLBVALUE/GLBMSTIMER (phien dieu tra 14/08/2026)

Spec goc de viet KJx2League.cpp - moi claim co file:line trong D:\ServerLinux.

Đã đọc xong toàn bộ nguồn. Dưới đây là spec.

---

# 0. Kết luận nền tảng (quyết định thiết kế C++)

**Hệ League gốc là 2 tầng:** relay giữ DB thật; mỗi GS giữ **bản sao (mirror) chỉ-đọc**. Vì vậy:

| Nhóm | Ngữ nghĩa gốc |
|---|---|
| `LG_Get*` / `LGM_Get*` | **ĐỒNG BỘ**, đọc mirror cục bộ trên GS. Không gọi mạng. |
| `LG_Apply*` / `LGM_Apply*` | **BẤT ĐỒNG BỘ**: gửi gói lên relay → relay ghi DB → broadcast cập nhật mirror về mọi GS → GS chạy callback `szCbScript::szCbFunc`. Giá trị trả về chỉ là "đã gửi được / không" (1/0), **không phải kết quả**. |

Bằng chứng đanh thép cho tính bất đồng bộ: comment `misc\league_cityinfo.lua:54-56` ("thao tác này gửi lên Relay, Relay trả về mới sửa thành công… nên phải khoá trạng thái") và mẫu khoá `SetGlbValue(id,1)` trước khi Apply (`league_cityinfo.lua:85,149`) rồi mở khoá `SetGlbValue(id,0)` **trong callback** (`league_cityinfo.lua:60`).

→ **Với engine 1-GS:** ghi ngay vào store C++, rồi **gọi callback ĐỒNG BỘ ngay trong cùng lời gọi** (hoặc cuối frame). Chọn "gọi ngay" thì `league_cityinfo` vẫn đúng (khoá được mở ngay). Nhưng phải chấp nhận thay đổi hành vi: sau `LG_ApplyAddLeague`, `LG_GetLeagueObj` sẽ trả về khác 0 **ngay** (bản gốc trả 0 cho tới khi relay hồi). Không script nào trong cây phụ thuộc vào việc "chưa thấy ngay", nên an toàn.

---

# 1. Bảng đặc tả từng hàm

### 1.1 Vòng đời League object (đối tượng TẠM, chưa đăng ký)

| Hàm | Chữ ký đầy đủ | Trả về | Ngữ nghĩa | Call site |
|---|---|---|---|---|
| `LG_CreateLeagueObj` | `()` — 0 tham số | **1 số**: handle obj tạm (>0); fail → `0`/nil (script luôn kiểm `== 0 or == nil`) | Cấp phát 1 struct rỗng trong pool obj tạm của **Lua state hiện tại**. Không đụng DB. | `league_cityinfo.lua:35`; `citywar_function.lua:245,253`; `citywar_global\head.lua:40`; `gb_taskfuncs.lua:27`; `gb_modulefuncs.lua:62`; `tongwar\head.lua:90`; `npc_shizhe.lua:79,307`; `wulinling\head.lua:36`; `wulin_2nd\head.lua:65,75`; `limitedaward_head.lua:29`; `battlehead.lua:1114`; relay `citywar_clearchallenger.lua:26,34` |
| `LG_SetLeagueInfo` | `(hLeagueObj, nLeagueType, szLeagueName)` — **3** | không dùng (0 hoặc 1) | Ghi (type, name) vào obj tạm. | `league_cityinfo.lua:38`; `citywar_function.lua:246,254`; `head.lua:41`; `gb_taskfuncs.lua:29`; `gb_modulefuncs.lua:64`; `tongwar\head.lua:91`; `limitedaward_head.lua:30`; `battlehead.lua:1115`; relay `citywar_clearchallenger.lua:27,35` |
| `LG_AddMemberToObj` | `(hLeagueObj, hMemberObj)` — **2** | không dùng | Gắn member-obj tạm vào league-obj tạm, để `LG_ApplyAddLeague` tạo league **kèm sẵn thành viên**. | `league_cityinfo.lua:40`; `gb_taskfuncs.lua:31`; `gb_modulefuncs.lua:66`; `infocenter_head.lua:687,759,789` ⚠ |
| `LG_ApplyAddLeague` | `(hLeagueObj [, szCbScript, szCbFunc])` — **1 hoặc 3** | **1 số**: 1 = đã gửi/OK, 0 = fail | ASYNC. Đăng ký league (type,name)+members vào DB. Callback: `fn(nLeagueType, szLeagueName, szMemberName, bSucceed)` — 4 đối, xem `league_cityinfo.lua:47` (`OnBuildCityLeague`) và `gb_taskfuncs.lua:36` (`OnCreateModule`). | 3 đối: `league_cityinfo.lua:42`; `gb_taskfuncs.lua:32`; `citywar_function.lua:247,255`; `head.lua:42`; `tongwar\head.lua:92`; `wulinling\head.lua:38`; relay `citywar_clearchallenger.lua:28,36`. **1 đối**: `gb_modulefuncs.lua:67`; `limitedaward_head.lua:31` |
| `LG_FreeLeagueObj` | `(hLeagueObj)` — **1** | không dùng | Giải phóng obj tạm. Gọi **ngay sau** Apply, tức Apply đã copy dữ liệu. | `league_cityinfo` (không gọi — rò rỉ ở bản gốc); `citywar_function.lua:248,256`; `head.lua:43`; `gb_taskfuncs.lua:33`; `battlehead.lua:1117` |
| `LG_ApplyRemoveLeague` | `(nLeagueType, szLeagueName)` — **2** | không dùng | ASYNC, xoá league + toàn bộ member + task. **Không có tham số callback.** | `leaguematch\task.lua:179`; `limitedaward_head.lua:69` |

⚠ `infocenter_head.lua:687,759,789` truyền **lid đã đăng ký** (từ `LG_GetLeagueObj`) vào `LG_AddMemberToObj` chứ không phải obj tạm. Đây là misuse của bản gốc; vô hại vì `LGM_SetMemberInfo` đã mang đủ (type,leagueName) nên `LGM_ApplyAddMember` tự tìm league. **C++ phải chịu đựng được lời gọi này (không crash, im lặng bỏ qua khi handle không thuộc pool obj tạm).**

### 1.2 Truy vấn League (đồng bộ)

| Hàm | Chữ ký | Trả về | Ghi chú |
|---|---|---|---|
| `LG_GetLeagueObj` | `(nLeagueType, szLeagueName)` — **2** | **1 số**: `lid` (>0) hoặc **`0`** khi không có | Đây là **lid đã đăng ký** (khác không gian ngữ nghĩa với handle obj tạm, dù cùng kiểu số). Call: `league_cityinfo.lua:32,76,140`; `citywar_function.lua:243,251`; `head.lua:38`; `infocenter_head.lua:162,441,681,753,769,784`; `gb_taskfuncs.lua:15,24,47`; `gb_modulefuncs.lua:16,60`; `tongwar\head.lua:41,89,305,530,576`; `npc_shizhe.lua:77,223,305`; `wulinling\head.lua:34`; `limitedaward_head.lua:27,77`; relay `citywar_head.lua:65,117`; relay `citywar_clearchallenger.lua:23,32,45,56` |
| `LG_GetLeagueObjByRole` | `(nLeagueType, szRoleName)` — **2** | **1 số**: lid của league mà role đó là thành viên; `0` nếu không | Tra ngược member→league **trong phạm vi 1 type**. Call: `head.lua:48`; `infocenter_head.lua:548,617,682`; `tongwar\event.lua:59`; `tongwar\head.lua:120,225,369,410,462,463,507`; `npc_shizhe.lua:113,124,243,270,324`; `wulinling\head.lua:44`; `createleague.lua:42,80,95,115,133` |
| `LG_GetFirstLeague` | `(nLeagueType)` — **1** | **1 số**: lid đầu; `0` khi hết | Vòng lặp: `while (n ~= nil and n ~= 0)` → `battlehead.lua:1046-1055`; `wulinling\head.lua:54-66`; `tongwar\head.lua:26`; `limitedaward_head.lua:61-67` |
| `LG_GetNextLeague` | `(nLeagueType, lid)` — **2** | **1 số**: lid kế; `0` khi hết | `battlehead.lua:1054`; `wulinling\head.lua:65` |
| `LG_GetLeagueInfo` | `(lid)` — **1** | **3 giá trị**: `szLeagueName, nCreateTime, nMemberCount` | ⚠ **Không phải (nMod, szName)**. Bằng chứng 3 giá trị: `leaguematch\head.lua:110` (`str_lgname, _, n_membercount`), `leaguematch\league.lua:70,102`, `missions\leaguematch\head.lua:466,664,741,823,920`, `npc\head.lua:121,186`, `npc\helper.lua:331,336`, `npc\officer.lua:458`, `npc\signup.lua:76`, `wulin_final_match\createleague.lua:43` (`leaguename, time, count`), `wulin_notifyrelay.lua:33`. Dùng 1 giá trị: `battlehead.lua:1048`; `tongwar\head.lua:231,371,414,513,586,618,646`; `limitedaward_head.lua:64`. lid xấu → `"" , 0, 0` |
| `LG_GetLeagueCreateTime` | `(lid)` — 1 | 1 số | Có trong cả 2 binary; **0 call site Lua** → stub trả 0 là đủ |
| `LG_GetMemberCount` | `(lid)` — **1** | **1 số** (0 nếu lid xấu) | `infocenter_head.lua:164,446,770`; `tongwar\head.lua:42,313,585`; `npc_shizhe.lua:94,224`; `leaguematch\head.lua:126,138`; relay `citywar_head.lua:66`; relay `citywar_clearchallenger.lua:47,58`; relay `stat_goodssale.lua:51`; `createleague.lua:82,101,121,139` |
| `LG_GetMemberInfo` | `(lid, nIndex)` — **2**, **nIndex 0-based, hợp lệ 0..count-1** | **2 giá trị**: `szMemberName, nJob` | Chỉ số ngoài dải **PHẢI trả `("", 0)`, KHÔNG trả nil** — `infocenter_head.lua:170` lặp `for nindex=0,nlgcount` (thừa 1 vòng) rồi `AppendString(szTongName)`; nil sẽ làm chết script. Call: `infocenter_head.lua:171,453,773`; `tongwar\head.lua:44,592`; `npc_shizhe.lua:96`; `leaguematch\head.lua:131,140`; relay `citywar_head.lua:73,82`; relay `citywar_clearchallenger.lua:49,60`; relay `stat_goodssale.lua:58`; `shitu.lua:119,140,175,327,524,592,703,720,762,782` |
| `LG_GetMemberObj` | `(lid, szMemberName)` (suy đoán) | handle | Có trong binary; **0 call site Lua** → stub |
| `LG_GetMemberJoinTime` | `(lid, ...)` | 1 số | Có trong binary; **0 call site Lua** → stub |

### 1.3 Task của League (2 không gian tên tách biệt: league-task vs member-task)

| Hàm | Chữ ký | Trả về | Call site |
|---|---|---|---|
| `LG_GetLeagueTask` | **QUÁ TẢI 2 DẠNG**:<br>(a) `(lid, nTaskID)` — 2<br>(b) `(nLeagueType, szLeagueName, nTaskID)` — 3 | **1 số**; không có → **`0`** (không nil) | **(a)**: `gb_taskfuncs.lua:18`; `gb_modulefuncs.lua:19`; `battlehead.lua:1049-1052`; `battletest.lua:34`; `tongwar\head.lua:54,343,344,577,578,619,620`; `tongwar_signup.lua:46,47,49`; `wulinling\head.lua:56,59`; `limitedaward_head.lua:79,80,81`; `leaguematch\head.lua:165,167`<br>**(b)**: `league_cityinfo.lua:101,117,133`; **`infocenter_head.lua:557`** (`getSignUpState`); `tongwar\head.lua:65,66,67,243,648,658,723`; `score.lua:55,56,57,58`; `wulinling.lua:63,64`; `leaguematch\head.lua:148` |
| `LG_ApplySetLeagueTask` | `(nLeagueType, szLeagueName, nTaskID, nValue [, szCbScript, szCbFunc])` — **4 hoặc 6** | 1/0 | **4**: `gb_taskfuncs.lua:34`; `gb_modulefuncs.lua:70`; `battlehead.lua:1082,1083`; `tongwar\event.lua:55`; `tongwar\head.lua:94`; `tongwar_signup.lua:50`; `limitedaward_head.lua:31,32,33`; `leaguematch\head.lua:150`; **relay `citywar_head.lua:103,144`**<br>**6**: **`league_cityinfo.lua:87-88`** và **`151-152`** (cb = `OnSetCityLeagueTask`); `tongwar\head.lua:71` (`"",""`); `wulinling.lua:111,114` (`"",""`) |
| `LG_ApplyAppendLeagueTask` | `(nLeagueType, szLeagueName, nTaskID, nDelta [, szCbScript, szCbFunc])` — **4 hoặc 6** | 1/0 | **4**: `gb_taskfuncs.lua:57`; `battlehead.lua:1080,1081,1086,1087`; `score.lua:41-54`; `tongwar\head.lua:248`<br>**6**: `tongwar\head.lua:742`; `wulinling.lua:113` |

Callback dạng 6 tham số: `fn(nLeagueType, szLeagueName, szMemberName, bSucceed)` — 4 đối; xem `league_cityinfo.lua:57` (`OnSetCityLeagueTask` chỉ dùng đối 2 = `szLeagueName`).

### 1.4 Task của Member

| Hàm | Chữ ký | Trả về | Call site |
|---|---|---|---|
| `LG_GetMemberTask` | `(nLeagueType, szLeagueName, szMemberName, nTaskID)` — **LUÔN 4** | **1 số**; không có → **`0`** | `infocenter_head.lua:210,252,292,454,566,623,637,675,694,775`; **relay `citywar_head.lua:83`**; **relay `ladder.lua:63`**; `tongwar\head.lua:372,593`; `npc_shizhe.lua:128`; `leaguematch\head.lua:139,155,166`; `limitedaward_head.lua:90`; relay `stat_goodssale.lua:59,60`; `tonggetaward.lua:26` (2010) & `:46` (2011) |
| `LG_ApplySetMemberTask` | `(nLeagueType, szLeagueName, szMemberName, nTaskID, nValue)` — **5** (không thấy dạng 7) | 1/0 | `tongwar\head.lua:102`; `npc_shizhe.lua:86`; `leaguematch\head.lua:156`; `limitedaward_head.lua:39,40` |
| `LG_ApplyAppendMemberTask` | `(nLeagueType, szLeagueName, szMemberName, nTaskID, nDelta [, szCbScript, szCbFunc])` — **5 hoặc 7** | 1/0 | **7**: **relay `ladder.lua:61`**; **`infocenter_head.lua:261`** (nộp lệnh, +nCount) và **`:701`** (báo danh, −nCount); `tonggetaward.lua:27,47`<br>**5**: **`infocenter_head.lua:589`** (hoàn lệnh, −nFree) và **`:764`** (mồi league 509) |

⚠ `LG_ApplyRemoveMember` **KHÔNG TỒN TẠI** — grep binary `jx_linux_y`/`s3relay_y` chỉ có `LGM_ApplyRemoveMember`. Đừng đăng ký tên đó.

### 1.5 Member object (LGM_*)

| Hàm | Chữ ký | Trả về | Ngữ nghĩa & call site |
|---|---|---|---|
| `LGM_CreateMemberObj` | `()` — 0 | **1 số** handle obj tạm | `league_cityinfo.lua:36`; `head.lua:50`; `infocenter_head.lua:684,756,786`; `gb_taskfuncs.lua:28`; `gb_modulefuncs.lua:63`; `tongwar\event.lua:85`; `tongwar\head.lua:97`; `npc_shizhe.lua:326`; `wulinling\head.lua:46`; `wulin_2nd\head.lua:82`; `limitedaward_head.lua:35` |
| `LGM_SetMemberInfo` | `(hMemberObj, szMemberName, nJob, nLeagueType, szLeagueName)` — **5** | không dùng | Member-obj **tự mang địa chỉ league đích** (type+name) → đó là lý do `LGM_ApplyAddMember` không cần lid. `head.lua:52`; `league_cityinfo.lua:39`; `infocenter_head.lua:686,758,788`; `gb_taskfuncs.lua:30`; `gb_modulefuncs.lua:65`; `tongwar\event.lua:87`; `tongwar\head.lua:99`; `npc_shizhe.lua:327`; `wulinling\head.lua:48`; `wulin_2nd\head.lua:84`; `limitedaward_head.lua:36` |
| `LGM_ApplyAddMember` | `(hMemberObj [, szCbScript, szCbFunc])` — **1 hoặc 3** | **1 số**: `1` = OK. **Có script kiểm `ret == 1`** → `infocenter_head.lua:760,763` | ASYNC. **3**: `head.lua:53`; `infocenter_head.lua:688,760,790`; `tongwar\event.lua:88`; `tongwar\head.lua:100`; `npc_shizhe.lua:328`; `wulinling\head.lua:49`. **1**: `limitedaward_head.lua:37` |
| `LGM_FreeMemberObj` | `(hMemberObj)` — **1** | không dùng | `head.lua:54`; `infocenter_head.lua:689,762,791`; `tongwar\event.lua:89`; `tongwar\head.lua:101`; `npc_shizhe.lua:329`; `wulinling\head.lua:50` |
| `LGM_ApplyRemoveMember` | `(nLeagueType, szLeagueName, szMemberName, szCbScript, szCbFunc, nUnknown)` — **LUÔN 6**, đối 6 luôn `0` | 1/0 | `event\citywar.lua:7` (`LGM_ApplyRemoveMember(509, szLGName, szMemName, "", "", 0)`); `npc_shizhe.lua:149`; **relay `citywar_clearchallenger.lua:53,64`**. Đối thứ 6 chưa xác định ngữ nghĩa (mọi call site = 0) → C++ nhận và bỏ qua |

### 1.6 LG_ApplyDoScript

| | |
|---|---|
| **Chữ ký** | `LG_ApplyDoScript(nLeagueType, szLeagueName, szMemberName, szScriptPath, szFuncName, szParam, szCbScript, szCbFunc)` — **LUÔN 8 tham số** |
| **Trả về** | 1/0 (không call site nào dùng) |
| **Ngữ nghĩa gốc** | Gửi lên relay; **relay chạy `szScriptPath::szFuncName(szParam)` trong Lua state của RELAY** (relay có bản sao script riêng: `gateway\s3relay\script\...`). 3 tham số đầu là ngữ cảnh league/member (thực tế phần lớn là `1,"",""` hoặc `0,"",""` — vô nghĩa). 2 tham số cuối = callback chạy lại trên GS: `fn(szLeagueName, szMemberName, nResult)` — 3 đối, xem `wulin_final_match\createleague.lua:5` (`wl_OnCreateLeague(leaguename, membername, result)`) |
| **Bằng chứng "chạy trên relay"** | `msg2allworld.lua:4` và `event\card.lua:13` gọi `GlobalExecute(...)` — hàm chỉ có trong `s3relay_y`, **không có trong `jx_linux_y`** (grep binary). Và cùng file tồn tại ở cả `server1\script\event\card.lua` lẫn `gateway\s3relay\script\event\card.lua` |
| **Ta chạy cục bộ** | Ánh xạ `\script\...` → cây script của ta rồi `dofile` + gọi hàm với 1 đối chuỗi. **Quan trọng:** với `citywar_global\ladder.lua` phải nạp **biến thể RELAY** (`_RELAY_=1`) chứ không phải biến thể GS |
| **Call site (toàn bộ)** | `infocenter_head.lua` (chỉ qua `citywar_tbLadder:DoRelayScript`, `ladder.lua:36`); `battlehead.lua:1137`; `playerfunlib.lua:603`; `npc_consigner.lua:279`; `npc_lmbiaobox.lua:68`; `heka.lua:74`; `jiefang_jieri\201004\main.lua:223,306,318,319,320,328,346,347,348`; `plant\main.lua:323`; `xunmashu\dialog.lua:185,198`; `actclass.lua:137`; `tongwar\head.lua:424`; `tongwar_gmscript.lua:40`; `wulin_2nd\master.lua:86`; `createleague.lua:28,65`; `card_colork.lua:98`; `card_fireworksk.lua:34`; `card_zhufu.lua:34`; `baijinbaoxiang.lua:11`; `tianziyuxi.lua:148,198,213`; `valentinegift.lua:121` |

---

# 2. "League object" và "Member object" là gì

**Đều là SỐ NGUYÊN (Lua number), không phải bảng, không phải chuỗi.** Có **hai không gian handle khác nhau nhưng cùng kiểu**:

1. **Handle obj TẠM** — do `LG_CreateLeagueObj()` / `LGM_CreateMemberObj()` cấp, chỉ sống trong tiến trình, phải `LG_FreeLeagueObj` / `LGM_FreeMemberObj`. Chỉ dùng làm "form" để điền rồi `Apply*`. Bằng chứng: `citywar_function.lua:245-248` tạo → set → apply → free.
2. **`lid` league ĐÃ ĐĂNG KÝ** — do `LG_GetLeagueObj` / `LG_GetLeagueObjByRole` / `LG_GetFirstLeague` / `LG_GetNextLeague` trả. Dùng cho mọi hàm đọc (`LG_GetLeagueInfo/GetMemberCount/GetMemberInfo/GetLeagueTask` dạng 2 đối).

Kiểm tra thất bại trong script luôn là `== 0` **hoặc** `== nil` (`head.lua:39`, `gb_taskfuncs.lua:16`) và hàm tiện ích `FALSE(v)` coi `nil/0/""` là sai (`citywar_function.lua:230-236`; relay `citywar_head.lua:54-60`). → **C++ trả `0` là an toàn nhất; tuyệt đối không trả chuỗi.**

⚠ **jx2compat hiện tại SAI ở đây**: `LG_GetLeagueObj` trả **chuỗi** `"mod|name"` (`jx2compat.lua:202-208`), `LG_GetFirstLeague`/`LG_GetNextLeague` cũng trả khoá chuỗi (`:236-260`). Nó "may mắn" chạy vì `LG_GetLeagueTask(lid,...)` nối chuỗi lại thành khoá đúng (`:214`), nhưng gãy ngay khi script làm số học/so sánh (`createleague.lua:81` `if (lid > 0)`).

---

# 3. Định danh League / Member

**Khoá kép của league = `(nLeagueType, szLeagueName)`.** `nLeagueType` (script gọi là nMod/LGTYPE) là hằng số toàn cục do thiết kế đặt, `szLeagueName` là chuỗi tự do.

| Type | Hằng | szLeagueName | Member là gì | Nguồn |
|---|---|---|---|---|
| **4** | `LEAGUETYPE_CITYINFO` | `tostring(nCityID)` = `"1".."7"` | chính chuỗi đó (1 member giả) | `league_cityinfo.lua:12,20-22,39` |
| **10** | `TONGWAR_LGTYPE` | tên thành ("Phượng Tường"…) | **tên BANG** | `tongwar\event.lua:4`; `head.lua:99` |
| **500** | `RELAYTASK_LEAGUEID` | tên module | tên module | `gb_taskfuncs.lua:11,29-30` |
| **502** | `RELAYTASK_GAMEMODULE` | tên module | tên module | `gb_modulefuncs.lua:11,64-65` |
| **508** | `LEAGUETYPE_CITYWAR_SIGN` | **`GetCityAreaName(nCityID)`** | **tên BANG báo danh** | `infocenter_head.lua:23`; `citywar_function.lua:238,261-263`; relay `citywar_head.lua:19` |
| **509** | `LEAGUETYPE_CITYWAR_FIRST` | như trên | **tên BANG đang dẫn đầu** | `infocenter_head.lua:24`; relay `citywar_head.lua:20` |
| **535** | `LG_SONGJINHONOUR` | tên NGƯỜI CHƠI | — | `battlehead.lua:1023` |
| **538** | `TIAOZHANLING_LGTYPE` | **hằng `"tiaozhanling"`** (1 league duy nhất toàn server) | **tên BANG** | `citywar_global\head.lua:24-26` |

**Đồng bộ tên thành GS↔relay:** `cityid_to_lgname(i)` GS = `GetCityAreaName(i)` (`citywar_function.lua:261-263`) đọc từ `settings\citywar.ini` `[CityArea] AreaName01..07`; relay dùng `TB_CITYWAR_ARRANGE[i][3]` hard-code cùng chuỗi (`relaysetting\task\citywar_head.lua:22-30` vs `server1\settings\citywar.ini:9-33`: Phượng Tường / Thành Đô / Đại Lý / Biên Kinh / Tương Dương / Dương Châu / Lâm An). **C++ phải khớp byte-for-byte** (chuỗi TCVN3).

**Member định danh bằng `szMemberName` (chuỗi), duy nhất trong 1 league.** Không có id số. `nJob` là thuộc tính phụ (0 = thường, 1 = chủ; `tongwar\head.lua:99,102`; `leaguematch\head.lua:141`).

**Task namespace:** league-task và member-task **tách biệt** — cùng `nTaskID = 1` được dùng cho cả "cờ trạng thái báo danh" (league task, `infocenter_head.lua:557` ↔ relay `citywar_head.lua:103,144`) lẫn "số Khiêu chiến lệnh" (member task, `infocenter_head.lua:20`).

---

# 4. SetGlbValue / GetGlbValue

| | |
|---|---|
| **Chữ ký** | `SetGlbValue(nID, nValue)` → 1 · `GetGlbValue(nID)` → **1 số, mặc định `0`** khi chưa đặt |
| **Có ở đâu** | Chỉ trong `jx_linux_y` (**KHÔNG có trong `s3relay_y`** — grep binary) → **kho int per-GameServer, trong RAM, KHÔNG persist, KHÔNG liên-GS** |
| **Số call site** | **323** dòng trong `server1\script` |
| **Dải id thực tế** | id nhỏ: 2, 3–6, 8, 9, 20–29, 30, 31, 35, 46 · 801–805 · 811–819 · 820, 824, 825 · **840–846** · 848, 850 · 919, 920, 921 · 1245, 1252, 1253, 1283. **Max quan sát = 1283** → mảng 2048 int là dư an toàn |

**Call site liên quan Công thành (đúng như bạn hỏi, `misc\league_cityinfo.lua`):**

| Dòng | Nội dung |
|---|---|
| `league_cityinfo.lua:18` | `CITYINFO_GLBVALUEID = {840,841,842,843,844,845,846}` — 7 thành |
| `league_cityinfo.lua:60` | `SetGlbValue(CITYINFO_GLBVALUEID[tonumber(szLeagueName)], 0)` — **mở khoá, trong callback** `OnSetCityLeagueTask` |
| `league_cityinfo.lua:68` | `SetGlbValue(..., 0)` — khởi tạo trong `buildAllCityInfoLeague` (gọi từ `autoexec.lua:147`) |
| `league_cityinfo.lua:85` | `SetGlbValue(..., 1)` — **khoá trước** `LG_ApplySetLeagueTask` 6 đối (`set_citybonus_date`) |
| `league_cityinfo.lua:95` | `GetGlbValue(...) == 1` → đang chờ relay ⇒ trả giá trị tạm `get_maincity_deadline` |
| `league_cityinfo.lua:111` | như trên ⇒ trả `date("%Y%m%d")` |
| `league_cityinfo.lua:127` | như trên ⇒ trả hằng `100` |
| `league_cityinfo.lua:149` | `SetGlbValue(..., 1)` — khoá trước Apply trong `set_citybonus_task` |

Ngoài Công thành, các nơi khác đáng chú ý: `tongwar_signup.lua:53` (`GetGlbValue(850) ~= 1`), `leaguematch` (`GLB_WLLS_TIME = 825`), `battlemain.lua:6,11` (`GLB_FORBIDBATTLE=31`, `GLB_BATTLESTATE=30`), `shangchenghead.lua:50` (`arraycitytoglobalvalue` = 20..26 theo thành), `recoin_goldenequip.lua:80-81` (919, 920), `christmastree.lua:1-2` (8, 9).

**Với 1 GS chạy cục bộ:** hai nhánh `GetGlbValue(...) == 1` ở `league_cityinfo.lua:95,111,127` sẽ **không bao giờ đúng** nếu callback chạy đồng bộ (khoá đặt rồi mở ngay trong cùng call). Đó là hành vi mong muốn — bỏ hẳn đường "giá trị tạm".

---

# 5. StartGlbMSTimer / StopGlbMSTimer

**Chữ ký (suy từ toàn bộ 6 call site, nhất quán 100%):**

```
StartGlbMSTimer(nMissionID, nTimerID, nIntervalFrames)   -> 3 tham số
StopGlbMSTimer (nMissionID, nTimerID)                    -> 2 tham số
```

| Call site | Tham số | Giải mã |
|---|---|---|
| `citywar_global\mission.lua:6` | `StartGlbMSTimer(8, 18, INTERVAL)` | mission 8, timer-task 18, `INTERVAL = 5*60*18` frame (`mission.lua:1-2`) |
| `citywar_global\mission.lua:14` | `StopGlbMSTimer(8, 18)` | |
| `localnews\mission.lua:7` | `StartGlbMSTimer(MSID_LOCALNEWS=17, TMID_LOCALNEWS=32, 15*60*18)` | `mission.lua:2-4` |
| `statinfo\mission_goodssale.lua:4` | `StartGlbMSTimer(MSID_STAT_GOODS_SALE=32, TMID_STAT_GOODS_SALE=60, ...)` | `statinfo\head.lua:3-4` |
| `leaguematch\glbmission\mission.lua:6,15,16` | `(WLLS_MSID_GLB=26, WLLS_TIMERID_SCHEDULE=50 / _COMBAT=51, ...)` | `leaguematch\head.lua:102,107,108` |
| `mid_autumn\xingjiaoshang.lua:111`, `gmscript.lua:1068,1080` | `(MISSION_MOONCAKE_EXCHANGE=8, TIMER_MOONCAKE_EXCHANGE=47, delay*60*18)` | `mooncake_head.lua:14,17` |

**Xác nhận hai bảng tra:**

* `nMissionID` tra `server1\settings\task\missions.txt`: dòng `8 → \script\missions\citywar_global\mission.lua`; `17 → localnews`; `26 → leaguematch\glbmission`; `32 → statinfo`. Khớp tuyệt đối.
* `nTimerID` tra `server1\settings\timertask.txt`: file có header `TASK<TAB>SCRIPT` ở dòng 1, nên **id N nằm ở dòng N+1**. **Dòng 19 = id 18 = `\script\missions\citywar_global\timer.lua`** ✔ (đúng như bạn nêu). Tương tự id 32 = `\script\missions\clearskill\timer.lua`… (id 32 ở dòng 33 — script localnews dùng id 32, khớp bảng).

**Hành vi mong đợi:**
- Timer **LẶP LẠI vô hạn** với chu kỳ `nIntervalFrames` cho tới khi `StopGlbMSTimer` — **không ai giương lại**. Bằng chứng: `citywar_global\mission.lua` chỉ gọi Start **1 lần** trong `InitMission()` và `RunMission()` rỗng (`mission.lua:9-11`); `schedule.lua:173,176` (leaguematch) dùng cặp Stop-rồi-Start để **đổi tần số**, chứng tỏ Start là chu kỳ chứ không one-shot.
- Mỗi lần đáo hạn: chạy script `timertask.txt[nTimerID]` và gọi hàm **`OnTimer()`** trong đó (`citywar_global\timer.lua:8`).
- **Chu kỳ timer 18 (Công thành global) = 5 phút** (`mission.lua:2`: `5*60*18` frame, 18 frame = 1 giây).
- Mission 8 được mở lúc boot: **`autoexec.lua:142` `OpenGlbMission(8)`** → engine gọi `InitMission()` của `citywar_global\mission.lua`.
- `SetGlbMissionV(1,1)` (`mission.lua:5`) — biến mission toàn cục; `GetGlbMissionV` cũng có trong binary. Trong `timer.lua:15,28` lại dùng `GetMissionV(1)` **sau khi gán `SubWorld = WorldIdx`** — tức đọc mission của SUBWORLD, khác hẳn. Đừng nhầm hai cái.
- Nội dung `OnTimer` (`timer.lua:8-36`): quét 8 lôi đài (`IsArenaBegin(i-1)`, map 213..220 → `OpenMission(5)`) và 7 thành (`HaveBeginWar(i)` → map 221 → `OpenMission(6)` + `RunMission(6)`).

Ghi chú C++: `DIEUTRA_CONGTHANH_BINARY.md:116` đang ghi `StartGlbMSTimer(nMissionId, nHour?)` — **sai**, phải sửa thành 3 tham số `(nMissionID, nTimerID, nIntervalFrames)`.

---

# 6. Kho Khiêu chiến lệnh — league 538

**Cấu hình** (`missions\citywar_global\head.lua:24-26`):
```
TIAOZHANLING_LGTYPE        = 538
TIAOZHANLING_LGName        = "tiaozhanling"     -- MỘT league duy nhất
LGTSK_TIAOZHANLING_COUNT   = 1                  -- MEMBER task giữ số lệnh
```
Member = **tên bang** (`GetTongName()`).

**Tạo league / gia nhập** (`head.lua:37-56`):
- `checkCreatLG(szlgname)` — `LG_GetLeagueObj(538,"tiaozhanling")`; nếu 0/nil → `LG_CreateLeagueObj` → `LG_SetLeagueInfo(h,538,"tiaozhanling")` → `LG_ApplyAddLeague(h,"","")` → `LG_FreeLeagueObj(h)`. **Đúng, `head.lua:37-45` như bạn đoán.**
- `checkJoinLG(szlgname)` — `LG_GetLeagueObjByRole(538, szlgname)`; nếu 0/nil → `LGM_CreateMemberObj` → `LGM_SetMemberInfo(h, szlgname, 0, 538, "tiaozhanling")` → `LGM_ApplyAddMember(h,"","")` → `LGM_FreeMemberObj(h)`.
- Gọi từ: `infocenter_head.lua:205,208` (`GiveTiaoZhanLing`) và `:281,284` (`ViewTiaoZhanLing`); ngoài ra `mergeserver201010\tonggetaward.lua:23-24`, `mergeserver201105\tonggetaward.lua:43-44`, `vng_event\denbu_congthanh\congthanh.lua:37-38`.

**Đường GiveTiaoZhanLing (nộp lệnh) — `infocenter_head.lua:183-270`:**

| Bước | Dòng | Việc |
|---|---|---|
| 1 | `184` | `checkBangHuiLimit()` — phải có bang |
| 2 | `188-201` | Hạn ngày: task người chơi `TIAOZHANLING_TASK_DATE = 1839` (3 byte y/m/d qua `SetByte/GetByte`) + `TIAOZHANLING_TASK_COUNT = 1840`; **tối đa 300 lệnh/ngày/người** (`:192`) |
| 3 | `205,208` | `checkCreatLG` + `checkJoinLG` |
| 4 | `210-214` | `LG_GetMemberTask(538,"tiaozhanling",szTongName,1)`; **trần 2 000 000 000** |
| 5 | `216` | `GiveItemUI(..., "sure_GiveTiaoZhanLing", "OnCancel")` |
| 6 | `224-231` | Kiểm mọi item = **6/1/1499** (`nCityWar_Item_ID_G/D/P`, `infocenter_head.lua:25-27`) |
| 7 | `254-257` | `RemoveItemByIndex` từng cái |
| 8 | `258` | `SetTask(1840, nCountall+nCount)` |
| 9 | **`261`** | **`LG_ApplyAppendMemberTask(538,"tiaozhanling", szTongName, 1, nCount, "", "")` — 7 đối, CỘNG vào kho** |
| 10 | `266-268` | `AddOwnExp(nCount*50000)` |

**Xem kho:** `ViewTiaoZhanLing` (`:273-295`) — chỉ bang chủ/trưởng lão (`:275`), đọc `LG_GetMemberTask(538,...,1)` ở `:292`.

**Trừ kho:** chỉ ở **`infocenter_head.lua:701`** trong `sure_signupcitywar` — `LG_ApplyAppendMemberTask(538,"tiaozhanling",szTongName,1, -nCount, "", "")`, tức khi bang đem lệnh đi **đấu giá báo danh Công thành**. Kiểm trước ở `:675-679` (`nCount > nTongCurCount` → từ chối) và ở `:637-644` (`nCurCount <= 0` → từ chối; trần 1 000 000/lần).

**Hoàn:** kho 538 **KHÔNG được hoàn**. Cái được hoàn là điểm đã đặt cược ở league **508**: `TakeQingtongDing`/`sure_takeQingtongDing` (`:560-596`) trả **item** 6/1/1499 rồi `LG_ApplyAppendMemberTask(508, cityname, szTongName, 1, -nFree)` (`:589`, **5 đối**). Điều kiện: 19h–24h, không phải chủ thành/khiêu chiến, đã hết pha báo danh (`checkIsTakeQingtongDing`, `:516-554`).

⚠ Bug gốc cần giữ nguyên hay sửa (bạn quyết): `:589` trừ `nFree` chứ không phải `ncount` đã phát ra.

---

# 7. citywar_tbLadder — luồng dữ liệu 508 + ladder 10261

**Một file, hai biến thể**, khác nhau đúng 2 dòng cờ:
- GS: `server1\script\missions\citywar_global\ladder.lua:6-7` → `_GAMESERVER_=1, _RELAY_=nil`
- Relay: `gateway\s3relay\script\mission\citywar_global\ladder.lua:6-7` → `_GAMESERVER_=nil, _RELAY_=1`
- Chung: `nLadderId = 10261` (`:9`), `LEAGUETYPE_CITYWAR_SIGN=508` (`:12`), `LGTSK_QINGTONGDING_COUNT=1` (`:14`), `citywar_tbLadder:_init(10261)` chạy lúc nạp (`:85`).

**Chuỗi gọi đầy đủ (nộp lệnh → xếp hạng → bốc thăm):**

```
[GS] infocenter_head.lua:662  sure_signupcitywar(nCount)
  :666   kiểm 18h≤H<19h  và  getSignUpState(cityId)==1
           └─ :557  LG_GetLeagueTask(508, cityid_to_lgname(id), 1)   -- cờ pha, relay bật ở citywar_head.lua:103
  :675   LG_GetMemberTask(538,"tiaozhanling",tong,1)   -- đủ lệnh?
  :681   nlg  = LG_GetLeagueObj(508, cityName)
  :682   nlid = LG_GetLeagueObjByRole(508, tong)
  :683-690  nếu chưa là member 508:
             LGM_CreateMemberObj → LGM_SetMemberInfo(h,tong,0,508,cityName)
             → LG_AddMemberToObj(nlg,h)  → LGM_ApplyAddMember(h,"","") → LGM_FreeMemberObj
  :694   nCurCount = LG_GetMemberTask(508, cityName, tong, 1)          -- điểm CŨ (mirror)
  :698   citywar_tbLadder:AddOneInGameServer(nTongID, cityName, tong, nCount)
           └─ ladder.lua:30  szParam = format("%u,%s,%s,%d", ...)
           └─ ladder.lua:31→36  LG_ApplyDoScript(1,"","",
                                  "\\script\\mission\\citywar_global\\ladder.lua",
                                  "citywar_tbLadder__AddOne", szParam, "", "")
  :701   LG_ApplyAppendMemberTask(538,"tiaozhanling", tong, 1, -nCount, "", "")   -- trừ kho
  :705   checkFirstSignUpChallenger(cityName, tong, nCount+nCurCount)  -- cập nhật league 509

[RELAY] mission\citywar_global\ladder.lua:69  citywar_tbLadder__AddOne(szParam)
  :70-74  split → nTongId, szLGName(=tên thành), szName(=tên bang), nValue
  :75  → :58  AddOneInRelay(...)
        :61  LG_ApplyAppendMemberTask(508, szLGName, szTongName, 1, nValue, "", "")   -- ĐIỂM THẬT
        :63  nPoint = LG_GetMemberTask(508, szLGName, szTongName, 1)                  -- tổng mới
        :64  Ladder_NewLadder(10261, szTongName, nPoint, 1)                            -- bảng hiển thị
```

**Đọc ra:**

```
[GS  hiển thị] infocenter_head.lua:124  citywar_tbLadder:GetInfo()
                 └─ ladder.lua:44-45  vòng i=1..10  Ladder_GetLadderInfo(10261, i) → (szName, nValue)
               → citywar_CheckVotes() dựng bảng top-10 (infocenter_head.lua:122-139)

[GS  ai đứng nhất] infocenter_head.lua:440  getCityWarElector(szLeagueName)
                 :441 LG_GetLeagueObj(509, city) → :446 LG_GetMemberCount
                 :452-465 duyệt i=0..nMem-1: LG_GetMemberInfo(509lid, i)
                          + LG_GetMemberTask(508, city, szMem, 1)   ⚠ đọc task ở league 508
                 :467 random trong nhóm đồng điểm cao nhất

[RELAY 19h bốc thăm] relaysetting\task\citywar_head.lua:64  GetRandomChallenger(szCityName)
                 :65 LG_GetLeagueObj(508, szCityName)   :66 LG_GetMemberCount
                 :73 / :82  LG_GetMemberInfo(nlid, i)     (i = 0..nmem-1)
                 :83  LG_GetMemberTask(508, szCityName, szMem, 1)
                 :96  tbMem[random(getn(tbMem))][1]   -- ngẫu nhiên trong nhóm đồng điểm nhất
```

**Reset:** `citywar_tbLadder:Reset()` = `Ladder_ClearLadder(10261)` (`ladder.lua:82-84`), gọi từ **relay** `citywar_head.lua:101` trong `cw_startsignup_fun` lúc 18h00.

**Lịch relay (5 pha):** `citywar_head.lua:99-145` + `relaysetting\task\citywar_NN_*.lua`
- `citywar_clearchallenger.lua` — **0h00 mỗi ngày** (`TaskTime(0,0)`, `TaskInterval(1440)`): `clearCityWarLeague()` (`:42-68`) gỡ toàn bộ member của 509 rồi 508 bằng `LGM_ApplyRemoveMember(type, city, mem, "", "", 0)`; `checkCityWarLeague()` (`:20-40`) tạo lại league 508/509 cho 7 thành nếu thiếu.
- 18h00 `cw_startsignup_fun` (`:99-108`): `Reset()` ladder → `LG_ApplySetLeagueTask(508, city, 1, 1)` → `AddLocalNews`.
- 19h00 `cw_endsignup_fun` (`:115-145`): `GetRandomChallenger` → `AppointViceroy` (thành vô chủ) hoặc `AppointChallenger`, qua `GlobalExecute("dwf \\script\\missions\\citywar_global\\citywar_function.lua citywar_appoint*(...)")` → chạy `citywar_function.lua:269-280` trên GS → cuối cùng `LG_ApplySetLeagueTask(508, city, 1, 0)` (`:144`, **luôn chạy kể cả khi return sớm ở nhánh trên**).
- 20h00 `cw_start_fun` (`:109-113`): `StartCityWar(ncan)`.

**Điểm quan trọng cho C++:** điểm quyết định bốc thăm nằm ở **member-task league 508**, KHÔNG phải ladder 10261. Ladder chỉ để hiển thị. Nếu bạn chỉ port ladder mà bỏ 508 thì phần bốc thăm chết.

⚠ Lưu ý một bất nhất gốc: `checkFirstSignUpChallenger` (`infocenter_head.lua:751-796`) chỉ **THÊM** member vào 509, **không bao giờ xoá** member cũ (dòng xoá bị comment ở `:792-793`) → league 509 tích luỹ nhiều bang trong ngày; `getCityWarElector` mới là chỗ lọc theo điểm 508. Và **`citywar_function.lua:240` `checkCityWarLeague()` phía GS KHÔNG có call site nào** (grep toàn cây) — league 508/509 chỉ do relay tạo lúc 0h. **Khi port 1-GS bạn PHẢI tự gọi nó (hoặc bản relay) lúc boot + 0h**, nếu không mọi `LG_ApplySetLeagueTask(508,...)` sẽ rơi vào hư không.

---

# 8. Kiểm kê jx2compat.lua hiện tại — đúng/sai

| Hàm | jx2compat | Thực tế | Phán quyết |
|---|---|---|---|
| `LG_CreateLeagueObj` | `:122-127` → số | số | ✅ |
| `LG_FreeLeagueObj` | `:129-132` | | ✅ |
| `LG_SetLeagueInfo` | `:134-142` (3 đối) | 3 đối | ✅ |
| `LG_GetLeagueInfo` | `:144-150` trả **`(nMod, szName)`** | trả **`(szName, nCreateTime, nMemberCount)`** | ❌ **sai thứ tự + thiếu 1 giá trị** |
| `LGM_CreateMemberObj` | `:152-157` | | ✅ |
| `LGM_SetMemberInfo` | `:159-166` (5 đối) — bỏ nJob/nMod/szLeagueName | 5 đối, **nMod+szLeagueName là ĐỊA CHỈ ĐÍCH bắt buộc** | ❌ mất thông tin → `LGM_ApplyAddMember` không biết vào league nào |
| `LG_AddMemberToObj` | `:168-176` | | ⚠ chỉ hoạt động với obj tạm; chưa chịu được lid đăng ký (`infocenter_head.lua:687`) |
| `LG_ApplyAddLeague` | `:179-195` (3 đối) | 1 **hoặc** 3 đối | ⚠ thiếu dạng 1 đối (`gb_modulefuncs.lua:67`); **không gọi callback** dù nhận `szScript/szFunc` |
| `LG_ApplyRemoveLeague` | `:197-200` | 2 đối | ✅ |
| `LG_GetLeagueObj` | `:202-208` → **CHUỖI** `"mod|name"` | **SỐ** lid, 0 nếu không có | ❌ sai kiểu |
| `LG_GetLeagueTask` | `:210-219` chỉ dạng **(lid, nTaskID)** | 2 dạng: (lid,task) **và** (type,name,task) | ❌ thiếu dạng 3 đối → `league_cityinfo.lua:101,117,133`, `infocenter_head.lua:557` (cờ pha báo danh) trả sai |
| `LG_ApplySetLeagueTask` | `:221-224` (4 đối) | 4 **hoặc** 6 | ⚠ dạng 6 vẫn "chạy" (thừa đối bị bỏ) nhưng **không gọi callback** → `OnSetCityLeagueTask` không bao giờ chạy → `GlbValue[840..846]` **kẹt ở 1 vĩnh viễn** → `get_citybonus_date/get_citybonus_task` luôn đi nhánh giả (`league_cityinfo.lua:95,111,127`) ❌ |
| `LG_ApplyAppendLeagueTask` | `:226-234` (4 đối) | 4 hoặc 6 | ⚠ như trên |
| `LG_GetFirstLeague` / `LG_GetNextLeague` | `:236-260` trả khoá chuỗi; `GetNext` là O(n) quét lại | trả lid số | ❌ sai kiểu (gãy `if (lid > 0)`) |
| `LGM_ApplyAddMember` | `:262-269` — **`(lid, szMember)`** | **`(hMemberObj [,cb,cbf])`** | ❌ **sai hoàn toàn**: nhận nhầm member-obj làm lid → luôn trả 0 |
| `LG_GetMemberTask` | `:271-277` — **`(lid, szMember, nTaskID)`** 3 đối | **`(nType, szLgName, szMember, nTaskID)`** 4 đối | ❌ **sai arity** → mọi chỗ đọc số Khiêu chiến lệnh sai |
| `LG_ApplySetMemberTask` | `:279-282` — 4 đối | **5 đối** | ❌ sai arity |
| `LG_ApplyAppendMemberTask` | `:284-292` — 4 đối | **5 hoặc 7 đối** | ❌ sai arity → `infocenter_head.lua:261,589,701` + relay `ladder.lua:61` đều hỏng |
| `LG_GetMemberCount` | **KHÔNG CÓ** | | ❌ thiếu |
| `LG_GetMemberInfo` | **KHÔNG CÓ** | | ❌ thiếu |
| `LG_GetLeagueObjByRole` | **KHÔNG CÓ** | | ❌ thiếu (11+ call site) |
| `LGM_FreeMemberObj` | **KHÔNG CÓ** | | ❌ thiếu |
| `LGM_ApplyRemoveMember` | **KHÔNG CÓ** | | ❌ thiếu |
| `LG_ApplyDoScript` | **KHÔNG CÓ** | | ❌ thiếu (25+ call site, gồm toàn bộ đường ladder) |
| `LG_GetMemberObj`, `LG_GetLeagueCreateTime`, `LG_GetMemberJoinTime` | **KHÔNG CÓ** | 0 call site | ⚪ stub cho đủ tên |

Thêm: `jx2compat.lua:71-88` `gb_GetTask(szModule, szName)` / `gb_SetTask(szModule, szName, nValue)` — tên tham số sai nhưng **arity khớp** với bản gốc `gb_GetTask(szModuleName, nTaskID)` (`gb_taskfuncs.lua:14,22`), nên vẫn chạy được. Chỉ cần lưu ý: khi có League C++ thật, `gb_taskfuncs.lua` gốc sẽ tự chạy qua League (500) và **phải gỡ bản mô phỏng này** để tránh đệ quy như comment `jx2compat.lua:116-118` cảnh báo.

---

# 9. Danh mục hàm cần đăng ký C++ (chốt)

**League (16):** `LG_CreateLeagueObj`, `LG_FreeLeagueObj`, `LG_SetLeagueInfo`, `LG_AddMemberToObj`, `LG_ApplyAddLeague`, `LG_ApplyRemoveLeague`, `LG_GetLeagueObj`, `LG_GetLeagueObjByRole`, `LG_GetFirstLeague`, `LG_GetNextLeague`, `LG_GetLeagueInfo`, `LG_GetLeagueCreateTime`, `LG_GetMemberCount`, `LG_GetMemberInfo`, `LG_GetMemberObj`, `LG_GetMemberJoinTime`

**Task (5):** `LG_GetLeagueTask`(quá tải 2/3), `LG_ApplySetLeagueTask`(4/6), `LG_ApplyAppendLeagueTask`(4/6), `LG_GetMemberTask`(4), `LG_ApplySetMemberTask`(5), `LG_ApplyAppendMemberTask`(5/7)

**Member (5):** `LGM_CreateMemberObj`, `LGM_SetMemberInfo`, `LGM_ApplyAddMember`, `LGM_ApplyRemoveMember`, `LGM_FreeMemberObj`

**Khác (5):** `LG_ApplyDoScript`(8), `SetGlbValue`(2), `GetGlbValue`(1), `StartGlbMSTimer`(3), `StopGlbMSTimer`(2)

Tất cả 28 tên `LG_*`/`LGM_*` trên đều **có thật trong cả `jx_linux_y` lẫn `s3relay_y`** (grep chuỗi, danh sách khớp 100% giữa 2 binary); `SetGlbValue/GetGlbValue/StartGlbMSTimer/StopGlbMSTimer/SetGlbMissionV/GetGlbMissionV/OpenGlbMission/CloseGlbMission/RunGlbMission/Msg2MSAll` **chỉ có trong `jx_linux_y`** (GS-side); `GlobalExecute/StartCityWar` **chỉ có trong `s3relay_y`**.

---

Bổ sung (job nền vừa xong) — **đối chiếu hiện trạng C++ trong cây của bạn**:

## 10. Đã có gì / còn thiếu gì phía engine JX1

| Nhóm | Hiện trạng | Vị trí |
|---|---|---|
| `OB_*` (14 hàm ObjBuffer) | ✅ **đã đăng ký** | `Sources\Core\Src\ScriptFuns.cpp:12861-12874` (extern `:12140-12153`) |
| `Ladder_NewLadder` / `Ladder_ClearLadder` / `Ladder_GetLadderInfo` | ✅ **đã đăng ký** | `ScriptFuns.cpp:12875-12877` |
| `SetGlbValue` / `GetGlbValue` | ✅ **đã đăng ký & đã hiện thực** | `ScriptFuns.cpp:12878-12879`; impl `KJx2SharedStore.cpp:461-484` |
| `SetGlbMissionV` / `GetGlbMissionV` / `GetGlbMissionVC` / `Msg2MSAll` | ✅ **đã có sẵn (JX1 gốc)** | `ScriptFuns.cpp:12580-12582, 12611` — chạy trên `g_GlobalMissionArray` |
| **`StartGlbMSTimer` / `StopGlbMSTimer`** | ❌ **CHƯA CÓ** (grep `ScriptFuns.cpp` + `KJx2SharedStore.cpp` = 0 hit) | cần viết mới |
| **`OpenGlbMission`** | ❌ **CHƯA CÓ** — mà `global\autoexec.lua:142` gọi `OpenGlbMission(8)`; thiếu nó thì `citywar_global\mission.lua:InitMission()` không bao giờ chạy ⇒ timer 18 không bao giờ được giương | cần viết mới |
| **Toàn bộ 28 hàm `LG_*` / `LGM_*`** | ❌ **CHƯA CÓ** | cần viết mới (nội dung mục 1–3 ở trên) |

**Chi tiết `GlbValue` đã hiện thực** (`KJx2SharedStore.cpp:459-484`):
- Kho là `static std::map<int,int> s_GlbValueMap;` → **không có trần id**, nên dải 840–846 và id lớn nhất quan sát được 1283 đều an toàn, không cần sửa.
- `SetGlbValue(id, v)` trả **1** khi cả 2 đối là số, **0** khi sai kiểu — bản gốc chỉ trả 1; sai lệch này vô hại vì **không call site nào trong 323 dòng dùng giá trị trả về**.
- `GetGlbValue(id)` trả **0** khi chưa đặt hoặc đối không phải số — khớp bản gốc.
- ⚠ RAM thuần, **không persist qua restart**. Đúng ngữ nghĩa gốc (per-GS, RAM), và `buildAllCityInfoLeague()` (`autoexec.lua:147` → `league_cityinfo.lua:66-69`) đặt lại 840–846 = 0 mỗi lần boot, nên không cần persist.

**Hệ quả cho thứ tự thi công:** phần mục 4 (GlbValue) và mục 7 (Ladder 10261) **đã xong**; việc còn lại đúng bằng mục 1–3 (League/Member store + `LG_ApplyDoScript`) cộng mục 5 (`OpenGlbMission` + `StartGlbMSTimer`/`StopGlbMSTimer`). Nếu làm `LG_*` trước mà chưa làm `OpenGlbMission`/`StartGlbMSTimer`, đường Công thành vẫn đứng im vì không có nhịp timer 5 phút.

Cũng lưu ý sửa lại `DIEUTRA_CONGTHANH_BINARY.md:116` — đang ghi `StartGlbMSTimer(nMissionId, nHour?)`, thực tế là **`StartGlbMSTimer(nMissionID, nTimerID, nIntervalFrames)`** (3 tham số) và **`StopGlbMSTimer(nMissionID, nTimerID)`** (2 tham số), như đã chứng minh ở mục 5.