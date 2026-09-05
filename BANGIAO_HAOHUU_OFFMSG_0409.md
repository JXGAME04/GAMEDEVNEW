# BÀN GIAO 04/09/2026 — HẢO HỮU / CỪU NHÂN / THÂN NHÂN LƯU DB + LỜI NHẮN MẬT KHI NGOẠI TUYẾN

Chủ yêu cầu: *"hệ thống hảo hữu - cừu nhân - thân nhân của dự án: hảo hữu xong không lưu"* và
*"khi thoát game người khác nhắn tin mật, khi online lại sẽ thông báo tin nhắn — tất cả có sẵn ở bản Linux"*.

Nhánh `haohuu-0409` (worktree `D:\GAMEDEVNEW_wt_haohuu`). **Chỉ đụng S3Relay** — không cần build lại
CoreServer/CoreClient/GameServer. Bản build: `Sources\MultiServer\S3Relay\Release\S3Relay.exe`
→ đã đặt `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver\S3Relay.exe.moi` + `ChayRelay.bat`.

---

## 1. Hệ hảo hữu của dự án nằm ở đâu

| Tầng | Tệp | Vai trò |
|---|---|---|
| Client | `S3Client\Ui\UiCase\UiChatCentre.cpp:69-75` | 3 đơn vị: `"Hảo Hữu"` / `"Thân Nhân"` / `"Cừu Nhân"` (TCVN3). Gửi `friend_c2s_groupfriend` với tên nhóm dạng `"<đơn vị>\n<nhóm con>"`, `friend_c2s_erasefriend`, `friend_c2s_asksyncfriendlist` |
| Core (cả 2 phía) | `Core\Src\KProtocolProcess.cpp:5094-5195`, `KPlayerChat.cpp:2068/2117` | client tách tên nhóm ở `'\n'` (`sParseUGName`); server `MakeEnemy` (bị giết bởi người chơi, `KNpc.cpp:9326`) / `MakeBrother` gửi `friend_c2s_associate` với tên nhóm **GBK** `仇人\n` / `亲人\n` |
| Relay | `MultiServer\S3Relay\FriendMgr.cpp`, `TongConnect.cpp:679-1040` | `CFriendMgr`: bản đồ 2 chiều `m_mapPlayers[tên].friends[tên]{group, cheating, pCheated}`; kho `ZDBTable("dbfriend","friend")` → MySQL `jx1_role.relay_kv` (`store='friend'`, `DBTable_MySQL.cpp`) |

Bản Linux (`D:\ServerLinux\gateway\s3relay\s3relay_y`): `KPlayerRelationMgr` + bảng `Relation(RoleName, Data)`,
đổi tên nhóm trong relay bằng `lang\vn\stringtable_relay.txt` dòng 22-24 (`K_PR_BROTHER "Thân nhân\n"`, `K_PR_ENEMY "Cừu nhân\n"`);
`KOfflineMsgStore` + bảng `OfflineMsg(Receiver, Sender, Msg, LastModify)`.

## 2. Vì sao "không lưu" — 3 gốc đo được

Đo MySQL 04/09 18:25: `relay_kv store='friend'` có **993** dòng, **93.140** mục nhóm `仇人\n` (GBK), **174** mục nhóm rỗng, **0** mục
`Thân Nhân`/`Cừu Nhân` dạng client. Người thật `CaiBang` có 175 cừu nhân trong DB nhưng client không hiện mục nào.

1. **Đổi nhóm không bao giờ xuống DB** — `FriendMgr.cpp` `SetFriendGroup` chỉ gán `rInfo.group`, không bật `dirty`;
   `_StoreSomeone`/`BkgrndUpdateDB` tính `needstore` **chỉ từ cờ `cheating`** ⇒ kéo bạn vào "Thân Nhân"/"Cừu Nhân" trên client
   mất sau khi relay khởi động lại.
2. **Không ghi khi thoát** — `SomeoneLogout` có dòng `//_StoreSomeone(itPlayer);` bị chú thích; chỉ còn timer 5 phút
   (`relay_friendcfg.ini updateinterval=300000`, tối đa 8 người/lần) và lúc tắt relay đúng cách. Relay sập/kill = mất.
3. **Cừu nhân / thân nhân do máy chủ tạo VÔ HÌNH** — server gửi tên nhóm GBK `仇人\n` (`KPlayerChat.cpp:2117`), client tách ra
   đơn vị `仇人` rồi `FindUnitIndex` (`UiChatCentre.cpp:1118`) trả `-1` vì client chỉ có `"Cừu Nhân"` TCVN3 ⇒ `FriendInfo`/`AddPeople`
   bỏ qua. Bản Linux đổi tên trong relay; dự án chưa đổi ở đâu cả.

