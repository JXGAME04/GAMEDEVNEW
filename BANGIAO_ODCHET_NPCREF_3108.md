# BÀN GIAO 31/08 — "Ô CHẾT": gốc thật của *đánh không trúng con quái cuối trong Viêm Đế*

Commit `aaf5bb24`. Ghi nhớ: `jx1-npcref-o-chet-3108`.
Báo cáo điều tra đầy đủ (45 tác nhân, 38 ứng viên, 23 qua phản biện):
`C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\61d924bb-ce66-46e1-a22b-820305b4bde7\tasks\w3r80x3vl.output`

> ⚠️ **NHIỀU PHIÊN CÙNG LÀM CÂY NÀY.** Hôm nay phiên khác đã swap DLL lúc 10:53 / 11:33 /
> 11:56 / 12:06. Trước khi dựng lại DLL **phải** liệt kê tệp nguồn mới hơn DLL đang chạy.
> Lúc tôi dựng (12:41) chỉ có đúng `KNpc.cpp` của tôi mới hơn — nên bản `.moi` = bản
> 12:06 của họ **cộng đúng thay đổi của tôi**, không nuốt việc dang dở của ai.

---

## 1. Triệu chứng chủ báo

> "quái trong viêm đế hay bị lỗi đánh không mất máu dạng như là bị lag quái"
> "đánh hết quái chỉ còn **1 con quái duy nhất** là hay đánh ko mất máu **nhưng quái vẫn đánh tôi được**"
> "vẫn còn tình trạng **đánh không trúng** khi còn 1 con quái trong ải"

## 2. Gốc — một dòng thừa

`KNpc.cpp:2344`, nằm trong khối chú thích `[PORT5 23/08]` ⇒ **mã mới thêm 23/08**,
đúng lúc triệu chứng xuất hiện.

```
KNpc.cpp:2306      DoRevive: DecRef(m_MapX, m_MapY, obj_npc)          TRỪ LẦN 1 (đúng)
KNpc.cpp:2307      NpcChangeRegion(m_RegionIndex, VOID_REGION, m_Index)
KSubWorld.cpp:2368 if (nDesRnIdx != VOID_REGION) m_RegionIndex = -1;
                   đích CHÍNH LÀ VOID_REGION (-2, KSubWorld.h:9) ⇒ KHÔNG chạy
                   ⇒ m_RegionIndex GIỮ NGUYÊN giá trị cũ (>= 0) dù NPC đã trả bộ đếm
KNpc.cpp:1718      m_bNoRevive ⇒ ép m_Frames.nTotalFrame = 18 (~1 giây)
KSubWorld.cpp:1148 duyệt m_NoneRegionNpcList -> Npc[].Activate()
KNpc.cpp:920       case do_revive: OnRevive()
KNpc.cpp:2341      if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)    LỌT bằng giá trị cũ
KNpc.cpp:2344          DecRef(m_MapX, m_MapY, obj_npc)                TRỪ LẦN 2  *** LỖI ***
```

**Sổ cái:** 1 lần `AddRef` lúc sinh (`KNpcSet.cpp:542`) đổi lấy **2 lần `DecRef`**.
`KRegion::RemoveNpc` (`:845-866`) và `KNpcSet::Remove` chỉ thao tác `m_Node`, **không**
`DecRef` ⇒ không có lần thứ ba, cũng không có gì bù.

Ô đang có con **khác** đứng thì lần trừ thứ hai **ăn mất phần đếm của con đang sống**
(`KRegion.cpp:29 g_nPbNpcChan = 0` ⇒ NPC chồng ô thoải mái). Hậu quả:

```cpp
// KRegion.h:191  KRegion::FindNpc
if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)
    return 0;          // thoát NGAY, KHÔNG duyệt m_NpcList
```

⇒ **mọi thứ đứng trên ô đó tàng hình trước mọi phép va chạm**, dù vẫn sống, vẫn đánh trả.
Chỉ tự lành nếu nó **di chuyển** sang ô khác.

**Vì sao riêng Viêm Đế:** 100% quái Viêm Đế có `m_bNoRevive = 1` —
`npc.lua:19-22` (`YDBZ_AddNpcEx = HD3_AddNpcEx`) và cả 4 điểm gọi
(`npc.lua:324-333 / 386 / 427 / 504-513`) đều truyền **tham số thứ 7 = 1** →
`KJx2WarInfra.cpp:1740-1741`. Quái sinh theo **cụm trong ô vuông 7×7** (`npc.lua:500`)
nên xác suất xác chết chung ô với con đang sống rất cao và **lặp lại mỗi đợt**.
Quái thường (không `bNoRevive`) đi nhánh `Revive()` → `AddRef` lại → **cân bằng**.

