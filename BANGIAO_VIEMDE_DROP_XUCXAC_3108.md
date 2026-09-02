# BÀN GIAO 31/08 tối (cập nhật 01/09 sáng) — Viêm Đế: **spam `nItemGenre=59082`** + **xúc xắc bấm "Cần" ra 0 điểm**

> ## ⚡ CẬP NHẬT 01/09 — GỐC THẬT CỦA LỖI XÚC XẮC LÀ TẦNG CLIENT, ĐÃ VÁ
>
> Sau restart 7:43 sáng 01/09 (hai vá script dưới ĐÃ sống) chủ báo **bấm "Cần" vẫn
> không ra điểm** ⇒ mở lại điều tra, tìm ra **tầng gốc thật**:
>
> **`KWndButton` chỉ báo `WND_N_BUTTON_CLICK` cho CHA TRỰC TIẾP** (`WndButton.cpp:326`),
> mà hai nút Cần/Bỏ qua là **con của ô hàng `m_Row[i]`** (`UiDiceItem.cpp:112-113`),
> không phải của cửa sổ chính — trong khi `KUiDiceItem::WndProc` mới là nơi bắt sự kiện.
> Thông báo bấm nút **chết ngay tại `m_Row` (KWndImage)** ⇒ `SendChoice` không bao giờ
> chạy ⇒ **gói `c2s_diceitem` không bao giờ được gửi** ⇒ server không thấy gì, hết 20
> giây chốt hết-giờ, người bấm vẫn ở trạng thái CHỜ ⇒ "0 điểm". Hằng số, mọi cú bấm —
> giải thích trọn triệu chứng cả 31/08 lẫn 01/09, kể cả khi không có bot.
>
> **Vá (commit `d64e288a`):** `UiDiceItem.h` — `m_Row` đổi `KWndImage` → **`KWndPage`**
> (`WndPage.cpp:20-28`, lớp Kingsoft gốc 2002 sinh ra đúng cho việc này: chuyển tiếp
> `WND_N_BUTTON_CLICK` + chuột lên cha, khuôn PropertiePage). 1 dòng kiểu + 1 include,
> không đổi bố cục. Quét toàn cây S3Client: **chỉ UiDiceItem** dính bẫy này.
>
> **`Game.exe.moi` md5 `F08DE7D14F9E`** (build 8:31 từ HEAD `18a3930b` + vá — superset
> bộ 8:12 của phiên wauto-9c, đã đối chiếu chéo: S3Client sạch ở HEAD). Hai `.moi` Core
> của wauto-9c (`51821825D4EE` / `F9D077F2A653`) **giữ nguyên, swap cùng lượt**.
>
> **Xếp tầng chẩn đoán lại cho đúng:** vá lọc bot 31/08 (dưới) là **tầng phụ vẫn đúng
> và vẫn cần** (khôi phục chốt-sớm như Linux + hết dòng "0 điểm" của bot), nhưng **không
> phải gốc** — gốc là tầng UI client này. Phần "race đồng hồ trễ 1-2s" hôm qua là
> suy luận thừa khi chưa thấy tầng UI; giữ lại làm ghi chú, không phải cơ chế chính.
>
> **Nghiệm thu 01/09:** riêng lỗi xúc xắc chỉ cần **thoát game → `ChoiGame.bat`** (ăn
> `Game.exe.moi` + `CoreClient.dll.moi`; đã kiểm: không có thay đổi giao thức nào sau
> PF13 nên client mới nói chuyện server đang chạy vẫn khớp). Muốn ăn trọn bộ kể cả
> `CoreServer.dll.moi` của phiên wauto-9c thì theo quy trình chuẩn: tắt server →
> `ChayGameServer.bat` → `ChoiGame.bat`. Sau đó vào Viêm Đế → bấm "Cần": phải ra
> **điểm 1-100 ngay lập tức** + cửa sổ đóng liền + nhận đồ nếu điểm cao nhất.
>
> **Phân định khi nghiệm thu (tránh chẩn nhầm "vẫn lỗi"):** thước đo vá UI sống là
> **ĐIỂM hiện NGAY khi bấm**. Đi solo (lọc bot đang sống) thì chốt + trao đồ cũng tức
> thì (m_nSize=1). Đi NHIỀU người thật thì chia đồ đợi **mọi người bấm xong hoặc hết
> 20 giây** — đó là thiết kế gốc (ai cũng được gieo), KHÔNG phải lỗi. Chỉ khi bấm mà
> điểm KHÔNG hiện ngay mới là vá chưa ăn (kiểm md5 Game.exe đang chạy = `F08DE7D14F9E`).