Thiếu hẳn: **lời nhắn mật tới người ngoại tuyến bị vứt** — `ChannelMgr.cpp:806` `SomeoneChat` trả `codeFail`, client hiện
"Người chơi, X không có trên mạng!" (`GameSpaceChangedNotify.cpp:1305`). Enum `codeStore` có sẵn trong `KProtocol.h:2327` nhưng chưa ai dùng.

## 3. Đã làm (bộ vá `ReverseTools\haohuu_0409\p01_relay_haohuu_offmsg.py`, chạy lại được)

| Tệp | Thay đổi |
|---|---|
| `FriendMgr.h/.cpp` | `NormalizeGroup()`: `仇人\n`→`Cừu Nhân\n`, `亲人\n`→`Thân Nhân\n` (byte lấy đúng từ `UiChatCentre.cpp`), áp khi **nạp DB** (dữ liệu cũ 93k mục hiện ra) và khi đặt nhóm. Bật `dirty` ở Add/Del/SetGroup/Associate/AssociateBevy. **Ghi DB ngay** ở `PlayerAddFriend` (2 phía), `PlayerDelFriend` (2 phía, cả xoá một phía), `SetFriendGroup`, `SomeoneLogout`. Associate (PK bot sinh hàng loạt) chỉ bật cờ, ghi lúc logout/timer |
| `TongConnect.cpp` | `NormalizeGroup` ngay khi nhận `friend_c2s_associate/associatebevy/groupfriend` ⇒ gói `friend_s2c_syncassociate` tới client cũng mang tên đúng |
| `OfflineMsgDB.h/.cpp` (mới) | kho lời nhắn: MySQL `jx1_role.relay_offline_msg` (tự `CREATE TABLE IF NOT EXISTS`), kết nối riêng đọc `DataBase.ini [relaydb]`, prepared statement 100%; tối đa **10 lời nhắn/người nhận**, giữ **30 ngày** (= bản Linux, xem §5) |
| `ChannelMgr.h/.cpp` | `SomeoneChat`: người nhận ngoại tuyến → lưu → feedback `codeStore` (client vẫn hiện câu vừa gửi) + dòng `[Hệ thống] X hiện không trực tuyến, lời nhắn đã được lưu và sẽ chuyển khi X đăng nhập.` trong chính cửa sổ chat với X. `DeliverOfflineMsgs()`: giao từng lời nhắn như chat mật từ người gửi, đầu câu `[Lời nhắn lúc dd/mm hh:mm]`, giao đủ mới xoá |
| `ChatConnect.cpp` | gọi `DeliverOfflineMsgs` tại `Proc1_Normal_EnterGame` (GS chỉ báo relay sau `enumPlayerSyncEnd`, lúc đó client đã `UiStartGame()` mở `KUiMsgCentrePad` — `UiShell.cpp:461`) |
| `Global.cpp` | `OfflineMsg_Init()` sau `g_FriendMgr.Initialize()` (lỗi MySQL ⇒ tự tắt tính năng, relay vẫn chạy); `OfflineMsg_Uninit()` lúc tắt |
| `S3Relay.vcxproj` | thêm 2 tệp mới; **thêm `/execution-charset:windows-1258`** cho Release (trước chỉ có `/source-charset` ⇒ MSVC chuyển mã 1258→ACP máy 1252, hỏng byte F5/D5/E3…), Debug sửa lỗi gõ `/execute-charset` |
| `ToolsMySQL\schema_relay.sql` | DDL `relay_offline_msg` (tham chiếu) |

Bộ vá p01 chỉ chèn ASCII (chữ Việt tạm bằng octal `\326`…), số byte cao mọi tệp **không đổi**. Sau đó chủ yêu cầu *"ghi đúng font"* →
bộ vá `p02_raw_tcvn3.py` đổi 8 chuỗi sang **byte thật** (TCVN3 sinh từ `vn_to_octal.unicode_to_tcvn3_bytes`; GBK đọc thẳng từ
`#define ENEMY_UNITNAME/BROTHER_UNITNAME` trong `KPlayerChat.cpp` — không gõ tay byte nào) và thêm cờ `/execution-charset` nói trên.
Kiểm chứng: exe build lại **chỉ khác 9 byte header** (TimeDateStamp/PDB age) so với bản octal ⇒ chuỗi trong exe giống hệt từng byte;
`check_encoding.py` FFFD = 0, byte cao FriendMgr 24→36, ChannelMgr 72→95 đúng bằng số byte Việt/Hán đã chèn. Từ nay relay ghi raw TCVN3 như Core (RULE 0).
Build `Release|Win32`: COMPILE 0 lỗi, LINK ra `S3Relay.exe` 4.413.952 byte 19:37 (lỗi `MSB3073` chỉ là post-build copy sang `D:\bin` không tồn tại).
Đã thử **đúng các câu SQL** của mã C trên DB tạm (tạo/đếm/chèn/đọc/xoá, byte TCVN3 nguyên vẹn) rồi drop DB tạm.