## 3. Bằng chứng đo được

Chia tỉ lệ chạm **theo từng ô** (nhãn `[S5-SCAN]`) — phép thử phân định "con quái hỏng"
với "ô hỏng":

| con 91481 đứng ở | lần quét | lần chạm | tỉ lệ |
|---|---|---|---|
| **ô (vùng 37; 7,2)** | **440** | **0** | **0,0 %** |
| ô (vùng 91; 8,31) | 8 | 2 | 25 % |
| ô (vùng 89; 6,10) | 4 | 2 | 50 % |
| ô (vùng 87; 9,3) | 4 | 1 | 25 % |

Trên 73 ô đủ mẫu: **72 ô chạm 24,9 %, đúng 1 ô chết.**
Cùng con quái mà ô khác đánh được ⇒ hỏng ở **Ô**. Đây cũng đúng chữ ký cơ chế dự đoán
(lành khi di chuyển) — hai đường độc lập gặp nhau.

Phễu đầy đủ một đòn: `S3-PKT-IN` 178 gói → `S2-NETSKILL-TARGET accept=1 dist=5`
(tầm 90) → `S4-CAST` ×111 → 95 khung đòn cách quái **≤ 3 mps** → `map9=.........`
(FindNpc rỗng cả 9 ô) → `S4-MSL-END lasthit=0`. Trong khi `HIT-OK launcher=92777` ×26
(nó đánh lại được).

## 4. Đã vá

| việc | tệp | |
|---|---|---|
| gỡ `DecRef` thừa | `Sources\Core\Src\KNpc.cpp:2344` | xoá **đúng 1 dòng** |
| thêm nhãn nghiệm thu `[REFOAN]` | `Sources\Core\Src\KRegion.cpp:1194` | đặt **trong nhánh vốn đã hỏng** ⇒ đường chạy bình thường tốn 0 |

Bộ vá: `ReverseTools\cauhinh\t76_va_decref_onrevive.py`, `t77_do_refoan.py`
(diễn tập mặc định, `--ghi` mới ghi; có sao lưu `.truoc_va_decref_3108` /
`.truoc_refoan_3108`).

**Giữ nguyên** `RemoveNpc` (gỡ `m_Node` khỏi `m_NoneRegionNpcList` — bỏ đi để lại nút
mồ côi) và `NpcSet.Remove` (giữ đúng hành vi `bNoRevive`).

**Bản vá KHÔNG làm gì:** không đổi cân bằng game · không đổi cấu trúc dữ liệu · không
đổi gói tin · không sửa `.lua` nào · không đụng `KRegion.h`/`FindNpc`.

### 🔴 CẤM vá ở `KSubWorld.cpp:2368`
Ép `m_RegionIndex = -1` ở đó thì chốt `KNpc.cpp:2341` sai ⇒ `NpcSet.Remove` **không bao
giờ chạy** ⇒ **rò rỉ khe NPC vĩnh viễn**; đồng thời hỏng `DelAllNpcInWro`
(`KSubWorld.cpp:3003` và `:3037` dùng chính giá trị đó làm phép thử) và mất gói
`s2c_npcremove` (`KNpcSet.cpp:565`).

## 5. Nhị phân

| | |
|---|---|
| đang chạy | `18.206.720` byte, md5 `74b55a4febfe` (31/08 12:06) |
| **chờ swap** | `bin\server\CoreServer.dll.moi` md5 **`9ed3cb2a907f`** |
| sao lưu | `bin\server\CoreServer.dll.cu_3108_truoc_decref_74b55a4f` |

Dựng sạch **cả hai** cấu hình: `Server Release|x64` (link OK) và `Client Release|Win32`
(**0 lỗi** — `KRegion.cpp` là tệp dùng chung nên bắt buộc kiểm). `KRegion.obj` và
`KNpc.obj` đều được dựng lại ở cả hai. Chuỗi `[REFOAN]` **có mặt trong DLL mới**.

### 5b. Cập nhật 31/08 chiều-tối (phiên tiếp theo)