Chủ báo hai triệu chứng khi đánh qua ải Viêm Đế trên bản vừa swap:
1. Console GameServer spam liên tục `KItemSet::AddItemSet2 khong tim thay nItemGenre=59082`.
2. "Đồ xúc xắc tôi bấm **cần** mà báo **0 điểm** và không nhận được gì."

Chủ ra lệnh: **"fix đúng chuẩn của bản Linux chứ không tự chế"** → cả hai bản vá dưới đây
đều được chốt bằng cách **mổ chính `jx_linux_y`** (ELF máy chủ Linux) + đối chiếu script
gốc `D:\ServerLinux`, không suy đoán.

---

## 1. LỖI A — `59082` chính là **toạ độ X của con boss vừa chết**

### Gốc: hai hàm khác nhau **trùng tên** `DropItem`

| | bản Linux | bản JX1 |
|---|---|---|
| địa chỉ / hàm | `jx_linux_y` **0x081200B0** | `LuaDropItem` (`ScriptFuns.cpp:4505`) |
| chữ ký | `DropItem(subworld, x, y, belonger, genre, detail, particular, level, series, luck, magic1..6)` | `DropItem(nNpcIndex, genre, detail, particular, level, series, luck)` |
| thả ở đâu | **toạ độ** truyền vào | **vị trí NPC** `nNpcIndex` |
| của ai | `belonger` (−1 = rơi **chung**) | ép = người chơi ngữ cảnh |

Chứng minh chữ ký Linux bằng disasm (`ReverseTools\port_3hd\t79_dump.txt`):
`cmp eax,9; jle` ⇒ cần **>9 tham số**; tham số 1 → `imul 0x63fc8` (= `sizeof KSubWorld`)
⇒ **chỉ số subworld**; tham số 2/3 giữ `double` rồi `fistp` ⇒ **X/Y (MPS)**; tham số 4 =
belonger. Bốn điểm gọi thật trong script Linux đều khớp thứ tự này (`lib\coordinate.lua:150`,
`challengeoftime\award.lua:221/226`, `fengling_ferry\bossdeath.lua:22/31`, `yandibaozang\npc_death.lua:27`).

`npc_death.lua:28` (JX1) vẫn giữ nguyên lệnh gọi **kiểu Linux 16 tham số**, nên trên JX1
tham số 2 = `x` (toạ độ) bị đọc thành **genre** ⇒ mỗi lần rơi đồ lại spam
`nItemGenre=59082`. Số `59082` nằm đúng cụm x=59040…59200 của tệp sinh quái `waya_01.txt`.

**Hệ quả kép:** (1) **Ngũ Hoa Ngọc Lộ Hoàn không bao giờ rơi** (15 viên/boss ải, 30 viên ở
ải 10 — đúng số dòng spam đếm được); (2) mỗi dòng spam còn **rò 1 khe item rỗng** + thả
một vật thể ma không tên cạnh NPC mang chỉ số trùng `world` (rác trong RAM, restart là sạch).

**Không phải lỗi PF13** — lệnh gọi sai này có từ trước 28/08; nó chỉ lộ ra bây giờ vì vá
mở-mission 30-31/08 mới làm đường thưởng này chạy lần đầu.

### Vá (đúng chuẩn Linux, dùng tiền lệ đã sống trong cây)

`missions\yandibaozang\npc_death.lua:28` (nhãn `[DROPEX 31/08]`, sao lưu `.truoc_dropex_3108`):

