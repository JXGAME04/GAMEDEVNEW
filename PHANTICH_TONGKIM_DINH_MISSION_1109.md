# PHÂN TÍCH: Tống Kim "thoát ra đứng ngoài vẫn báo điểm 2 phe" + "bảng xếp hạng trùng tên" (11/09/2026)

Chủ báo (11/09):
- *"đang bị tình trạng thoát tống kim nữa chừng rồi trận tống kim sau không tham gia nhưng vẫn hiện thông báo điểm số tống kim như đang tham gia - kiểu như dính mission"*
- *"có khi nào do index mỗi người chơi lúc đăng nhập không?"*
- *"không phải do item · tôi đã test đôi lúc tống kim tôi thoát ra đứng ngoài nó vẫn báo điểm số 2 phe tống - kim · bảng xếp hạng tống kim hay bị có tên 2 nhân vật trùng nhau do đánh trận 1 xong rồi đến trận 2 đi tiếp - kiểu như chưa xóa dữ liệu cũ khi xong trận"*

Phần 0–8 là phân tích (đã đọc trên cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` và mã `D:\GAMEDEVNEW`
origin/main e06c1490, đối chiếu Linux `D:\ServerLinux\server1\script`). **Phần 9 = đã sửa (chủ: "fix hết và up lên git - lưu ý fix cẩn thận").**

> **Đính chính (khi thi công):** `GetLocalDateEx` thực ra đăng ký vào `LuaGetTimeByMiao` (`ScriptFuns.cpp:16213`) = **giây epoch**, không phải
> ngày. Khoá trận `TK_VARV_KEY` vì thế đã là **từng trận** (mốc `BeginMission`) — các câu "khoá trận = ngày", "trận sau cùng ngày nạp lại điểm"
> ở §0, §1(c), §4 S4, §5 G6, §6 mục 4, §7 mục 4 **sai, bỏ**. Phần còn lại của S4 (`SetLogoutRV(1)` không ai tắt ⇒ đăng nhập luôn ở 324) vẫn đúng.

## 0. Kết luận ngắn

Tìm được **bốn lỗi thật** khớp đúng hai hiện tượng chủ test. Không cần vật phẩm, không cần lệnh admin:

1. **Máy chủ gửi bảng xếp hạng cho cả người đã rời trận.** Mỗi lần điểm của bất kỳ ai đổi (giết người, giết lính, cờ, người vào trận),
   `KMission::SetPlayerParam` dựng lại top-10 rồi gửi gói xếp hạng tới **ô chỉ số của mọi mục trong mission** — không xét cờ còn hiệu lực, không so ID
   (`KMission.cpp:277-289`, `KPlayer.cpp:11001-11008`). Người **bị đẩy vì đứng im 5 phút** chỉ bị tắt cờ, mục **vẫn nằm trong danh sách tới hết trận**,
   nên đứng ở 324 hay về thành vẫn nhận top-10 cập nhật liên tục. Nếu người đó thoát game ở ngoài, mục vẫn còn (thoát game chỉ gỡ mission **của bản đồ
   đang đứng**), và **ai đăng nhập vào đúng ô đó sẽ nhận tiếp**.
2. **Client không bao giờ xoá dữ liệu Tống Kim.** Cửa sổ Bảng xếp hạng (`KUiRankData`) vẽ mỗi khung từ `m_MissionData`/`m_MissionRank`
   (`UiRankData.cpp:416-466`). Hai mảng này chỉ bị xoá trong `KPlayer::Release()` (`KPlayer.cpp:145-146`) — **không** xoá khi đổi bản đồ, bị đẩy ra
   hay hết trận. Vì vậy ra ngoài mở bảng vẫn thấy tên trận, số liệu hai phe Tống – Kim, giờ còn lại, điểm của mình.
3. **Client không xoá hàng rỗng ⇒ tên trận trước nằm lại ⇒ trùng tên.** Mỗi hàng top-10 chỉ được ghi khi hàng mới **có tên** (`if(tmp.Name[0])`,
   không có nhánh xoá, `UiRankData.cpp:452-463`). Trận 1 có 8 người có điểm, trận 2 mới có 3 người thì hàng 4–8 **vẫn là tên trận 1**. Ai có mặt ở
   cả hai danh sách sẽ hiện hai lần — đúng câu *"như chưa xóa dữ liệu cũ khi xong trận"*.
4. **Máy chủ tự sinh tên trùng.** Top-10 dựng từ mọi mục, **tên lấy theo ô chỉ số tại lúc dựng** (`DataCopy`: `Player[m_ulPlayerIndex].m_PlayerName`,
   `KMission.cpp:40`), còn `AddMSPlayer` không chống trùng (`KMission.cpp:132-157`). Một nhân vật có hai mục, hoặc một mục cũ trỏ vào ô nay thuộc người
   khác, sẽ cho ra một tên xuất hiện hai lần ngay từ máy chủ.

**Trả lời câu hỏi "index lúc đăng nhập": đúng, đó là cái móc.** Mission ghi mỗi người theo cặp (ô `Player[]`, ID). ID là băm tên nhân vật nên cố
định qua mọi lần đăng nhập (`KPlayer.cpp:9468`). Ô trống được cấp theo kiểu vào sau ra trước (`KLinkArray.cpp:44-60` + `KPlayerSet.cpp:161-164`),
nên người vừa thoát đăng nhập lại mà không ai chen giữa sẽ nhận **đúng ô cũ**. Có ba chỗ chỉ theo **ô** mà không so ID: gói xếp hạng (mục 1), dòng chat
`Msg2MSAll`/`Msg2MSGroup` (`KMission.h:214-255`) và vòng tổng kết (`KMission.h:263-291`). Vì thế mục còn sót của người đã đi sẽ **bám sang người đăng
nhập kế tiếp trên ô đó**.

Hai lỗi phụ làm hiện tượng nặng thêm (§4 S4): khoá trận chỉ theo **ngày**, nên trận 17:50, 20:50, 22:50 cùng khoá; ai thoát giữa trận rồi vào lại Mộ
binh ở trận sau bị ép đúng phe cũ và **được nạp lại điểm/giết/quân hàm trận trước**. Ngoài ra `SetLogoutRV(1)` không ai tắt, nên sau khi thoát giữa
trận thì **mọi lần đăng nhập đều hiện ở điểm báo danh 324**.

## 1. Hai hiện tượng chủ test ↔ cơ chế

| Chủ thấy | Cơ chế (đã đọc mã) |
|---|---|
| "thoát ra đứng ngoài vẫn báo điểm số 2 phe" | (a) client giữ `m_MissionData` mãi ⇒ Bảng xếp hạng vẫn hiện tên trận, số liệu hai phe, giờ, điểm của mình (`UiRankData.cpp:421-445`); (b) máy chủ vẫn gửi top-10 cho ô của người bị đẩy / ô mục sót ⇒ bảng còn **nhảy số** khi trong trận có người giết nhau (`KMission.cpp:277-289`, `KPlayer.cpp:11001-11008`) |
| "bảng xếp hạng trùng tên, trận 1 xong sang trận 2 như chưa xoá dữ liệu cũ" | (a) client không xoá hàng rỗng ⇒ tên trận 1 nằm lại dưới tên trận 2 (`UiRankData.cpp:452-463`); (b) máy chủ dựng top-10 từ mọi mục, tên theo ô, `AddMSPlayer` không chống trùng (`KMission.cpp:38-46, 259-270`); (c) khoá trận theo ngày ⇒ điểm trận 1 được nạp lại vào trận 2 (`mission01.lua:21`, `mobinhtk.lua:148-172, 335-354, 407-426`); (d) `KMission::Init()` không xoá `m_MissionLadder` (`KMission.h:165-174`), mission mới dùng lại đúng đối tượng cũ, nên top-10 trận trước còn nguyên cho tới lần dựng lại đầu tiên (thường ngay khi người đầu tiên vào trận — ảnh hưởng nhỏ) |

Bảng điểm nhỏ "Tống – Kim" (`KUiTongKimScore`) và cửa sổ Thông Tin Trận (`KUiTongKimInfo`) thì **ẩn khi đổi bản đồ**
(`GameSpaceChangedNotify.cpp:884-885`). Máy chủ chỉ gửi hai cửa sổ này cho mục **còn hiệu lực + còn kết nối + đúng ID** (`UpdateBattleBoxAll`/`UpdateBattleInfoAll`),
nên chúng chỉ hiện lại ngoài trận khi người đó rời 379 mà **không bị gỡ** (xem §3 — lệnh bài admin, Chiêu thư, GM).

## 2. Mission lưu người chơi thế nào

| Thành phần | Mã | Hành vi |
|---|---|---|
| Mục mission | `KMission.h:27-34` | `{ô chỉ số, ID, phe, giờ vào, 18 tham số}`; tham số 0 = **cờ hiệu lực của engine** (`MISSION_PARAM_AVAILABLE = 0`, `GameDataDef.h:91`) — script gọi là "online" |
| ID người chơi | `KPlayerSet.cpp:207`, `KPlayer.cpp:9468` | `g_FileName2Id(tên nhân vật)` ⇒ cố định |
| Cấp ô khi đăng nhập | `KPlayerSet.cpp:161-164`, `KLinkArray.cpp:44-60` | ô trả về chèn **đầu** danh sách trống, đăng nhập lấy **đầu** ⇒ vào sau ra trước |
| `AddMSPlayer` | `ScriptFuns.cpp:11765-11801` → `KMission.cpp:132-157` | luôn tạo **mục mới**, không kiểm đã có mục |
| `PIdx2MSDIdx` | `KMission.h:442-450` → `FindSame` `:48-62` | tra theo **ID**, gặp mục **mới nhất** trước, **ghi đè ô** của mục đó |
| `SetPMParam` | `ScriptFuns.cpp:12720-12745` → `KMission::SetPlayerParam` `KMission.cpp:246-292` | đổi tham số điểm (6) ⇒ dựng lại top-10 từ **mọi** mục (`GetNextPlayer` không lọc, `KMission.h:293-306`), tên theo ô, rồi `SendMSRank` tới ô của **mọi** mục |
| `SendMSRank` | `KPlayer.cpp:10991-11009` | gói "bản thân" chỉ khi mục còn hiệu lực; **10 gói top-10 gửi vô điều kiện** |
| `Msg2MSAll` / `Msg2MSGroup` | `KMission.h:214-255` | mục còn hiệu lực, theo **ô**, **không so ID** |
| `UpdateBattleBoxAll` / `UpdateBattleInfoAll` | `ScriptFuns.cpp` ~12390-12460 | còn hiệu lực + còn kết nối + **đúng ID** (an toàn) |
| Vòng tổng kết `GetNextPlayer(ms, i, 0)` | `KMission.h:263-291` | mục còn hiệu lực, trả **ô**, không so ID |
| Thoát game | `CoreServerShell.cpp:247-251` → `KPlayerSet.cpp:362-384` | gỡ **một** mục theo ID, **chỉ ở bản đồ đang đứng**, chạy `OnLeave` |
| Đóng mission | `ScriptFuns.cpp:11638-11664` → `KMission.cpp:112-130` | xoá hết người/NPC/hẹn giờ; **không** xoá `m_MissionLadder` |
| Client lưu xếp hạng | `KProtocolProcess.cpp:4725-4740` | ghi đè `m_MissionData`, `m_MissionRank[i]`; chỉ xoá trong `KPlayer::Release()` (`KPlayer.cpp:145-146`, gọi ở `KPlayerSet::Init`) |
| Cửa sổ Bảng xếp hạng | `UiRankData.cpp:416-466`; bật/tắt ở phím tắt 22 (`ShortcutKey.cpp:342-351`) | vẽ khi còn tên mission; hàng rỗng **không xoá**; máy chủ không có lệnh tự bật (`GDCNI_RANKDATA` chỉ có ở phía client) |

## 3. Các đường rời bản đồ 379 và số phận mục mission

| Đường rời | Mã | Mục mission | Hệ quả |
|---|---|---|---|
| Thoát game / rớt mạng **trong 379** | `KPlayerSet.cpp:362-384` | gỡ, `OnLeave` chạy | sạch ở máy chủ; nhưng `SetLogoutRV(1)` + điểm hồi sinh 324 + `T_CHECKPHETK` + `T_SAVE_TK_*` còn nguyên (§4 S4); client vẫn giữ dữ liệu xếp hạng |
| **Đứng im 5 phút** (người chơi thường, rất hay gặp) | `KNpc.cpp:3123-3134` → `tongtu.lua:115-164` / `kimtu.lua:114-…` | **còn trong danh sách**, cờ = 0 | không nhận bảng điểm/Thông Tin Trận/dòng chat; **vẫn nhận top-10** (§0-1); không vào vòng tổng kết ⇒ không thưởng, **không được dọn** (`T_CHECKPHETK`, `SetLogoutRV(1)`, điểm hồi sinh 324 còn); điểm cất vào `T_SAVE_TK_POINT`. Thoát game ở 324 ⇒ mục **vẫn còn**, người vào ô đó nhận top-10 |
| Lệnh bài admin → *Tống kim → Báo danh bên Tống/Kim trung cấp* | `lenhbaiadmin.lua:500` → `lib_tktc.lua:754-811` | **còn, cờ = 1** | ở ngoài vẫn nhận **mọi thứ** (bảng điểm bật lại do `SetScore → Show()` `UiTongKimScore.cpp:144`, dòng chat, top-10); hết trận bị kéo về 324 + thưởng; thoát game ở ngoài ⇒ mục sót cờ = 1 |
| Tống Kim Chiêu thư 6/1/154 (chủ xác nhận không liên quan; ghi lại cho đủ) | `magicscript.txt:156` → `vatpham\battles\rescript.lua` | **còn, cờ = 1** | như lệnh admin. Bản dự án **bỏ kiểm tra bản đồ**; Linux chặn toàn bộ `tbBATTLEMAP` gồm 378–380 (`item\battles\rescript.lua:14-53`, `battlehead.lua:280`). Không shop/script nào phát |
| Thổ Địa / Hồi thành / Thần Hành phù | `thodiafu.lua:22`, `townportal_l.lua:33`, `shenxingfu.lua:238` | — | **chặn** bản đồ 375–386 |
| Xa phu | `lib_tktc.lua:635-636` (chú thích), `xaphu.lua:49` (chỉ chạy ở 323–325) | — | 379 **không có** xa phu |
| Hết trận | `task03.lua:28-175` | xử lý rồi `CloseMission` | sạch ở máy chủ nếu vòng không lỗi; client vẫn giữ dữ liệu xếp hạng |

Nền của mọi "mục sót": engine **không** gỡ mission khi đổi bản đồ (`KNpc.cpp:11995, 12020` bị chú thích từ commit nhập đầu 43bca2e0), móc
`OnLeaveWorld` của 379 không chạy vì `\script\maps\newworldscript.lua` **không tồn tại** (495 bản đồ trỏ tới, `KSubWorld_FireMapScript` bỏ qua im
lặng — `ScriptFuns.cpp:11533-11557`), và script Tống Kim không gọi `DelMSPlayer` cho người thật (chỉ bot: `global\bot_tongkim.lua`). Linux dựa vào engine:
chú thích `battles\marshal\mission.lua:236` (Fanghao_Wu 2006-3-21) nói rớt mạng cũng chạy `OnLeave`, nên `SetLogoutRV(0)` phải đặt ở chỗ NewWorld ra.

## 4. Kịch bản

**S1 — bị đẩy vì đứng im, đứng ở 324/thành.** Mở Bảng xếp hạng thấy số liệu lúc bị đẩy (client giữ), hàng top-10 tiếp tục nhảy theo trận (máy chủ gửi
cho ô). Bảng điểm nhỏ và Thông Tin Trận không hiện lại.

**S2 — S1 rồi thoát game ở ngoài, đăng nhập lại.** Mục cờ = 0 vẫn trỏ ô k. Đăng nhập lại ngay nhận lại ô k ⇒ tiếp tục nhận top-10. Người khác vào ô k
cũng nhận. Hết trận `CloseMission` xoá sạch (mục cờ = 0 không vào vòng tổng kết nên không làm lỗi).

**S3 — rời bằng lệnh admin (hoặc Chiêu thư/GM) rồi thoát game ở ngoài.** Mục cờ = 1, ô k.
- Đăng nhập lại đúng ô k ⇒ bám lại 100 %: bảng điểm, Thông Tin Trận, dòng giết địch, top-10; hết trận bị kéo về 324 + thưởng.
- Nhân vật khác vào ô k ⇒ nhận dòng chat Tống Kim; tên của họ còn **thế chỗ** trong top-10 (tên theo ô) ⇒ trùng tên nếu họ cũng đang đánh.
- Hết trận khi ô k trống hoặc thuộc người không có mục ⇒ `PIdx2MSDIdx` = 0 ⇒ `TAB_PHE_TONGKIM[0][6]` lỗi (`task03.lua:189`) ⇒ vòng dừng, **không**
  `SetMission(1..20,0)` / `CloseMission` / `DelAllNpc` ⇒ trận sau `OpenMission` gặp mission cũ (`LuaInitMission` `_ASSERT(0); return 0`,
  `ScriptFuns.cpp:11476-11480`), `BeginMission` không chạy, `StartMissionTimer` tra theo id (`:11709`) nên chạy tiếp **trên mission cũ**, NPC bị thêm
  chồng ⇒ người trận trước không vào vẫn nhận điểm trận sau, lặp tới khi khởi động lại GameServer. Dấu vết chắc chắn: `ScriptError.log` có
  `…task03.lua:189: attempt to index a nil value` + `Script Name: (\script\timertask\task03.lua) cFuncName:(OnMissionTimer)`. Tệp hiện có 48 dòng
  (06/09 21:03 → 09/09 14:29), **không** dòng task03 ⇒ S3 chưa xảy ra trong khoảng đó.

**S4 — người chơi thường thoát game (hoặc bị đẩy) giữa trận rồi đánh trận sau cùng ngày.**
- `SetLogoutRV(1)` (`mobinhtk.lua:370/441`) lưu DB `cUseRevive` (`KPlayerDBFuns.cpp:1060`) ⇒ đăng nhập ở điểm hồi sinh (`:267-280`) = 324
  (`SetRevPos(MAP_BD_TC, phe)`). Chỉ `PlayerEndTongKim` (`task03.lua:233`) và bot tắt nó.
- Khoá trận = ngày (`mission01.lua:21`); lịch sống `ch_lich.lua:75` gồm 01:58, 17:50, 20:50, 22:50, mỗi trận 60 phút (`:71`).
  Vào Mộ binh trận sau cùng ngày: `sj_checklastbattle()` = 1 và `T_CHECKPHETK` > 0 ⇒ **ép đúng phe cũ** (`mobinhtk.lua:155-162`), rồi `common_*`
  nạp lại `T_SAVE_TK_*` (điểm, giết, chết, liên trảm, quân hàm, cờ) vào trận mới (`:335-354`, `:407-426`). Cờ chỉ xoá khi sang ngày (`lib_ham.lua:281-282`).

**S5 — mục trùng.** Bị đẩy (hoặc rời bằng lệnh admin) rồi vào lại Mộ binh **cùng trận** ⇒ `AddMSPlayer` tạo mục thứ hai. Nếu mục cũ còn cờ = 1 (lệnh
admin), vòng tổng kết gặp nhân vật **hai lần** ⇒ kéo về và **thưởng 2 lần**. Thoát game trong 379 chỉ gỡ mục mới ⇒ mục cũ thành mục sót (S3). Mục cũ
có điểm > 0 thì top-10 hiện tên hai lần. Bị đẩy thì điểm mục cũ về 0 (`tongtu.lua:147`) nên không lên bảng.

## 5. Nguyên nhân gốc

| # | Chỗ | Sự thật |
|---|---|---|
| G1 | `KMission.cpp:246-341` + `KPlayer.cpp:10991-11009` | dựng top-10 và gửi `SendMSRank` cho **mọi** mục, không lọc cờ/ID/kết nối; tên theo ô |
| G2 | `UiRankData.cpp:416-466` + `KPlayer.cpp:145-146` | client không xoá dữ liệu xếp hạng khi rời trận/đổi bản đồ; không xoá hàng rỗng |
| G3 | `KMission.cpp:132-157` | `AddPlayer` không chống trùng theo ID |
| G4 | `KMission.h:165-174` | `Init()`/`StopMission` không xoá `m_MissionLadder` |
| G5 | `tongtu.lua`/`kimtu.lua` `OnPlayerTimerIdle` | đẩy ra chỉ tắt cờ, **không** `DelMSPlayer`, không tắt `SetLogoutRV`, không xoá `T_CHECKPHETK` (bot thì có: `bot_tongkim.lua`) |
| G6 | `mission01.lua:21` | `TK_VARV_KEY` = ngày, không phải từng trận |
| G7 | `KNpc.cpp:11995/12020`, MapList 495 bản đồ → `newworldscript.lua` thiếu | đổi bản đồ không gỡ mission, móc rời bản đồ không chạy |
| G8 | `KMission.h:214-255, 263-291` | chat mission và vòng tổng kết chỉ theo ô, không so ID |
| G9 | `task03.lua:73-172` | vòng tổng kết không bỏ qua mục sót/mục trùng; một lỗi làm mất `CloseMission` |

## 6. Đề xuất sửa (CHƯA LÀM — chờ chủ chọn)

| Ưu tiên | Việc | Tầng | Trị |
|---|---|---|---|
| 1 | `KUiRankData::Breathe`: hàng mới rỗng ⇒ `Clear()` cả hàng; không còn tên mission ⇒ xoá trắng/đóng bảng. Xoá `m_MissionData`/`m_MissionRank` khi nạp bản đồ mới (cạnh `GameSpaceChangedNotify.cpp:884`) | client (Game.exe + CoreClient), **không đổi gói tin** | trùng tên do hàng cũ, "ra ngoài vẫn thấy số liệu" |
| 2 | `SetPlayerParam`/`UpRankAllParam`: chỉ dựng top-10 và chỉ gửi cho mục **còn hiệu lực + đúng ID ở ô đó** (như `UpdateBattleBoxAll`); `SendMSRank` chỉ gửi top-10 khi còn kết nối; `StopMission` xoá `m_MissionLadder` | CoreServer | người ngoài hết nhận xếp hạng; hết tên thế chỗ |
| 3 | `OnPlayerTimerIdle`: lưu `T_SAVE_TK_*` rồi `DelMSPlayer(MS_TONGKIM, 0)` như bot, `SetLogoutRV(0)`, trả điểm hồi sinh | script | người bị đẩy rời hẳn mission |
| 4 | Khoá theo từng trận: `GetLocalDateEx()*10 + số trận` (hoặc giờ `BeginMission`) | script | vào lại **cùng trận** vẫn giữ điểm (đúng ý thiết kế); trận khác bắt đầu từ 0 |
| 5 | Lúc đăng nhập: `T_CHECKPHETK` > 0 mà không có trận hoặc khác khoá ⇒ xoá cờ + `T_SAVE_TK_*`, `SetLogoutRV(0)`, trả điểm hồi sinh | script | hết "đăng nhập luôn ở 324" |
| 6 | `task03` bền: bỏ qua mục có `PIdx2MSDIdx` = 0 hoặc đã xử lý, chặn phe ngoài 1..2, bọc từng người bằng `xpcall`, **luôn** `SetMission` + `CloseMission` + `DelAllNpc` | script | chặn S3 tận gốc |
| 7 | Móc rời bản đồ riêng cho 379/380 (`OnLeaveWorld` ⇒ `DelMSPlayer` + `SetLogoutRV(0)`, bỏ qua lúc tổng kết) + Chiêu thư chặn bản đồ như Linux | script + MapList (khởi động lại GS) | lối ra admin/GM/Chiêu thư |
| 8 | (tuỳ chủ) `AddPlayer` dùng lại mục cùng ID; `Msg2All/Msg2Group` so ID | CoreServer | chạm mọi mission — duyệt riêng |

Không khuyến nghị: bỏ chú thích `KNpc::ChangeWorld` (đổi hành vi mọi mission: Kiếm Môn Quan, Vận Tiêu, Viêm Đế, liên đấu…); tạo
`\script\maps\newworldscript.lua` chung cho 495 bản đồ (bật lại hàng loạt cờ: `CreateTeam_OFF` tự rời tổ đội ở 193 bản đồ, `PUNISH_PK10`,
`STALL_OFF`…) — đổi lối chơi, chủ quyết riêng.

Gợi ý gói nhỏ nhất chữa đúng hai hiện tượng chủ báo: **1 + 2 + 3** (client + CoreServer + một script). **4 + 5 + 6** là dọn phần "dính" còn lại.

## 7. Cách thử để chủ tự khẳng định (không cần sửa mã)

1. Vào trận, đứng im 5 phút để bị đẩy về 324, mở Bảng xếp hạng: đầu bảng giữ số liệu lúc bị đẩy, hàng top-10 vẫn nhảy khi trong trận có người giết nhau.
2. Hết một trận, về thành mở Bảng xếp hạng: vẫn còn số liệu trận vừa xong.
3. Trận 1 có nhiều người có điểm, trận 2 lúc ít người có điểm: các hàng dưới giữ tên trận 1 ⇒ trùng tên.
4. Vào trận 17:50, thoát game giữa chừng rồi đăng nhập lại: hiện ở 324. Trận 20:50 nói chuyện Mộ binh: bị ép phe cũ, điểm và quân hàm cũ.
5. Sau mỗi trận xem `bin\server\ScriptError.log` có dòng `task03.lua` không.

## 8. Đã loại / đính chính so với bản nháp trước

- Xa phu và trap `tongkimchientruong_to_*` **không** phải lối ra của 379 (xa phu bị chú thích; trap thuộc bản đồ thường "Tống Kim chiến trường").
- Phù về thành / Thổ Địa / Thần Hành chặn 375–386.
- Người bị đẩy vì đứng im **không** nhận bảng điểm nhỏ và dòng chat (tham số 0 là cờ hiệu lực của engine) — nhưng **vẫn nhận top-10**.
- Ủy thác: rớt mạng vẫn qua `PrepareRemove` (gỡ mission) rồi mới giữ nhân vật (`KPlayer.cpp:1029-1035`).
- Bot: `DelMSPlayer` khi rời (`bot_tongkim.lua`), bị gỡ bằng `PrepareRemove` → `RemoveQuiting` (`KPlayerBot.cpp:1575-1592`).
- Trận chồng trận: lịch hiện tại cách nhau ≥ 2 giờ, trận 60 phút ⇒ không.
- Biến `SubWorld` trong `task03` chỉ do engine đặt (`KMission.cpp:90-91`) ⇒ `CloseMission` không trượt vì lý do này.
- "Thưởng trận Tống Kim này: Hạng…" (`Msg2SubWorld` = thông báo toàn máy chủ, `ScriptFuns.cpp:3837-3845`) và "Đại chiến Tống Kim đã kết thúc. Tích lũy
  Tống A:B Kim" (`AddGlobalNews`) là thông báo **toàn máy chủ theo thiết kế**.
- Số liệu: `CreateTeam_OFF` ở 193 bản đồ (không phải 268); Linux có 17 cờ `aryFuncStore` (không phải 21).

## 9. ĐÃ SỬA (11/09 — chủ: "fix hết và up lên git - lưu ý fix cẩn thận"; thẻ `[TKMS 11/09]` C++, `[TKFIX 11/09]` script)

### 9.1 CoreServer (`Sources\Core\Src`)
- `KMission.h`: mảng người chơi mission có `IsSamePlayer(i)` (ô `Player[]` mà mục trỏ tới vẫn đúng ID) và `IsLive(i)` (còn hiệu lực + đúng
  người). Dùng ở `Msg2Group`, `Msg2All` (dòng chat mission), `GetNextPlayerC` (Lua `GetNextPlayer` = vòng tổng kết), `GetGroupPlayerCount`,
  `GetOnlinePlayerCount` (Lua `GetMSPlayerCount`), `GetMissionPlayer_PlayerIndex` (Lua `MSDIdx2PIdx` trả 0 khi ô đã đổi chủ). `Init()` và
  hàm dựng xoá `m_MissionLadder` (top-10 trận trước không sống sang trận sau). Vì `KMission.h` không include `KPlayer.h` và các hàm inline
  của `KMission` instantiation ngay trong header, ID đọc qua hàm ngoài `KMission_PlayerIdAt()` (thân ở `KMission.cpp`).
- `KMission.cpp`: `UpRankAllParam` (trước đây không ai gọi) thành thân chung: top-10 chỉ dựng từ mục còn sống, chỉ gửi cho mục còn sống
  **và còn kết nối**; `SetPlayerParam` gọi nó; `RemovePlayer` dựng lại top-10 ngay khi một người rời (thoát game / bị đẩy / rời map);
  `StopMission` chỉ đặt nhóm −1 + `SendMSGroup` cho đúng người.
- `KPlayer.cpp SendMSRank`: không gửi 10 gói top-10 khi không còn kết nối (bot, đang thoát).
- Không đổi gói tin, không đổi cấu trúc dữ liệu.

### 9.2 Client (`Game.exe` + `CoreClient.dll`)
- `KProtocolProcess.cpp s2cSetMissionData`: nhóm −1 (máy chủ gửi khi bị đẩy / rời trận / hết trận) ⇒ xoá `m_MissionData` + `m_MissionRank`.
- `UiRankData.cpp Breathe`: không còn tên trận ⇒ xoá trắng (một lần); hàng top-10 rỗng ⇒ xoá hàng (hết "tên trận trước nằm lại").
- Còn lại (không sửa): rớt mạng giữa trận rồi vào lại mà không vào trận ⇒ bảng còn số liệu cũ tới khi có gói mission kế tiếp (client
  không có móc đăng nhập tiện để xoá; nhỏ, tự hết khi vào trận sau).

### 9.3 Script máy chủ (đã đặt trên cây chạy thật + chép vào `serverscript_live\script` của repo; **cần restart GameServer**)
| Tệp | Sửa |
|---|---|
| `tinhnang\tong_kim_tcap\tongtu.lua`, `kimtu.lua` `OnPlayerTimerIdle` | sau khi lưu `T_SAVE_TK_*` ⇒ `DelMSPlayer(MS_TONGKIM, 0)` + `SetLogoutRV(0)` (như bot và Linux `bt_pop2signmap`) |
| `tinhnang\phuban\mission01.lua` `OnLeave` | param 0 = 0 trước khi vào (bị đẩy / bot) ⇒ không báo "đã rời" lần nữa, **không ghi đè** `T_SAVE_TK_*` |
| `timertask\task03.lua` | `ontime_tongkim` = `pcall` + luôn `SetMission/CloseMission/DelAllNpc`; từng người trong `pcall`, bỏ mục sót (`PIdx2MSDIdx` = 0) và trùng; `PlayerEndTongKim` kiểm `dataindex`/`PlayerIndex`/`nPhe`, đặt param 0 = 0 **trước** `NewWorld` |
| `tinhnang\tong_kim_tcap\mobinhtk.lua` | `TK_GoMucCu()` trước 4 chỗ `AddMSPlayer`: còn mục cũ của chính mình ⇒ gỡ (OnLeave lưu điểm) rồi mới thêm |
| `nhanvat\nguoichoi\playerlogin.lua` | `TK_DonCoLucDangNhap()`: còn `T_CHECKPHETK` mà không còn mission / khoá khác ⇒ xoá cờ + `SetLogoutRV(0)` (hết "đăng nhập luôn ở 324"); đang cùng trận ⇒ giữ để vào lại |
| `vatpham\battles\rescript.lua` | Tống Kim Chiêu thư chặn bản đồ 375–386 như Linux |
| **mới** `maps\tongkim\newworld_tk.lua` + `settings\MapList.ini` `379_NewWorldScript=\script\maps\tongkim\newworld_tk.lua` | `OnLeaveWorld`: rời 379 bằng cách khác thoát game (lệnh bài admin, GM, …) ⇒ `DelMSPlayer` + `SetLogoutRV(0)`; thoát game (còn đứng trên 379) hoặc param 0 = 0 ⇒ để nguyên. `OnNewWorld` rỗng (không kích hoạt `PARTNER_OFF|TISHENZHIREN`) |

### 9.4 Nhị phân `.moi` chờ chủ swap
- `bin\client\Game.exe.moi` **0e327ee3** · `bin\client\CoreClient.dll.moi` **fcb33a1d** (ChoiGame.bat).
- `bin\server\CoreServer.dll.moi` **d5fef156** (ChayGameServer.bat — restart cũng nạp MapList + script mới).
- Client mới chạy được với máy chủ cũ và ngược lại (gói tin không đổi) — nhưng nên đổi cả ba.

### 9.5 Cố ý KHÔNG đổi
- Không bỏ chú thích `KNpc::ChangeWorld` (chạm mọi mission); không tạo `\script\maps\newworldscript.lua` chung 495 bản đồ; `KMission::AddPlayer`
  giữ nguyên (chống trùng làm ở script Tống Kim); `OnLeave` vẫn không tắt `SetLogoutRV` khi thoát game (đúng Linux — vào lại trận được),
  phần dọn để `playerlogin.lua` lo khi trận đã hết.

### 9.6 Kiểm sau swap
1. Vào trận, đứng im 5 phút bị đẩy về 324: hết nhận dòng giết địch; mở Bảng xếp hạng (phím tắt 22) thấy trắng; `GetMSPlayerCount` giảm.
2. Hết trận, về thành mở Bảng xếp hạng: trắng. Trận sau ít người: không còn tên trận trước ở hàng dưới.
3. Thoát game giữa trận, đăng nhập lại **sau khi hết trận**: không còn ở 324 ở lần đăng nhập kế tiếp; vào Mộ binh trận sau bắt đầu từ 0.
4. Lệnh bài admin *Tống kim → Báo danh* từ trong 379: chat "đã rời khỏi chiến trường", không nhận điểm nữa, hết trận không bị kéo về.
5. `ScriptError.log` không có dòng `task03.lua` / `newworld_tk.lua`; console không in `[TKFIX] loi ...`.