Bản `9ed3cb2a907f` ở trên đã được GỘP vào đợt Phi Phong (`0024C5E945C6`) và **chủ đã swap
lúc ~15:24** — vá gốc ĐANG SỐNG. Sau đó:

| | md5 (12 đầu) | ghi chú |
|---|---|---|
| đang chạy (từ 15:24) | `0024C5E945C6` | vá gốc ô chết + Phi Phong |
| `.moi` 15:52 (phiên wauto-9c) | `AFAF8E402E06` | + KItem 31/08e (`d545facb`) — ĐÃ ĐƯỢC GỘP TIẾP |
| **`.moi` HIỆN TẠI (16:29)** | **`6931F6C401C9`** | = tất cả trên **+ đợt vá vệ sinh 7 điểm (mục 7)**, 18.211.328 byte |

`bin\client\CoreClient.dll.moi` `751F86EB207D` (của wauto-9c, KItem 31/08e) **ĐÃ ĐƯỢC
CHỦ ĂN ~16:30** (bin\client hết `.moi` — phiên wauto-9c xác nhận 16:3x) — client phần
đó đã sống, chỉ còn `.moi` SERVER chờ restart. Đợt vệ sinh không đổi byte client
(guard nằm trong `#ifdef _SERVER`, kích thước ClientRelease trước/sau y hệt 2.423.296).
`Game.exe` không cần swap nữa (bản `9374C3DFFD0D` đã sống từ 15:24).

⚠️ Phiên wauto-9c sắp build tiếp (port ≤8 handler thuộc tính đá vào `KNpcAttribModify`)
và sẽ **đè `CoreServer.dll.moi` bằng bản superset** (ôm cả 7 guard `[REFOAN-VS]`) rồi
báo md5 — bản `.moi` cuối cùng trước restart mới là bản chuẩn, đối chiếu tin nhắn
giữa hai phiên.

## 6. Nghiệm thu

1. Sau khởi động lại, chạy một trận Viêm Đế, đếm `[REFOAN]` trong `jx_auto_server.log`.
   **Phải về gần 0.** Còn nhiều ⇒ **còn nguồn lệch khác**, mở lại điều tra (mục 7).
2. Giết vài đợt quái rồi đánh con còn lại **đứng tại chỗ** — tỉ lệ trúng phải về mức
   quái thường (**25-32 %**), không còn cảnh đạn xuyên qua.
3. Lệch cũ **không tự khỏi**: các ô đã bị trừ hụt từ phiên trước vẫn lệch cho tới khi
   vùng được nạp lại ⇒ **bắt buộc khởi động lại**, không hot-swap.

## 7. ✅ ĐÃ THI CÔNG 31/08 chiều — đợt vá vệ sinh (mục này nguyên là "còn treo")