```lua
-- CŨ (chữ ký Linux, JX1 hiểu sai):
DropItem(world, x, y, -1, y1[3][1], ..., y1[3][6], 0,0,0,0,0,0);
-- MỚI:
DropItemEx(world, x, y, -1, 0, 0, 0, y1[3][1], ..., y1[3][6], 0,0,0,0,0,0);
```

`DropItemEx` = `LuaHD3_DropItemEx` (`KJx2WarInfra.cpp:1596`) chính là **bản port trung
thành của hàm thả-theo-toạ-độ bên Linux**: nhận MPS, và **tôn trọng `belonger = -1`**
(chỉ `SetItemBelong` khi `nBelonger > 0`, `:1674`) nên đồ rơi **chung** đúng ý Linux —
`DropItem` của JX1 không làm được vì luôn ép của người giết (`ScriptFuns.cpp:4585`).
Ba số `0` chèn giữa là `nVersion / szRandSeed / nQuality` (JX1 bỏ qua).

**Tiền lệ:** chính team đã xử lý một lỗi y hệt theo đúng cách này ở
`tong\collectgoods\npcpoint.lua:123-126`, kèm ghi chú `[3HD 25/08 C46]` giải thích.
Và `fengling_ferry\bossdeath.lua` đã được đổi sang chữ ký JX1 khi dịch — chứng tỏ
`npc_death.lua` chỉ **bị bỏ sót**.

---

## 2. LỖI B — bấm "Cần" ra 0 điểm: **cú bấm chưa bao giờ tới nơi**

Toàn chuỗi nút → gói tin → server đã soi từng khớp và **đúng hết**: nút "Cần" gửi
`DICE_CHOICE_NEED = 1`, nút "Bỏ qua" gửi `0`, handler `c2sDiceItem` có đăng ký
(`KProtocolProcess.cpp:7353`, `:414`), cỡ gói khớp. Nên **không phải bấm nhầm, không
phải gói tin sai**.

Chuỗi `"<tên> đã xúc xắc 0 điểm"` **chỉ in ra cho người còn ở trạng thái CHỜ**
(`DICE_ROLL_WAITING = 1`) lúc chốt phiên — `saizi.lua:73` chỉ xử lý `==0` (hủy bỏ) và
`==2` (đã gieo), trạng thái CHỜ rơi vào chuỗi mặc định với `nNumber = 0`. Nếu bấm được
xử lý thì luôn ra 1-100 và **cửa sổ đóng ngay**; nếu là hủy bỏ thì in "hủy bỏ".

### Vì sao cú bấm không tới nơi

`saizi.lua` mời **cả bot cùng phe** vào phiên. Bot không bao giờ bấm ⇒ điều kiện chốt sớm
`m_nDecided >= m_nSize` (`KItemDice.cpp:171`) **không bao giờ đạt** ⇒ phiên **luôn chạy đủ
20 giây** rồi chốt bằng hết-giờ. Cộng với đồng hồ client trễ hơn server 1-2 giây (client
đếm theo khung hình 18fps, server trừ giờ theo nhịp chung 1 giây, cộng độ trễ mạng), nút
"Cần" **còn sáng trong khi phiên phía server đã chốt và giải phóng** — bấm lúc đó gói bị
`Find()` trả NULL và **bỏ im lặng**. Mỗi bot cũng sinh thêm một dòng "0 điểm" của chính nó.

### Vá (đúng chuẩn Linux)

`missions\yandibaozang\saizi.lua:14` (nhãn `[LOCBOT 31/08]`, sao lưu `.truoc_locbot_3108`):

```lua
if nPidXX ~= nil and nPidXX > 0 and (IsBot == nil or IsBot(nPidXX) ~= 1) then
```

**Vì sao đây là "về chuẩn Linux" chứ không phải tự chế:** bản Linux mời người gieo bằng
`GetTeamSize()/GetTeamMember(i)` — tức **chỉ người thật trong tổ đội**, vì *bản Linux
không có bot*. Lọc bot = trả lại đúng tập người tham gia của bản gốc. Rào `IsBot == nil`
để script vẫn chạy nếu gặp DLL cũ.

