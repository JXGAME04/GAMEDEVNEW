# BÀN GIAO — PORT HẠ TẦNG S3RELAY KIỂU LINUX (đợt 1a)

> 06/09/2026 · nhánh `relayhatang-0609` (worktree `D:\GAMEDEVNEW_wt_relay`)
> Tiếp nối `BANGIAO_MO_S3RELAY_LINUX_0609.md` (kết quả mổ nhị phân `s3relay_y`).
>
> **Mục tiêu chủ đặt ra**: "port như Linux để sau này còn lấy nhiều tính năng qua
> sẽ làm như bản Linux" — tức dựng **hạ tầng**, để mỗi tính năng sau này chỉ là
> một kịch bản Lua đặt trên relay, đúng cách bản Linux làm.

---

## 1. VÌ SAO LÀM HẠ TẦNG TRƯỚC

Đo được từ nhị phân: relay Linux có **~339 hàm Lua**, S3Relay của JX1 chỉ có **4**
(`GlobalExecute`, `GameExecute`, `Msg2IP`, `GetIP`) và **không hề chạy kịch bản** —
chỉ thi hành chuỗi lệnh GM gửi tới. Mọi hoạt động toàn máy chủ bên Linux
(công thành, Tống Kim, liên đấu, vận tiêu, xổ số…) đều là kịch bản Lua trên relay,
tựa trên đúng 3 thứ:

| Hạ tầng | Việc nó làm |
|---|---|
| **Bộ kịch bản** (KLuaScriptSet) | mỗi tệp `.lua` một state, `Include`, `DynamicExecute` |
| **ShareData** | kho dữ liệu dùng chung cả cụm, lưu MySQL |
| **TaskCentre** | chạy kịch bản theo lịch (`tasklist.ini`) |
| (+ **ObjBuffer** `OB_*`) | đóng gói bảng Lua thành byte để lưu / truyền |

Port 3 thứ này một lần **rẻ hơn** port từng hoạt động một.

---

## 2. ĐÃ LÀM ĐƯỢC GÌ (đợt 1a — ĐÃ BIÊN DỊCH SẠCH)

### 2.1 Tệp mới
| Tệp | Dòng | Nội dung |
|---|---:|---|
| `Sources\MultiServer\S3Relay\RelayScript.h/.cpp` | ~700 | bộ kịch bản + **ObjBuffer 14 hàm `OB_*`** + 20 hàm Lua chung |
| `Sources\MultiServer\S3Relay\RelayShareData.h/.cpp` | ~520 | ShareData trên MySQL + **12 hàm Lua SDB** |
| `Sources\MultiServer\S3Relay\RelayTaskCentre.h/.cpp` | ~360 | TaskCentre + **9 hàm Lua Task*** |
| `ReverseTools\relay_0609\p01_noi_hatang.py` | 150 | bộ vá nối vào tệp có sẵn (idempotent, latin-1) |
| `serverscript_relay\script\lib\objbuffer_head.lua` | 120 | port từ Linux, đã chuyển Lua 4 → 5.4 |
| `serverscript_relay\script\lib\common_sharedata.lua` | 186 | port từ Linux, đã chuyển Lua 4 → 5.4 |
| `serverscript_relay\relaysetting\task\hb_relay.lua` | 65 | tác vụ mẫu + **phép thử hạ tầng** |
| `serverscript_relay\relaysetting\task\tasklist.ini` | — | danh sách tác vụ |

### 2.2 Tệp có sẵn bị đụng (qua `p01_noi_hatang.py`)
- `DoScript.h` — mở `GameScriptFuns` ra ngoài để kịch bản relay dùng lại
  `GlobalExecute` / `GameExecute` / `Msg2IP` / `GetIP`.
- `Global.cpp` — gọi `RelayScript_Init()` → `AddFuncTable(ShareData)` →
  `AddFuncTable(TaskCentre)` → `ShareData_Init()` → `TaskCentre_Init()` ngay sau
  `InitScript()`; và 3 hàm `*_Uninit()` lúc tắt.
  **Thứ tự bắt buộc**: đăng ký hết bảng hàm rồi mới nạp kịch bản.