## 4. Cấu hình (mặc định = bản Linux, không cần thêm gì)

`relay_friendcfg.ini` (cạnh S3Relay.exe), thêm nếu muốn chỉnh:
```ini
[offlinemsg]
enable = 1          ; 0 = tắt, relay chạy như cũ
maxperreceiver = 10 ; s3relay_y 0x80e65dd: cmp [ebp-20h],9 ; jg bỏ
keepdays = 30       ; s3relay_y 0x80e5c29: now - 0x278d00 giây
```

## 5. Bằng chứng bản Linux (mổ `s3relay_y`, ELF32, capstone)
- vtable `KQuery_OfflineMsg` @ `0x81e0d98`: `GetQuery` `0x80e6274` (switch 0..3 = COUNT / INSERT / DELETE / SELECT), `HandleResult` `0x80e6574`.
- `0x80e65dd  cmp dword [ebp-0x20], 9 ; jg 0x80e6868` ⇒ đã có ≥10 lời nhắn thì không lưu.
- `0x80e5c29  sub eax, 0x278d00` (=2.592.000 s = 30 ngày) trước `DELETE FROM OfflineMsg WHERE LastModify<='%s'`.
- Tên nhóm: `stringtable_relay.txt:22-24` `K_PR_BROTHER Thân nhân\n`, `K_PR_MATE Phối ngẫu\n`, `K_PR_ENEMY Cừu nhân\n`.

## 6. Triển khai (CHỜ CHỦ — relay đang chạy, tệp bị khoá)
1. Tắt `S3Relay.exe` đang chạy (GameServer tự nối lại relay theo `retryinterval`).
2. Chạy `bin\multiserver\ChayRelay.bat`: đổi `S3Relay.exe` → `.truoc`, `.moi` → `S3Relay.exe`, rồi mở relay. (Hoặc đổi tên tay.)
3. Kiểm `relay_log\<ngày>\*.log` có dòng `[OfflineMsg] MO: 127.0.0.1:3306/jx1_role bang relay_offline_msg, toi da 10 ..., giu 30 ngay`.
4. MySQL: `SHOW TABLES IN jx1_role LIKE 'relay_offline_msg'` phải có.
Không cần restart GameServer/CoreServer/Client. Quay lui: đổi `.truoc` lại thành `S3Relay.exe`.

## 7. Kiểm thử đề nghị
- A kéo B vào **Thân Nhân** (menu chuột phải) → tắt relay đúng cách hoặc chờ B thoát → mở lại → vào game: B vẫn ở Thân Nhân. Log relay: `Player Group Friend: A [B] (Th�n Nh�n...)` + `store someone: A`.
- A bị người chơi B giết (PK) → A relog → tab **Cừu Nhân** của A có B (gói `friend_s2c_syncassociate` giờ mang `Cừu Nhân\n`). Người cũ như `CaiBang` sau khi relay mới nạp sẽ thấy 175 cừu nhân cũ.
- A nhắn mật B (B offline): A thấy câu mình gửi + dòng `[Hệ thống] B hiện không trực tuyến, ...`; MySQL có 1 dòng. B đăng nhập: cửa sổ chat mật mở tab A với `[Lời nhắn lúc ...] ...`; bảng về 0 dòng. Nhắn quá 10 lần: log `hop cua B DAY (10/10)`, A nhận "không có trên mạng" như cũ.
- Bot: whisper tới bot không qua relay (GS xử lý tại chỗ) — không ảnh hưởng.

## 9. RÀNG BUỘC LUA54 (05/09 12:00 — tin từ phiên wauto-c9: chủ duyệt nâng Lua 4.0.1 → 5.4.7)

`bin\multiserver` là một **đảo Lua riêng** mà giai đoạn 1a của họ chưa phủ (bat của họ chỉ đổi `bin\server` và `bin\client`):

| Tệp trong `bin\multiserver` | Kiến trúc | Nhập | Ghi chú |
|---|---|---|---|
| `S3Relay.exe` (đang chạy) và `S3Relay.exe.moi` của tôi | x86 | `LuaLibDll.dll`, `Engine.dll`, `libmysql.dll` | `.moi` build trước commit 38d65e50 ⇒ còn Lua 4 |
| `engine.dll` (634.368 byte, sha d4c788bb) | x86 | `LuaLibDll.dll` | KHÁC bản client `Engine.dll` (402 KB) — cấu hình Win32 kiểu server |
| `Bishop.exe` | x86 | `Engine.dll` | không dùng Lua trực tiếp |