> Phiên tiếp theo (16:28) đã áp trọn đợt vệ sinh: **7 điểm** (6 điểm dưới + 1 điểm mới do
> phản biện tìm ra), script `ReverseTools\cauhinh\t78_va_vs_decref_dorevive.py`
> (diễn tập mặc định, `--ghi` mới ghi; sao lưu `.truoc_va_vs_3108`). Phản biện 4 tác tử
> độc lập (650k token) TRƯỚC khi ghi: bất biến "do_revive ⟺ đã trả ô" vững trên mọi
> đường server (`m_Doing = do_revive` gán đúng 1 chỗ toàn cây, `KNpc.cpp:2293`, cùng hàm
> với DecRef `:2307`, không có khe Lua chen giữa; `Revive()` AddRef `:8916` TRƯỚC khi rời
> do_revive `:8923`). Script chết (`OnDeath` `KNpc.cpp:1695`) chạy lúc `do_death` TRƯỚC
> `DoRevive` `:1714` nên DecRef của mọi death-script vẫn chạy đúng — guard không đổi
> hành vi đường sống nào đang chạy.
>
> Đã áp (nhãn grep `[REFOAN-VS 31/08]`):
> 1. `KJx2WarInfra.cpp` `LuaClearMapNpc` — ô 82 cũ. Đo giao thông thật: ngoài Viêm Đế
>    còn PLD **mỗi giờ, ngay ĐẦU trận** (`fengling_ferry\mission.lua:13` — bắt đúng xác
>    lượt trước), tongwar/citywar/challengeoftime/bw.
> 2. `KJx2WarInfra.cpp` `LuaHD3_DelNpcByName`
> 3. `KJx2WarInfra.cpp` `LuaHD3_DelNpcByNameEx` — đường mỗi phút: `timerserver.lua:133`
>    → `HD3_Tick` → `HD3_DonNpcCu(0)` (`hd3_driver.lua:209-210`) gọi 2 lệnh này/phút.
> 4. `KJx2WarInfra.cpp` `LuaHD3_DelNpcByScript` — ×3 lệnh/phút cùng đường trên; thêm
>    đường boot/admin `HD3_DriverInit` xoá boss killer-task theo script (xác thật).
> 5. `ScriptFuns.cpp` `LuaDelNpc` — guard bọc `#ifdef _SERVER` (tệp biên dịch cả CLIENT,
>    mà client xác do_revive VẪN giữ ô — `KNpc.cpp:2313-2328` không DecRef; tiền lệ
>    `KRegion.cpp:647`). Caller nguy hiểm thật: `dungeon.lua:223` (quét index đã nhớ lúc
>    đóng bí cảnh), `tongcastle.lua:697/830`, `battlehead.lua:494` (delnpcsafe chỉ chặn
>    người chơi, không chặn xác).
> 6. `KMission.cpp` `KMission::RemoveNpc` (`DelMSNpc`) — hiện 6 caller đều gọi lúc
>    do_death/còn sống ⇒ thuần phòng xa.
> 7. **MỚI (phản biện tìm ra):** `ScriptFuns.cpp` `LuaNpcEnterNewWorld` (`NpcNewWorld`,
>    caller thật `event_vantieu\tieudau.lua:191`) — trên xác do_revive, `ChangeWorld`
>    (`KNpc.cpp:10272`) vừa trừ đôi vừa **nối `m_Node` vào region list khi node còn nằm
>    trong `m_NoneRegionNpcList`** (hỏng liên kết, nặng hơn ô chết). Chặn sớm: xác thì
>    không chuyển map, trả 0 (bọc `#ifdef _SERVER`).
>
> Giữ nguyên vô điều kiện `RemoveNpc` + `NpcSet.Remove` ở mọi điểm — phản biện xác nhận
> đây là **bắt buộc** chứ không chỉ an toàn: `KNpc::Remove→Init` không bao giờ gỡ
> `m_Node`, bỏ `RemoveNpc` là để lại nút mồ côi trong `m_NoneRegionNpcList` được
> Activate trên khe đã tái sử dụng.
>
> Ghi chú để lại cho đợt sau (đều KHÔNG chạy được hôm nay, chỉ là mìn tương lai):
> - `KSubWorld.cpp:2287` (nhánh server `GWM_NPC_DEL`): mã chết phía server (cả 2 nơi gửi
>   đều client-only) nhưng thiếu cả chốt `m_RegionIndex>=0` lẫn do_revive.
> - `KSubWorld.cpp:3124-3127` (`RevivalAllNpc`): gán `do_none` cho NPC do_revive nếu nó
>   nằm trong region list — hiện bất khả (xác luôn ở NoneRegion list), nhưng sửa đổi
>   tương lai nào để xác ở lại region list sẽ kích hoạt lại đường trừ đôi này.
> - `KNpcAI.cpp:166/217`: chỉ an toàn GIÁN TIẾP (DoRevive tắt `m_ProcessAI`) — đáng thêm
>   guard cùng khuôn nếu có dịp.
> - `KSimCity.cpp:570/590`: an toàn vì bot SimCity là `kind_player`; riêng
>   `SC_SetBotFlag` có thể gắn cờ NPC thường từ Lua (chưa script nào làm) — đừng làm.
>
> Nhị phân mang đợt này: xem mục 5b bên dưới.

### Nguyên văn mục "còn treo" cũ (đã xử lý như trên)

Cùng khuôn "lấy `m_RegionIndex >= 0` làm bằng chứng còn chiếm ô". Cách vá: thêm đúng
chốt đã có sẵn trong cây (`KRegion.cpp:659`), **giữ nguyên** `RemoveNpc` + `NpcSet.Remove`:

```cpp
if (Npc[n].m_Doing != do_revive)
    ...DecRef(Npc[n].m_MapX, Npc[n].m_MapY, obj_npc);
```

Áp tại `KJx2WarInfra.cpp:82 / 1800 / 1859 / 1896`, `ScriptFuns.cpp:7176`,
`KMission.cpp:234`.