- `S3Relay.h` / `S3Relay.cpp` — thêm `timer_relaytask = 7`, `SetTimer(…, 1000)`,
  `case timer_relaytask: TaskCentre_Tick();`.
- `S3Relay.vcxproj` — thêm 3 cặp tệp.

### 2.3 Bảng hàm Lua relay hiện có (43 hàm mới + 4 cũ)

```
-- kịch bản
OutputMsg  Include  IncludeLib  LoadScript  DynamicExecute
-- thời gian
GetCurrentDate  GetCurrentTime  GetSysCurrentTime  Tm2Time  Time2Tm
FormatTime2Number  FormatTime2String
-- tiện ích
Random  WriteStringToFile
-- biến toàn cụm (trong bộ nhớ)
GetGblInt  SetGblInt  DelGblInt  GetGblStr  SetGblStr  DelGblStr
-- ObjBuffer
OB_Create  OB_Release  OB_Clear  OB_IsEmpty  OB_Copy  OB_Append
OB_PushByte  OB_PopByte  OB_PushInt  OB_PopInt
OB_PushDouble  OB_PopDouble  OB_PushString  OB_PopString
-- ShareData (bền vững, MySQL)
OB_SaveShareData  OB_LoadShareData  OB_DeleteShareData
SaveStringToSDB  SaveStringToSDBOw  GetStringFromSDB
SaveIntegerToSDB  SaveIntegerToSDBOw  GetIntegerFromSDB
ClearRecordOnShareDB  GetRecordCount  GetFirstRecordFromSDB
-- TaskCentre
TaskName  TaskInterval  TaskTime  TaskCountLimit  TaskSetStartDay  TaskSetMode
GetTaskCurCount  TaskLoad  TaskRelease
-- có sẵn từ trước
GlobalExecute  GameExecute  Msg2IP  GetIP
```

**Tên hàm và ý nghĩa tham số giữ NGUYÊN như bản Linux**, để kịch bản bên Linux chép
sang chỉ phải chuyển cú pháp Lua 4 → 5.4 (dùng đúng bộ chuyển của đợt Lua 5.4).

### 2.4 Khuôn một tác vụ (y hệt bản Linux)
```lua
function TaskShedule()          -- ĐÚNG: bản gốc viết thiếu chữ 'c'
    TaskName("Chien dich Tuong Duong 13:00")
    TaskInterval(1440)          -- số PHÚT giữa hai lần chạy
    TaskTime(12, 50)            -- giờ:phút lần chạy đầu trong ngày
    TaskCountLimit(0)           -- 0 = không giới hạn
end
function TaskContent() ... end  -- thân tác vụ
function GameSvrConnected(dwIP) end
function GameSvrReady(dwIP) end
```

### 2.5 Kiểm đã chạy
- **Biên dịch + liên kết SẠCH**: `S3Relay.vcxproj Release|Win32` → `S3Relay.exe`
  4.462.592 byte (bản đang chạy 4.413.952). Lỗi `MSB3073` sau khi liên kết là
  bẫy cũ đã biết (post-build copy sang `D:\bin\...`), exe nằm ở `Release\S3Relay.exe`.
- **Nhập DLL**: `Engine.dll`, **`Lua54Dll.dll`**, `libmysql.dll`, `heaven/rainbow/FilterText`.
- **Cú pháp Lua 5.4 thật** (`ReverseTools\lua54\kiem_54.py`): 3 tệp, **0 lỗi**.
- **Mã nguồn thuần ASCII** (0 byte cao) → không có rủi ro hỏng TCVN3.
- Đã kiểm chuỗi trong exe: `OB_Create`, `OB_SaveShareData`, `TaskShedule`,
  `TaskContent`, `relay_sharedata`, `tasklist.ini`… đều có.

---

## 3. ✅ ĐÃ SWAP THẬT LÚC 12:23 — VÀ CÁCH GỠ CHỖ VƯỚNG