### 🔴 Ba thứ **CỐ TÌNH KHÔNG** đổi (đã kiểm: đổi mới là tự chế)

1. **Không** trả `saizi.lua` về `GetTeamSize()/GetTeamMember()`. Cả hai bản đều gọi
   `LeaveTeam()` **trước trận** (`readymap\ready.lua` Linux:243 / JX1:251) nên
   `GetTeamSize()` = 0 ở **cả hai engine** — trả về sẽ **tái hiện đúng lỗi "0 điểm"**.
   Cách gom `GetTmpCamp()/GetNextPlayer(...)` mà JX1 đang dùng chính là **khuôn mà bản
   Linux dùng cho mọi phần thưởng Viêm Đế khác** (`npc_death.lua` Linux:61/81/160/181),
   thậm chí còn chuẩn hơn dòng `GetTeamSize()` của chính `saizi.lua` Linux.
2. **Không** đụng `ndsign` (cờ Viêm Đế Lệnh không reset trong vòng lặp). Đã đối chiếu:
   **bản Linux gốc cũng không reset** (`saizi.lua:33`) — sửa là lệch chuẩn. Nếu chủ muốn
   chặn nhân đôi hình nhân khi chơi tổ đội thì đó là **cải tiến ngoài Linux**, phải quyết
   riêng.
3. **Không** trả `CallPlayerFunction(list[i], msg)` về kiểu Linux. `CallPlayerFunction`
   của JX1 bắt buộc tham số 2 là **một hàm** (`ScriptFuns.cpp:2247` `lua_pushvalue(L,2)`),
   không port nhánh gọi-theo-tên của Linux. Dạng `(list[i], Msg2Player, msg)` hiện tại là
   bắt buộc và vẫn dùng đúng hai hàm gốc của Linux.

---

## 3. Kiểm chứng đã chạy

| | |
|---|---|
| mã hoá | `check_encoding.py` cả 2 tệp: byte cao **không đổi** (219 / 104), FFFD = 0, CRLF đồng nhất |
| cú pháp | dựng lại `syncheck.exe` từ **chính Lua 4.0 của dự án** → `cu phap OK` cả `npc_death.lua`, `saizi.lua`, `head.lua` |
| bẫy thân chunk | `t71_quet_goi_nil_thanchunk.py` → **0 chỗ** |
| công cụ | vá qua `safe_edit.py` (từ chối byte không-ASCII, tự kiểm byte cao) — **không** dùng Edit thường |
| disasm | `ReverseTools\cauhinh\t79_disasm_linux_dropitem_team.py` → `port_3hd\t79_dump.txt` |

**Chỉ sửa script, không đụng C++** ⇒ không cần build lại DLL.

---

## ✅ CHECKLIST CHO CHỦ

1. **Khởi động lại GameServer** (script `.lua` chỉ nạp lúc khởi động ⇒ hai vá này **chưa
   sống** cho tới khi restart). Lần restart này cũng ăn luôn `CoreServer.dll.moi` đang chờ.
2. Vào Viêm Đế, đánh qua vài ải:
   - console **không còn** dòng `nItemGenre=59082` nào;
   - **Ngũ Hoa Ngọc Lộ Hoàn phải rơi** ~15 viên mỗi boss ải (ải 10: 30 viên), rơi **chung**
     — ai nhặt cũng được.
3. Khi hiện ô xúc xắc, bấm **"Cần"**: phải ra **điểm 1-100 ngay lập tức**, cửa sổ **đóng
   liền** (không còn phải chờ hết 20 giây), và nếu điểm cao nhất thì **nhận được đồ**.
4. Nếu vẫn thấy "0 điểm": báo tôi kèm **giây thứ mấy chủ bấm** — còn một tầng nữa
   (bấm trễ thì gói bị nuốt im lặng) tôi đã có sẵn 2 dòng log chẩn đoán để chốt trong một
   lần thử.

Muốn quay lại: đổi tên `npc_death.lua.truoc_dropex_3108` và `saizi.lua.truoc_locbot_3108`
về tên gốc rồi khởi động lại.