- Đáng vá nhất: `KJx2WarInfra.cpp:1800/1896` — `hd3_driver.lua:210 HD3_Tick →
  HD3_DonNpcCu(0)` chạy **mỗi phút** trên toàn máy chủ, quét cả `nhieptran.lua`
  (boss có xác). **Ngoài phạm vi Viêm Đế.**
- `KJx2WarInfra.cpp:82` (`LuaClearMapNpc`, chạy cuối mỗi trận Viêm Đế): có `DecRef`
  thừa thật **nhưng tự triệt tiêu** (quét xoá sạch mọi NPC không-phải-người, người chơi
  đã bị `YDBZ_kickout()` đẩy ra trước) ⇒ vá cho sạch, **đừng dừng điều tra ở đây**.
- `ScriptFuns.cpp:7217` (`LuaSetNpcPos`) = **mã chết**, 0 lời gọi toàn cây script.
- `KNpcSet.cpp:821` nằm trong `#ifndef _SERVER` ⇒ lỗi **phía client**, không giải thích
  được số đo trên log máy chủ.

## 8. Đã bác bằng dữ liệu — đừng quay lại

vật cản Viêm Đế (`SetObstacleCell` ghi `m_Obstacle[][]` là **mảng riêng**, không đụng
`m_pNpcRef`; ô chết cũng không trùng chỗ đặt vật cản — (7,2) xuất hiện 2/1.039 lần, đúng
mức ngẫu nhiên) · bộ tệp `trap\clear\` "thiếu 667 ô" (thật ra **cố ý mở cửa 3 cột giữa**
một bức tường 9×2) · 0/500 chỗ đẻ quái trùng ô tường · quái đứng khít mốc ô `off(0,0)` ·
quái đứng yên hay di chuyển · khoảng cách (trượt cả khi cách **1 mps**) · xúc xắc
trúng-trượt (người chơi `AR=10.217.167` ⇒ `[S1-MELEE-NOROLL]` **bỏ qua xúc xắc, tự
trúng**) · `S2-MELEE-TOOFAR-RUN` và `S3-TGT-FIND found=0` (mỗi thứ **7** lần cả phiên).

## 9. Bẫy log (đã mất thời gian thật vì chúng)

- `[MSL-TGT-LOST]` và `[MIS-ACT-FOLLOWLOST]` in **TRƯỚC** cửa chặn (`KMissle.cpp:462/468`
  vs chặn ở `:469`) ⇒ **báo động giả**. Cùng loại `[SKILL-REFUSE-FAR]` đã ghi 26/08.
- `AUTOLOG_EVERY(500,...)`: `DMG-TRY` 3809 / `DMG-SKIP-TARGETKIND` 3808 **không phải** tỉ
  lệ 99,97 % — hai bộ đếm cùng bị kẹp nhịp. **Bộ đếm bị kẹp nhịp không còn là bộ đếm**
  (vì vậy `[REFOAN]` cố ý **không** kẹp nhịp).
- `map9=.........` là **bình thường** ở khung đòn chưa tới nơi; chỉ có nghĩa khi lọc
  riêng các khung cách quái ≤ 3 mps.
- `jx_auto_server.log` **xoay vòng ở 64 MB** → phải đọc cả `.log.1`.
- Bật log: `bin\server\config.ini` mục `[AutoLog]` `On=1` / `Name=<tên nhân vật>`.
  **Đọc một lần lúc khởi động** ⇒ đổi `config.ini` phải khởi động lại mới ăn.

---

## ✅ CHECKLIST CHO CHỦ

1. **Tắt GameServer.**
2. Chạy `bin\server\ChayGameServer.bat` — nó tự đổi `CoreServer.dll` → `.truoc`, đưa
   `CoreServer.dll.moi` lên thay, rồi mở server.
3. Vào Viêm Đế, giết vài đợt quái, rồi **đánh con còn lại đứng tại chỗ**.
4. Báo tôi: **còn cảnh "đánh không trúng" nữa không**.
5. Nếu muốn số liệu chắc chắn: gửi tôi `bin\server\jx_auto_server.log` (và `.log.1` nếu
   đã xoay vòng) — tôi đếm `[REFOAN]`, phải về gần 0.

Muốn quay lại bản cũ: tắt server, đổi tên
`CoreServer.dll.cu_3108_truoc_decref_74b55a4f` thành `CoreServer.dll`.