> **Cập nhật 06/09 12:23**: chủ tắt máy chủ và cho phép chép vào. Đã gỡ được chỗ vướng
> mô tả bên dưới, `bin\multiserver` giờ có đủ bộ:
>
> | Tệp | Cỡ | Ghi chú |
> |---|---:|---|
> | `S3Relay.exe` | 4.462.592 | bản mới (cũ 4.413.952 → `S3Relay.exe.truoc_relayht0609`) |
> | `engine.dll` | 923.648 | **"Engine Server Release\|Win32" bản Lua 5.4** (cũ 634.368 → `engine.dll.truoc_relayht0609`) |
> | `Lua54Dll.dll` | 492.544 | Win32 Release, tệp mới |
> | `libcrypto-3.dll` | 2.970.624 | **tệp mới bắt buộc** — engine.dll mới nhập OpenSSL (do `Src\KIniFile.cpp`); thiếu là engine.dll không nạp được và cả S3Relay lẫn Bishop chết lúc khởi động. Lấy từ `C:\vcpkg\packages\openssl_x86-windows\bin` |
>
> **Lùi lại**: chạy `LuiRelayHT0609.bat` trong chính thư mục đó (relay phải tắt).
>
> **Cách gỡ lỗi liên kết**: thêm `<IgnoreSpecificDefaultLibraries>MSVCRT.lib` vào
> cấu hình *"Engine Server Release|Win32"* (commit `6f251915`). Còn lại chỉ là cảnh
> báo `LNK4217/LNK4286` "imported by" của `JpgLib.lib`/`mp3lib.lib` — vô hại vì cả
> hai chỉ dùng `malloc`/`memmove` và đều nằm trong **cùng một CRT tĩnh** bên trong
> `engine.dll`.
>
> **Đã đối chiếu trước khi swap** (bằng bộ đọc PE tự viết): mọi hàm `engine.dll` mà
> `Bishop.exe` (1), `StartGameSV.exe` (1), `S3DBInterface.dll` (3) và `S3Relay.exe`
> (23) gọi **đều còn đủ** trong bản mới (1.320/1.324 hàm xuất); mọi DLL phụ thuộc
> đều có mặt. Cặp cũ vốn **đã trộn CRT sẵn** (S3Relay tĩnh + engine.dll CRT gỡ rối)
> nên bản mới (tĩnh + tĩnh) không phát sinh rủi ro mới.
>
> **CHƯA CHẠY THỬ**: `S3Relay.exe` có hộp thoại đăng nhập lúc khởi động nên không
> tự kiểm được — xem mục 5 để biết cần nhìn gì trong nhật ký khi chủ bật lên.

### 3.1 (Ghi lại) Chỗ vướng ban đầu

`bin\multiserver` là **đảo Lua riêng** và **vẫn đang chạy Lua 4**
(`lualibdll.dll`, `engine.dll` 634 KB ngày 18/08). S3Relay trên `origin/main` từ
commit `38d65e50` đã gắn Lua 5.4 ở tầng **header** (`Engine\Include\LuaLib.h` ánh xạ
`Lua_*` → `lua4_*` của `Lua54Dll.dll`) nên **relay build ra bắt buộc là Lua 5.4**.

⇒ Muốn swap phải đưa **cùng lúc 3 tệp**:
1. `S3Relay.exe` (đã có — xem mục 4)
2. `Lua54Dll.dll` **Win32** (đã build được — xem mục 4)
3. `engine.dll` **"Engine Server Release|Win32"** — **CHƯA CÓ**

**Kẹt ở (3)**: build `Engine.vcxproj / Engine Server Release|Win32` trong worktree
này **liên kết hỏng**:
```
MSVCRT.lib(chandler4gs.obj) : error LNK2019: unresolved external symbol
                              __except_handler4_common
.\EngineServerRelease\Engine.dll : fatal error LNK1120: 1 unresolved externals
```
Cấu hình đó để `RuntimeLibrary = MultiThreaded` (/MT) nhưng có tệp/thư viện kéo
`MSVCRT.lib` (CRT động) vào — đúng loại bẫy "trộn CRT" đã cắn ở client 18/08.
`Lua54Dll` Release đã là /MT nên **không phải thủ phạm**; nghi `JpgLib.lib` /
`mp3lib.lib` dựng sẵn trong `Lib\release`.