Relay tạo `Lua_State` qua `KLuaScript` của `engine.dll` rồi gọi `Lua_*` trực tiếp trong `DoScript.cpp` (GM `ExcuteScript`)
⇒ **S3Relay.exe và engine.dll phải cùng một lõi Lua**; lệch (relay Lua54 + engine Lua4 hoặc ngược lại) là sập khi GM chạy script.

Quyết định tạm: **giữ `.moi` bản Lua 4** (nhất quán với `engine.dll` hiện có; các bat Lua54 không đụng `bin\multiserver` nên chủ
swap relay lúc nào cũng an toàn). Đã gộp origin/main 5e2fdf9d vào nhánh và **build thử relay bản Lua54: link sạch, nhập `Lua54Dll.dll`,
chuỗi TCVN3 nguyên** (giữ ở scratchpad, chưa đặt). Khi wauto-c9 phủ `bin\multiserver` (cần `Lua54Dll.dll` Win32 + `engine.dll` Win32
server build Lua54), chỉ cần build lại relay từ origin/main (`build.py --project Sources/MultiServer/S3Relay/S3Relay.vcxproj --config Release --platform Win32`)
rồi đặt `.moi` **cùng lúc** với engine.dll và sửa `ChayRelay.bat` swap thêm `engine.dll`. Đã nhắn wauto-c9 hai câu hỏi (cấu hình Engine cho multiserver; có phủ đợt này không)
+ nit: `S3Relay.vcxproj` @origin/main còn dòng 300 `Lib\release\LuaLibDll.lib` (chỉ bỏ dòng debug), vô hại.

**wauto-c9 trả lời 12:15:** engine.dll 634 KB = cấu hình **"Engine Server Release|Win32"** của `Engine.vcxproj` (OutDir `.\EngineServerRelease\`).
Đợt này **KHÔNG phủ `bin\multiserver`**: S3Relay/Bishop giữ `engine.dll` + `LuaLibDll.dll` cũ (relay không có script .lua trên Windows).
⇒ **ĐỪNG đặt S3Relay.exe.moi bản Lua54, đừng đặt engine.dll Lua54 vào multiserver**; `.moi` Lua 4 hiện tại là đúng. Đợt 2 họ sẽ đưa
engine.dll Win32 server + Lua54Dll Win32 vào cùng lúc với S3Relay. (Cảnh báo kèm: client bị đơ do bat đổi cây script, họ đã lùi client, đang sửa.)

**Đã thử (05/09 12:20) và KHÔNG được:** trả dòng `.lib` của `S3Relay.vcxproj` về `LuaLibDll` → link hỏng: `DoScript.obj` đòi
`__imp__lua4_gettop / lua4_pushstring / lua4_tonumber / lua4_tostring`, vì `Engine\Include\LuaLib.h` mới trên origin/main đã ánh xạ macro
`Lua_*` → `lua4_*`. **Relay trên origin/main gắn với Lua54 ở tầng header**, không tách được bằng vcxproj ⇒ đúng như wauto-c9 nói:
bản relay Lua 4 chỉ build được từ commit **trước 38d65e50** (= `.moi` hiện tại, nguồn e299bbb1, sha `6014c6d1`). Đã hoàn lại vcxproj về nguyên trạng.
**Quy tắc cho relay từ nay đến đợt 2:** KHÔNG build lại relay từ origin/main để đặt `.moi`; nếu buộc phải sửa relay, làm trên nhánh tách
từ e299bbb1 (hoặc `git worktree add <thư mục> e299bbb1` rồi cherry-pick) và ghi rõ trong bàn giao. Đợt 2 (wauto-c9 phủ multiserver) thì
build từ origin/main và swap S3Relay.exe + engine.dll (Engine Server Release|Win32) + Lua54Dll.dll (Win32) cùng lúc.

## 8. Chưa làm / lưu ý
- **Phối ngẫu** (`K_PR_MATE`) không port — dự án chưa có `DoMarry/UnMarry` (xem `jx1-kethon-linux-jx1-0309`).
- Không có đổi tên nhân vật ở JX1 ⇒ không port `UPDATE Relation SET RoleName`.
- Lời nhắn lưu cho **bất kỳ tên** người nhận (kể cả tên không tồn tại) — như Linux; chặn bằng trần 10/người + 30 ngày.
- `MakeBrother` (Lua `SwearBrother`) hiện không script nào gọi; ánh xạ `亲人\n` vẫn có sẵn nếu sau này dùng.
- Server vẫn gửi GBK từ `KPlayerChat.cpp` — relay đổi tên, không cần build Core. Nếu sau này đổi ở Core thì `NormalizeGroup` vẫn vô hại.