**Việc này thuộc nhánh Lua 5.4 (wauto-c9)** — họ đã hẹn "đợt 2 đưa engine.dll
Win32 server + Lua54Dll Win32 cùng lúc với S3Relay". Tôi **không sửa Engine** để
khỏi giẫm chân. Cần: hoặc họ dựng `engine.dll` Win32 server bản Lua 5.4, hoặc
cho phép tôi thêm `vcruntime.lib` / gỡ `MSVCRT` khỏi cấu hình đó.

---

## 4. HIỆN VẬT ĐÃ ĐẶT SẴN

| Chỗ | Nội dung |
|---|---|
| `bin\multiserver\S3Relay.exe` · `engine.dll` · `Lua54Dll.dll` · `libcrypto-3.dll` | **đã swap 12:23** (xem mục 3) |
| `bin\multiserver\*.truoc_relayht0609` | 2 tệp sao lưu để lùi |
| `bin\multiserver\LuiRelayHT0609.bat` | lùi về bản cũ (relay phải tắt) |
| `bin\multiserver\script\lib\*.lua` | 2 thư viện kịch bản |
| `bin\multiserver\relaysetting\task\*` | `tasklist.ini` + `hb_relay.lua` |

Thư mục dựng tạm `_relayht_0609\` đã xoá sau khi swap xong.

---

## 5. CÁCH THỬ SAU KHI SWAP

1. Mở relay, xem cửa sổ nhật ký, phải thấy:
   ```
   [RelayScript] san sang, NN ham Lua
   [ShareData] MO: <host>:<port>/<db> bang relay_sharedata, nap 0 ban ghi
   [TaskCentre] + hb_relay.lua [Nhip tim relay ...] moc ngay lap tuc, moi 10 phut, toi da 0 lan
   [TaskCentre] nap 1/1 tac vu tu .\relaysetting\task\tasklist.ini
   ```
2. Ngay sau đó (tác vụ không đặt `TaskTime` nên chạy liền):
   ```
   [Script] [hb_relay] lan 1 trong phien nay, tong tu truoc toi nay 1, luc 2026-09-06 ...
   ```
3. Kiểm MySQL: `SELECT * FROM relay_sharedata;` phải có 2 dòng khoá
   `RELAY_HEARTBEAT` (một dòng thông tin `p1=0,p2=0`, một dòng bản ghi `p1=1,p2=1`).
4. Khởi động lại relay: con số "tổng từ trước tới nay" **phải tăng tiếp**, không về 1
   → chứng minh ShareData bền vững.
5. Muốn thử đường relay → GameServer: sửa `hb_relay.lua` đặt `g_bThongBao = 1`,
   khởi động lại relay, trong game phải thấy dòng thông báo.

---

## 6. ĐỢT 1b — ĐÃ LÀM XONG PHẦN CỦA S3RELAY

**`RemoteExecute` thật (RPC GameServer ↔ relay)** — `RelayRpc.h/.cpp` (577 dòng).

| Việc | Cách làm |
|---|---|
| Giao thức | `s2s_script = 98` (`KProtocolDef.h`) + `struct RELAY_SCRIPT_CALL` (`KRelayProtocol.h`): gói nằm trong `RELAY_DATA`, byte đầu thân gói là `s2s_script`, rồi cấu trúc, rồi 3 khối `szScript` / `szFunc` / `data` (ObjBuffer) |
| GS → relay | bắt ở `CHostConnect::Proc1_Relay_Data` nhánh **"arrived"** (`nToIP == 0`) — chỗ trước đây vứt gói. Gói tới trên **luồng mạng** nên chỉ **xếp hàng**; `RelayRpc_Tick()` (WM_TIMER **100 ms**, luồng chính) mới chạy Lua — Lua không an toàn đa luồng |
| relay → GS | gửi thẳng trên đúng kết nối của máy chủ đó (`FindNetConnect`), hoặc `BroadPackage` khi `dwGameSvrId = 0` |
| Kết quả + callback | `dwCallId` ≠ 0 thì bên nhận gửi ngược `byIsResult = 1`; bên gọi chạy `szCallBackFunc(nCallBackParam, hResult)` trong state của **chính kịch bản đã gọi** |

Thêm **10 hàm Lua**: `RemoteExecute`, `GetHostPlayerCount`, `IsGameServerReady`,
`ConnectIdx2GameServerId`, `GameServerId2ConnectIdx`, `Msg2PlayerByName`,
`Msg2Tong`, `Msg2Faction`, `AddGlobalNews`, `NotifySDBRecordChanged`,
`NotifySDBRChanged1Svr`. Nhóm `Msg2*`/`Notify*` đi qua đường GM `dw <lệnh Lua>`
có sẵn nên **không phải sửa `GameServer.exe`**.

Và **4 hàm ShareData bản ghi nhiều ô** giống bản Linux: `SaveCustomDataToSDB`,
`SaveCustomDataToSDBOw`, `GetCustomDataFromSDB`, `GetRecordInfoFromNO`
(`'i'` số nguyên · `'l'` số dài · `'s'` chuỗi — ví dụ `'sliii'`).

### 6.1 Còn lại: nửa bên GameServer (chưa làm, cần swap `CoreServer.dll`)

`LuaJX2_RemoteExecute` trong `Core\Src\KJx2SharedStore.cpp` (port 23/08) vẫn
**chạy TẠI CHỖ trong chính GameServer** — ghi chú trong mã: *"Du an 1 GS khong relay
-> thuc thi TAI CHO dong bo"*. Relay giờ đã sẵn sàng nhận, chỉ cần bên GS chịu gửi:

1. `LuaJX2_RemoteExecute`: **giữ nguyên** đường chạy tại chỗ khi kịch bản có sẵn
   trong cây của GameServer (khỏi hỏng 14 chỗ gọi đang chạy: `weeklyrank`,
   `tongcastle`, `bigboss`, `msg2allworld`…); **chỉ khi không tìm thấy kịch bản**
   thì đóng gói gửi lên relay qua `g_NewProtocolProcess.PushMsgInTransfer` —
   hiện chỗ đó chỉ ghi log "script chua nap, bo qua" nên đổi là **thuần lợi**.
2. Nhận kết quả: relay trả về bằng `dw` (đường `s2s_execute` có sẵn) gọi một hàm
   Lua điều phối do Core đăng ký ⇒ **vẫn không phải sửa `GameServer.exe`**.
3. Chép `script\lib\remoteexc.lua` + `sharedata.lua` (bản Linux) sang cây relay.

Việc này đụng `CoreServer.dll` nên để thành một đợt riêng, swap cùng lúc với
GameServer khi chủ sắp xếp được.

---

## 7. BẪY ĐÃ GẶP / GHI CHO ĐỢT SAU

1. **Đăng ký hàm TRƯỚC khi chạy thân chunk.** Nếu `RegisterFunctions` gọi sau
   `LoadBuffer/ExecuteCode` thì `Include` = nil và thân chunk chết im lặng —
   đúng lỗi 3.820 dòng ScriptError của client sáng 06/09.
2. **`std::vector` đổi chỗ khi `push_back`.** `TaskShedule()` có thể gọi `TaskLoad()`
   → thêm phần tử → con trỏ đang giữ vào vector hỏng. Đã xử: `s_pLoading` trỏ vào
   **biến cục bộ**, và `reserve(TC_MAX_TASK)` ngay lúc khởi động.
3. **Vòng `Include` lẫn nhau**: đặt con trỏ script vào bảng **trước** khi chạy chunk,
   và mỗi state giữ một tập "đã include".
4. **Cú pháp Lua**: cây script đang chạy dùng `format(...)` / `date(...)` kiểu Lua 4
   (shim `lua4compat` cấp), **không** dùng `string.format`. Viết kịch bản relay theo
   đúng lối đó cho đồng bộ.
5. **Tiếng Việt trong kịch bản**: tệp bàn giao đợt này cố ý viết ASCII không dấu.
   Câu nào hiện lên màn hình người chơi phải ghi bằng `vn_to_octal` (TCVN3), đừng gõ
   thẳng.
6. **Build relay từ worktree** phải chép tay `Lib\debug\libdb181sd.lib` +
   `Lib\release\libdb181s.lib` (không tracked) — thiếu là LNK1181.

---

## 8. ĐÃ CHẠY THỬ THẬT — 06/09 12:49→12:51 (máy chủ đang tắt)

Cách thử: bật `S3RelayServer.exe` (chính là **root**, nghe cổng 7777) rồi bật
`S3Relay.exe`. Hộp thoại đăng nhập **tự đóng** (đọc tài khoản từ `relay_config.ini`
`[root]`) nên không cần bấm gì.

### 8.1 Nhật ký lần chạy đầu (`relay_log\2026_09_06\12_49_48_798.log`)
```
5  -- [RelayScript] san sang, 38 ham Lua
6  -- [RelayRpc] san sang (hang doi 256, cho ket qua 512)
7  -- [ShareData] MO: 127.0.0.1:3306/jx1_role bang relay_sharedata, nap 0 ban ghi
8  -- [Script] [hb_relay] da khai bao lich: 10 phut/lan
9  -- [TaskCentre] + hb_relay.lua [Nhip tim relay ...] moc ngay lap tuc, moi 10 phut
10 -- [TaskCentre] nap 1/1 tac vu tu .\relaysetting\task\tasklist.ini
...
357 -- [Script] [hb_relay] lan 1 trong phien nay, tong tu truoc toi nay 1, luc 2026-09-06 12:49:53
```
Dòng 357 chứng minh **cả chuỗi** trong một phát: TaskCentre gọi `TaskContent()` →
`Include` hai tệp thư viện → `NewCommonShareData` → ObjBuffer đóng gói bảng Lua →
`OB_SaveShareData` → ghi MySQL → `format()` / `FormatTime2String()` / `GetTaskCurCount()`.

### 8.2 MySQL sau đó
```
so ban ghi: 2
  skey=RELAY_HEARTBEAT p1=0 p2=0 len=45   <- ban ghi thong tin {nUsed, nMaxRow}
  skey=RELAY_HEARTBEAT p1=1 p2=1 len=63   <- ban ghi 1 {data, key}
```

### 8.3 Khởi động lại → dữ liệu bền vững
```
7   -- [ShareData] MO: ... nap 2 ban ghi          <- doc lai tu MySQL
357 -- [Script] [hb_relay] lan 1 trong phien nay, tong tu truoc toi nay 2
```
Bộ đếm **2** chứ không quay về 1 ⇒ ShareData sống qua khởi động lại. Đúng như bản Linux.

### 8.4 Một bẫy đã đo được (KHÔNG phải lỗi của đợt này)
Lúc đầu tôi thử bằng một **"root giả"** (chỉ mở cổng 7777 rồi im lặng). Relay
**sập `0xC0000005`** sau ~5–10 giây, tại `CNetClient::Startup+0x200`
(`NetClient.cpp:245` — nhánh dọn dẹp khi `ConnectTo` thất bại **sau khi** TCP đã nối).
Đã đối chứng: **bản CŨ** (`S3Relay.exe` 4.413.952 + `engine.dll` 634.368) **sập y hệt
ở cùng chỗ** ⇒ **lỗi có sẵn**, không do đợt này. Bài học: đừng thử relay bằng root giả,
hãy bật `S3RelayServer.exe` thật. (Dump nằm ở `bin\multiserver\DumpInfo\`; đọc bằng
`ReverseTools\crash\doc_dump.py` + `giai_ma.py` như đợt 04/09.)

### 8.5 Trạng thái hiện tại
- Tiến trình thử **đã tắt hết**, chủ bật cụm như bình thường.
- `relay_sharedata` còn 2 dòng `RELAY_HEARTBEAT` của tác vụ mẫu — vô hại, cứ để làm
  "nhịp tim" (10 phút một dòng log). Muốn tắt: sửa `relaysetting\task\tasklist.ini`
  thành `Count=0`.
