# ĐIỀU TRA — "DI CHUYỂN CHỖ ĐÔNG NGƯỜI BỊ KHỰNG / ĐƠ 1 NHỊP"

Ngày: 20/08/2026 · Trạng thái: **ĐÃ ĐIỀU TRA + PHẢN BIỆN — CHƯA SỬA GÌ**
Phạm vi: client (Core/CoreClient.dll). Không đụng server, không đụng giao thức.

---

## 0. ⚠️ ĐÍNH CHÍNH SAU KHI HỎI LẠI CHỦ GAME — ĐÂY LÀ **HAI LỖI TÁCH BIỆT**

Bản nháp đầu của tài liệu này gộp mọi thứ vào một nguyên nhân. **Sai.** Sau khi chủ
game đính chính và làm thêm hai phép thử, đã tách được rõ:

| Vế | Triệu chứng | Nguyên nhân | Trạng thái |
|---|---|---|---|
| **A** | **Cả màn hình dừng 1 nhịp** khi di chuyển tới gần một người — mình, người khác, mọi thứ đứng cùng lúc, "kiểu dừng khung hình vẽ nên nhìn không mượt" | **stall khâu VẼ** (một pass `UiPaint` bị treo) | 🔍 **đang điều tra riêng** — xem mục 11 |
| **B** | **Đứng yên** vẫn thấy người xung quanh đi khựng như robot; **khi đông thì cả người chơi thật lẫn bot chạy ngang qua đều khựng** | logic va chạm client (mục 2-3 bên dưới) | ✅ **đã chốt** |

### Hai phép thử của chủ game và kết quả

**Phép thử 1 — đứng yên nhìn người khác:** *"khi tôi đứng yên cũng nhìn thấy người xung
quanh đi khựng lại như robot"*
⇒ **DƯƠNG TÍNH.** Vế B tồn tại độc lập, không liên quan tới việc mình có di chuyển hay
không. Loại bỏ khả năng "camera của mình khựng theo nên tưởng người khác khựng".

**Phép thử 2 — người thật hay chỉ bot:** *"khi đông thì cả người chơi và bot chạy ngang
qua đều khựng"*
⇒ **B1 xác nhận, B2 bác bỏ.** Nếu nguyên nhân là bot phát tràn gói `do_run`
(kết luận của `b91831f5`) thì **chỉ bot mới khựng**, người chơi thật phải mượt — vì
người thật không sinh ra dòng gói đó. Người thật cũng khựng ⇒ thủ phạm nằm ở **chính
đoạn mã client tự mô phỏng chuyển động của mọi NPC**, tức chuỗi `GetDir` → `DoStand()`
→ `SyncNpcMin` mô tả ở mục 3. Gói bot tràn (nếu chưa restart) chỉ **cộng hưởng làm nặng
thêm**, không phải gốc.

Chữ **"như robot"** cũng khớp: nhịp đi-đứng-đi-đứng **đều đặn** là dấu hiệu của bộ đếm
`m_nFindTimes` luân phiên (mục 3, mắt xích 2), chứ gói tràn thì nhịp sẽ lộn xộn.

> **Toàn bộ mục 1-10 dưới đây nói về vế B.** Vế A xem mục 11.

---

## 1. Triệu chứng — vế B

> "Đứng yên cũng nhìn thấy người xung quanh đi khựng lại như robot."
> "Khi đông thì cả người chơi và bot chạy ngang qua đều khựng."

**Điểm mấu chốt để loại trừ:** FPS **không tụt**. Chủ game xác nhận nhiều lần (có ảnh
chụp HUD 60 vẽ / 18 logic). Với vế B, điều đó loại bỏ nhóm nguyên nhân vẽ/CPU — nếu là
mấy thứ đó thì **mọi thứ** phải đứng cùng lúc (kể cả UI, hiệu ứng, chữ chạy), chứ không
phải riêng **vị trí nhân vật** đứng trong khi khung hình vẫn chạy đều.

*(Lưu ý: vế A thì ngược lại — mọi thứ đứng cùng lúc thật. Đó là lý do phải tách hai vế.)*

---

## 2. KẾT LUẬN

**Đây không phải lỗi hiệu năng. Đây là luật va chạm "NPC là tường" của engine gốc,
cộng với việc bộ tìm đường chỉ cho né đúng một lần rồi bỏ cuộc.**

Chuỗi nhân quả đã kiểm chứng đủ 5 mắt xích trên mã nguồn hiện tại (mục 3).
Tóm tắt: ở client, **mỗi ô lưới có người đứng là một bức tường cứng, vô điều kiện**.
Khi đường đi bị chắn hai nhịp liên tiếp, bộ tìm đường trả về 0 → `ServeMove` gọi
`DoStand()` → **`DoStand()` xoá luôn điểm đến** → nhân vật đứng im cho tới khi có
lệnh di chuyển mới. Với nhân vật của mình, lệnh mới bị **hãm 5 nhịp** mới được phát
lại ⇒ **đứng im tới ~278 ms**. Đúng bằng "đơ 1 nhịp rồi mới bình thường".

Vế (b) là **cùng một đoạn mã**: client tự mô phỏng (dead-reckon) người khác bằng
chính `ServeMove` đó, nên mỗi người xung quanh cũng độc lập bị `DoStand()`. Tệ hơn,
gói đồng bộ `SyncNpcMin` **vứt bỏ toạ độ** khi người đó vẫn còn trong cùng region,
nên không có gì kéo họ về đúng chỗ — họ đứng im tới khi có lệnh mới hoặc qua ranh
region thì **nhảy một phát**.

### 🔴 Phát hiện quan trọng nhất

**Bản vá cho đúng vế (b) ĐÃ TỪNG ĐƯỢC VIẾT VÀ ĐÃ BỊ REVERT — mà lý do revert hoá ra
là oan.**

```
c8981653  Viec 1: port "co xin nan vi tri" cua ban goc - het canh nguoi khac dung khung roi nhay
eba30b04  Revert "Viec 1: port ..."          <-- revert trần, KHÔNG ghi lý do
54779c0c  Va 2 loi CHINH TOI gay ra lam crash o map dong nguoi
```

Ba commit này **liền nhau**. Đọc `54779c0c` thì hai lỗi crash thật sự là:
1. `Missle[MAX_MISSLE]` — ghi ra ngoài mảng (do commit `06a53862`, nội suy ám khí);
2. `KImageStore2` — port nửa cơ chế đuổi ảnh, thiếu ngân sách (do commit `cdecf04d`).

Và `ab7b5b4c` sau đó còn tìm ra gốc nữa: `Represent2` gọi `free()` lên bộ nhớ do
`engine.dll` cấp phát.

⇒ **Không cái nào liên quan tới `c8981653`.** Bản vá đó bị revert trong lúc truy crash
rồi **không bao giờ được áp lại**. Kiểm chứng: `grep -rn "m_nNeedFixPos" Sources/`
hiện trả về **rỗng** — cơ chế "cờ xin nắn vị trí" không còn trong cây nguồn.

Và ngay trong commit `c8981653`, tác giả **cố ý giữ nguyên `DoStand()` cho nhân vật
của mình** ("nhân vật CỦA MÌNH vẫn DoStand thật, nếu không sẽ đi xuyên người rồi bị
server kéo lùi") — nghĩa là **vế (a) chưa từng được sửa**, kể cả khi bản vá đó còn sống.

---

## 3. Chuỗi nhân quả — 5 mắt xích, có mã nguồn

### Mắt xích 1 — Client coi người khác là tường **vô điều kiện**

`Sources\Core\Src\KRegion.cpp:1016-1024` (nhánh `#else` = client):

```cpp
#else
    _ASSERT(0 <= nGridX && nGridX < REGION_GRID_WIDTH && ...);
    if (bCheckNpc && m_pNpcRef)
    {
        if (m_pNpcRef[nGridY * m_nWidth + nGridX] > 0)
            return Obstacle_JumpFly;
    }
    return Obstacle_NULL;
#endif
```

So với nhánh **server** ngay phía trên (`:974-1014`): server kiểm địa hình trước, và
còn có tinh chỉnh **nửa ô chéo** (`Obstacle_LT/RT/LB/RB`) cho phép lách qua góc.
Client thì **chỉ cần ô có `m_pNpcRef > 0` là tường**, không nửa ô, không ngoại lệ.
`Obstacle_JumpFly = 4` (`Scene\ObstacleDef.h`) ≠ 0 ⇒ chặn thật.

Ô lưới rộng **32 MPS** (`defLOGIC_CELL_WIDTH`, `KSubWorld.cpp:55`). Tốc chạy đo được
**18-20 MPS/nhịp** (số đo thật từ `bot.log`, ghi trong `b91831f5`) ⇒ **cứ ~1,6-1,8
nhịp là vượt một ranh ô**, tức gần như **mỗi nhịp đều có một lượt kiểm "có ai đứng
chắn không"**. Chỗ đông người thì xác suất trúng rất cao.

> Lưu ý: `TestBarrierMin` (`KSubWorld.cpp:1613-1625`) chỉ kiểm NPC khi **đổi sang ô
> khác** — đi trong cùng ô thì bỏ qua. Nên chính mình không tự chắn mình.

### Mắt xích 2 — Bộ tìm đường chỉ được né **đúng một lần** rồi bỏ cuộc

`Sources\Core\Src\KNpcFindPath.cpp`, hàm `GetDir` (`:49`). Có **ba** cửa trả về 0:

**Cửa A** (`:100-104`) — đích đến bị chắn và mình đã tới gần:
```cpp
if (nCheckBarrier != 0 && !CheckDistance(nXpos >> 10, nYpos >> 10, nDestX, nDestY,
                                         defFIND_PATH_STOP_DISTANCE))
{
    m_nFindTimes = 0;
    return 0;
}
```
`defFIND_PATH_STOP_DISTANCE = 64` MPS = **2 ô**. `CheckDistance` trả TRUE khi
khoảng cách ≥ ngưỡng, nên `!CheckDistance` = **đang ở trong vòng 2 ô**.
⇒ **Bấm chuột vào chỗ có người đứng, tới cách 2 ô là dừng cứng, mỗi nhịp một lần.**
Đây chính là chữ "**di chuyển TỚI NGƯỜI** bị đơ" trong lời chủ game.

**Cửa B** (`:106-110`) — bộ đếm né:
```cpp
m_nFindTimes++;
if (m_nFindTimes > 1)
{
    m_nFindTimes = 0;
    return 0;
}
```
Nhịp bị chắn thứ nhất: `m_nFindTimes` 0→1, chưa >1, được thử né.
Nhịp bị chắn thứ hai: 1→2, **>1 ⇒ trả 0 ⇒ đứng lại**.
Và các dòng reset `m_nFindTimes = 0` ở nhánh né thành công **đều bị comment sẵn từ
bản gốc** (`:150`, `:194`, `:219`, `:224`) ⇒ bộ đếm **không** được xoá khi né thành
công. Kết quả là **nhịp chẵn/nhịp lẻ luân phiên: đi được một nhịp, đứng một nhịp** —
đúng nhịp "stop-go" mà mắt nhìn ra là khựng.

**Cửa C** (`:192-196` và `:217-221`) — men tường thất bại: hết 4 hướng thử thì
`m_nFindState = 0; m_nFindTimer = 0; return 0;`. Trong đám đông đặc, sau khi lách
qua người thứ nhất là đụng ngay người thứ hai ⇒ rơi vào đây rất nhanh.
(`MAX_FIND_TIMER = 30` nhịp ≈ 1,67 s là trần men tường, hiếm khi chạm tới.)

### Mắt xích 3 — `ServeMove` biến "không tìm được hướng" thành **đứng lại**

`Sources\Core\Src\KNpc.cpp:4398-4408` (nhánh `#ifndef _SERVER`):

```cpp
int nRet = m_PathFinder.GetDir(x, y, m_Dir, m_DesX, m_DesY, MoveSpeed, &m_Dir);
#ifndef _SERVER
    if (nRet == 1) { ... }
    else if (nRet == 0)
    {
        DoStand();      // KNpc.cpp:4407  <-- áp dụng cho MỌI NPC, kể cả người khác
        return;
    }
```

**Không phân biệt nhân vật mình hay người khác.** Đây đúng là chỗ mà `c8981653` đã
sửa và đã bị revert.

### Mắt xích 4 — `DoStand()` **xoá luôn điểm đến**

`Sources\Core\Src\KNpc.cpp:1991-2020`:

```cpp
void KNpc::DoStand()
{
    m_Frames.nTotalFrame = m_StandFrame;
    if (m_Doing == do_stand) return;
    FixPos();
    m_Doing = do_stand;
    m_Frames.nCurrentFrame = 0;
    GetMpsPos(&m_DesX, &m_DesY);     // <-- KNpc.cpp:2003: điểm đến := vị trí hiện tại
    ...
    m_ClientDoing = cdo_stand;        // hoạt ảnh nhảy về tư thế đứng
}
```

Hai hậu quả:
1. **Lệnh di chuyển bị xoá sạch.** Không có gì tự đi tiếp — phải có lệnh MỚI.
2. **Hoạt ảnh reset về đứng** (`nCurrentFrame = 0`, `cdo_stand`). Đây là phần *mắt
   nhìn thấy*: không chỉ dừng, mà còn đổi tư thế chạy → đứng → chạy.

### Mắt xích 5a — Với **nhân vật của mình**: lệnh mới bị hãm **5 nhịp**

`Sources\Core\Src\GameDataDef.h:27`:
```cpp
#define  defMAX_PLAYER_SEND_MOVE_FRAME   5
```

`Sources\Core\Src\KPlayer.cpp:432` — `m_nSendMoveFrames++` đúng **một lần mỗi nhịp**.
`KPlayer.cpp:810`, `CoreShell.cpp:12288` (`GotoWhere`), `CoreShell.cpp:13842` — mọi
đường phát lệnh di chuyển đều bị chặn bởi:
```cpp
if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
```

⇒ Sau khi `DoStand()` xoá điểm đến, dù đang **giữ chuột**, lệnh mới cũng phải chờ tới
**5 nhịp = 5/18 giây ≈ 278 ms**.

> **278 ms chính là "đơ 1 nhịp"**. Đủ dài để mắt thấy rõ, đủ ngắn để "rồi bình thường".
> Và vì đây thuần là hãm lệnh chứ không phải tải máy, **FPS không hề tụt** — khớp
> chính xác với mô tả của chủ game.

### Mắt xích 5b — Với **người xung quanh**: không có gì nắn họ về chỗ đúng

`Sources\Core\Src\KProtocolProcess.cpp:1880-1993`, hàm `SyncNpcMin`.
Gói `NPC_NORMAL_SYNC` **có mang** `MapX / MapY / m_fkOffX / m_fkOffY`. Nhưng toàn bộ
khối ghi toạ độ nằm trong điều kiện:

```cpp
if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
{
    ... RemoveNpc / DecRef ...
    Npc[nIdx].m_MapX = nMapX;        // KProtocolProcess.cpp:1946-1949
    Npc[nIdx].m_MapY = nMapY;
    Npc[nIdx].m_OffX = NpcSync->m_fkOffX;
    Npc[nIdx].m_OffY = NpcSync->m_fkOffY;
    ...
}
```

⇒ **Còn cùng region thì toạ độ trong gói bị vứt đi hoàn toàn.** Chỉ máu/mana/tốc
độ/trạng thái được cập nhật. Người bị kẹt client-side sẽ đứng im **cho tới khi**:
có lệnh `s2c_npcwalk`/`npcrun` mới, hoặc bị đánh, hoặc **qua ranh region — lúc đó
nhảy một phát**.

Đây đúng nguyên văn chuỗi mà `c8981653` mô tả và đã vá, rồi bị revert.

---

## 4. Vì sao BÂY GIỜ mới thấy rõ (mà trước đây thì không)

Cơ chế này là **của engine gốc, không phải regression**. Kiểm chứng:

```
git log -1 --format="%h %s" -- Sources/Core/Src/KNpcFindPath.cpp
→ 9bc7936a  Snapshot nguon JX1 VS2022 truoc khi dung lai he bang hoi JX2
```
`KNpcFindPath.cpp` **chưa từng bị sửa** trong dự án này — chỉ có commit snapshot ban
đầu. `KRegion.cpp` cũng chỉ bị đụng bởi hai bản vá **server** (`042ee210` treo
`BroadCast`, `2d6d3179` trần phát đồng bộ), không đụng nhánh client.

Vậy tại sao giờ mới nổi? Ba yếu tố cộng dồn, đều mới có gần đây:

1. **Mật độ tăng vọt.** Hệ bot 1000 con (từ `a829482b` trở đi) làm các map thường
   xuyên đông đặc. Trước đây map vắng thì hiếm khi chạm luật NPC-là-tường.
2. **Nội suy làm cú dừng lộ hẳn ra.** Trước bản `1cd922c7`, cả game giật nấc 18 hình/
   giây nên một nhịp đứng bị **nguỵ trang** trong cái giật chung. Bây giờ chuyển động
   mượt 30-60 hình/giây, nên một cú dừng cứng 278 ms **nổi bật gấp bội**. Đây là câu
   trả lời cho "trước đây bật nội suy xong mượt hơn nhưng vẫn thấy khựng".
3. **Bản vá cho vế (b) đã bị revert** (mục 2) và chưa được áp lại.

---

## 5. Đã BÁC BỎ những gì (ghi lại để phiên sau khỏi điều tra lại)

| Nghi can | Vì sao bị loại |
|---|---|
| Tụt FPS / quá tải CPU khi vẽ đông người | Chủ game xác nhận **FPS không giảm**. Đo thật từ `bot.log` trong `b91831f5`: nhịp bot 650-880 ms/10 s, engine 1200 ms/10 s ⇒ ~11 ms trong ngân sách 55 ms mỗi khung. Server **không** quá tải. |
| Nạp tài nguyên đồng bộ (sprite/trang bị người mới lọt vào tầm nhìn) | Nếu là nó thì **cả khung hình** phải đứng (UI, hiệu ứng, chữ cũng đứng) và FPS phải tụt. Đây chỉ vị trí nhân vật đứng. Ngoài ra `b5092ade` (trải `PrerenderGround` 1 region/khung) và `402bbead` (probe đợt 2) đã **minh oan** cho nhánh nạp art: log cho thấy cross region hầu như không spike. |
| Lớp PaintFps / nội suy tự gây ra | `KNpcFindPath.cpp` chưa từng bị sửa; `ServeMove`/`DoStand` không nằm trong diff PaintFps. Lớp nội suy chỉ **làm lộ** cú dừng chứ không tạo ra nó. (Xem mục 7 để tự kiểm bằng `PaintFps=0`.) |
| `KScenePlaceC::MoveObject` churn đèn theo nhịp vẽ | Đã được xử lý: `POSSHIFT` giờ dùng `SetDrawPos` cho NPC thường, **không** đụng cây cảnh (`CoreShell.cpp:11774-11781`, có chú thích rõ). Chỉ nhân vật chính mới gọi `SetPos`. |
| Crash / hỏng bộ nhớ ở map đông | Đã vá tận gốc ở `54779c0c` (`Missle[500]` ghi ngoài mảng + `KImageStore2` thiếu ngân sách) và `ab7b5b4c` (`Represent2` `free()` chéo heap). Là **crash**, không phải khựng. |
| Bot phát tràn gói `do_run` (kết luận của `b91831f5`) là **gốc** của vế B | **ĐÃ BÁC BỎ bằng phép thử thực địa** (mục 0, phép thử 2): chủ game xác nhận **cả người chơi thật lẫn bot chạy ngang qua đều khựng**. Người thật không sinh ra dòng gói `do_run` dày đó ⇒ gốc phải nằm ở đoạn mã client dùng chung cho mọi NPC. Cơ chế vẫn có thật (`NetCommandRun`, `KProtocolProcess.cpp:633` → `SendCommand` → nhịp sau `NewPath` đổi `m_DesX/m_DesY` ⇒ `GetDir` reset sạch trạng thái tìm đường), nhưng chỉ là **yếu tố cộng hưởng** làm bot khựng nặng hơn người thật. ⚠️ Vẫn nên xác nhận `CoreServer.dll` bản waypoint đã restart chưa — theo ghi chép là **chưa**. |

---

## 6. Bảng xếp hạng nghi can còn sống

| # | Nghi can | Giải thích vế nào | Bằng chứng | Độ tin cậy |
|---|---|---|---|---|
| 1 | `GetDir` cửa A: đích bị người chiếm + trong 2 ô ⇒ dừng cứng mỗi nhịp | **(a)** — đúng chữ "di chuyển TỚI NGƯỜI" | `KNpcFindPath.cpp:100-104` | **Cao** |
| 2 | `GetDir` cửa B: `m_nFindTimes > 1` ⇒ luân phiên đi-đứng | **(a)** và **(b)** | `KNpcFindPath.cpp:106-110`, reset bị comment `:150/:194/:219/:224` | **Cao** |
| 3 | `DoStand()` xoá điểm đến + hãm phát lệnh 5 nhịp (278 ms) | **(a)** — đúng độ dài "1 nhịp" | `KNpc.cpp:2003`, `GameDataDef.h:27`, `KPlayer.cpp:432` | **Cao** |
| 4 | `SyncNpcMin` vứt toạ độ khi cùng region ⇒ người khác kẹt tới lúc nhảy | **(b)** | `KProtocolProcess.cpp:1941-1957` | **Cao** |
| 5 | Client coi NPC là tường vô điều kiện (không có nửa ô chéo như server) | nền của cả (a) và (b) | `KRegion.cpp:1016-1024` vs `:974-1014` | **Cao** |
| 6 | Bot phát tràn `do_run` làm client đặt lại đường đi người khác | **(b)**, cộng hưởng | `b91831f5` | Vừa (cần xác nhận đã restart) |

---

## 7. Cách ĐO để chốt (chủ game tự làm được, không cần build lại)

Ba phép thử, làm theo thứ tự, mỗi phép loại được một nhóm nghi can:

**Phép 1 — loại nhóm "vẽ/hiệu năng":** mở `config.ini` cạnh `Game.exe`, mục `[Client]`
đặt `PaintFps=0` (trả về hành vi cũ 100%, tắt hẳn nội suy). Vào chỗ đông, thử lại.
- Vẫn khựng ⇒ **đúng như kết luận mục 2**, thủ phạm là logic va chạm, không phải lớp vẽ.
- Hết khựng ⇒ kết luận sai, phải quay lại soi lớp nội suy. *(Dự đoán: vẫn khựng, chỉ
  là khó nhận ra hơn vì cả game giật đều nên nguỵ trang mất.)*

**Phép 2 — xác nhận không phải stall khung hình:** đặt `PaintFps=60` và `PaintLog=1`,
chạy vào chỗ đông rồi ra chỗ vắng. File `jx_paint.log` sinh cạnh `Game.exe`
(`S3Client.cpp:1366+`, gộp mỗi 10 giây; ghi dòng riêng khi một pass ≥ 25 ms).
- Nếu lúc khựng mà log **không** có spike ⇒ khung hình vẫn chạy đều, chỉ nhân vật
  đứng ⇒ khẳng định là logic di chuyển. **Đây là số liệu quyết định.**
- Nếu có spike 25-50 ms dày đặc ⇒ có thêm một nguyên nhân vẽ chồng lên, phải soi riêng.

**Phép 3 — tách vế (a) khỏi vế (b):** đứng **yên hoàn toàn** giữa đám đông và chỉ
nhìn người khác đi qua.
- Người khác vẫn khựng-rồi-nhảy ⇒ vế (b) có thật, độc lập với việc mình di chuyển
  ⇒ đúng nghi can #4 (`SyncNpcMin`) — tức là **bản vá `c8981653` bị revert oan cần
  được áp lại**.
- Người khác chạy mượt ⇒ vế (b) chỉ là ảo giác do camera của mình khựng theo.

**Bổ sung — xác nhận trạng thái triển khai:** kiểm xem `CoreServer.dll` bản waypoint
(`b91831f5`, md5 `40f3c85783794206eebb9786d0cf495a`) đã **restart** chưa. Theo ghi chép
lúc commit là **chưa**. Nếu chưa restart thì một phần vế (b) vẫn là do bot phát tràn
gói, chưa liên quan tới `SyncNpcMin`.

---

## 8. Hướng sửa ĐỀ XUẤT (mô tả thôi — **CHƯA LÀM**)

Xếp theo tỷ lệ lợi/rủi ro. Tất cả đều **thuần client**, không đụng giao thức, không
đụng gameplay.

| Ưu tiên | Việc | Chữa vế | Rủi ro | Khối lượng |
|---|---|---|---|---|
| **1** | **Áp lại `c8981653`** (`git revert eba30b04` hoặc cherry-pick). Cơ chế "cờ xin nắn vị trí" của bản gốc JX2: người khác bị chắn thì **giương cờ `m_nNeedFixPos` và giữ nguyên trạng thái chạy** thay vì `DoStand()`; `SyncNpcMin` thấy cờ thì ghi đè toạ độ thật từ gói (các trường **đã có sẵn** trong `NPC_NORMAL_SYNC`) rồi hạ cờ. | (b) | **Thấp** — đã viết xong, đã build PASS, và lý do revert đã chứng minh là oan. Vẫn nên test lại map đông vì hai lỗi crash cùng thời kỳ đã được vá riêng. | Rất nhỏ (3 file, 38 dòng) |
| **2** | **Nới hãm phát lệnh khi đang bị chắn.** Khi `GetDir` trả 0 cho nhân vật mình, cho phép phát lại lệnh sớm hơn 5 nhịp (ví dụ hạ về 1-2 nhịp **chỉ trong trường hợp này**). Giảm cú đứng từ 278 ms xuống ~55-110 ms. | (a) | **Vừa** — `defMAX_PLAYER_SEND_MOVE_FRAME` sinh ra để **giảm tải mạng**; nới bừa toàn cục là tăng lưu lượng ×5. Phải nới **có điều kiện**, và cân nhắc chỗ đông người chính là chỗ băng thông đã căng nhất. | Nhỏ |
| **3** | **Đừng xoá điểm đến trong `DoStand()` khi nguyên nhân là bị chắn tạm thời.** Giữ `m_DesX/m_DesY`, chỉ đổi hoạt ảnh; nhịp sau `GetDir` tự thử lại. | (a)+(b) | **Vừa-cao** — `DoStand()` được gọi từ **~25 chỗ** trong `KNpc.cpp` với ngữ nghĩa rất khác nhau (chết, bị choáng, tới đích, hết đường…). Đụng vào hàm dùng chung này dễ vỡ chỗ khác. Nếu làm thì phải tách một hàm riêng cho nhánh "bị chắn". | Vừa |
| **4** | **Cho client nới luật NPC-là-tường bằng tinh chỉnh nửa ô chéo giống server** (`KRegion.cpp` nhánh `#else` học theo nhánh `#ifdef _SERVER`), để lách qua góc được như server. | nền của (a)+(b) | **Cao** — client và server sẽ **lệch luật va chạm**. Client cho đi, server chặn ⇒ server kéo lùi ⇒ đổi khựng thành giật lùi, tệ hơn. **Không khuyến nghị** trừ khi sửa đồng bộ cả hai phía. | Lớn |
| **5** | **Tăng số lần né** (`m_nFindTimes > 1` → `> 2` hoặc `> 3`), hoặc bỏ comment các dòng reset `m_nFindTimes = 0` ở nhánh né thành công. | (a)+(b) | **Vừa** — cùng đoạn mã chạy **cả trên server** (`#ifdef _SERVER` dùng chung `GetDir`). Sửa là đổi hành vi di chuyển của **toàn bộ NPC và 1000 bot**. Nếu làm phải bọc `#ifndef _SERVER`. Nhưng đây là **cách rẻ nhất** để giảm hẳn tần suất dừng. | Nhỏ (nhưng phải test rộng) |

**Gợi ý thứ tự thực thi:** làm #1 trước (rẻ, an toàn, chữa đúng vế người ta thấy rõ
nhất), đo lại; nếu vế (a) vẫn phiền thì làm #5 có bọc `#ifndef _SERVER`, đo lại; #2
chỉ làm khi hai cái trên chưa đủ.

---

## 9. Câu hỏi còn bỏ ngỏ / giả định chưa kiểm chứng

1. ~~Đọc lại câu chữ chủ game~~ — **ĐÃ GIẢI QUYẾT**, xem mục 0. Hoá ra là hai lỗi
   tách biệt: vế A là stall khâu vẽ, vế B là logic va chạm. Cả hai đều có thật.
2. **Bản vá waypoint `b91831f5` đã restart chưa?** Chưa xác nhận. Không còn là gốc của
   vế B (đã bác bỏ ở mục 5) nhưng vẫn cộng hưởng làm bot khựng nặng hơn người thật.
3. **`PAINT_INTERP_SNAP_DIST = 64`** (`CoreShell.cpp:2505`) chú thích là "pixels per
   tick" nhưng giá trị so sánh là kết quả `Map2Mps`, tức **đơn vị MPS**. Chưa kiểm
   MPS có bằng pixel màn hình không (trục Y đồ hoạ đẳng cự thường bị chia đôi). Nếu
   lẫn đơn vị thì ngưỡng snap sai — **không phải nguyên nhân của lỗi này**, nhưng là
   một nợ kỹ thuật nên soi riêng.
4. **`m_pNpcRef` được cộng/trừ ở đâu và có bị rò không.** Nếu có ô nào `AddRef` mà
   quên `DecRef` (ví dụ khi người chơi ra khỏi tầm nhìn đột ngột) thì sẽ có **ô tường
   ma** — không ai đứng mà vẫn chặn. Đám đông là nơi dễ lộ nhất. Chưa kiểm.
5. **Chưa đo thực địa.** Toàn bộ báo cáo này dựa trên đọc mã nguồn. Ba phép thử ở
   mục 7 là để biến suy luận thành số đo.

---

## 10. Tóm tắt vế B cho chủ game

> Ở phía client, **mỗi người đứng là một bức tường cứng** (server thì có tinh chỉnh nửa
> ô cho lách góc, client thì không), và bộ tìm đường **chỉ cho né đúng một lần** rồi
> đứng lại. `DoStand()` còn **xoá luôn điểm đến**, mà không có gì nắn người đó về chỗ
> đúng vì `SyncNpcMin` vứt toạ độ khi còn cùng region. Kết quả là ai cũng đi-đứng-đi-
> đứng như robot khi chỗ đông — **người thật và bot như nhau**, đúng như anh quan sát.
> **Bản vá cho phần này đã viết xong từ trước (`c8981653`), bị revert nhầm trong lúc
> truy một lỗi crash hoá ra không liên quan, và chưa được áp lại.**

---

## 11. VẾ A — "dừng khung hình vẽ" (điều tra riêng, đang chạy)

Đây là lỗi **khác hẳn** vế B và phải sửa riêng.

**Triệu chứng:** di chuyển tới gần một người thì **cả màn hình dừng đúng một nhịp** rồi
bình thường trở lại. Không phải riêng vị trí nhân vật đứng — mà là **một pass vẽ bị treo**.

**Manh mối đã có sẵn trong lịch sử dự án** (commit `402bbead`):

> "log đợt 1 **minh oan** nạp region: cross hầu như không spike, **thủ phạm là paint
> 25-48 ms, logic = 0**"

⇒ Đã đo được: cú treo nằm **trong `UiPaint`**, không phải trong tick logic, không phải
do nạp region qua biên. Probe `[PDET]` được thêm để tách tiếp "nạp art giữa khung vẽ"
vs "Blt/DWM chặn" — nhưng **kết quả đợt 2 không thấy ghi lại ở đâu**.

**Công cụ đo đã có sẵn, chưa cần build lại:**
- `[Client] PaintLog=1` trong `config.ini` ⇒ sinh `jx_paint.log` cạnh `Game.exe`.
- `S3Client.cpp:1236-1400` — đo tổng pass, tách logic vs vẽ, ghi khi pass ≥ 25 ms.
- `Sources\S3Client\Ui\UiShell.cpp:280-358` — probe `[PDET]` chia `UiPaint` làm 3 khúc:
  ```
  [PDET] begin=<RepresentBegin, khoá surface>  render=<toàn cảnh+UI, art nạp ở đây>  end=<RepresentEnd, blt/flip>
  ```
- HUD trong game (`7359f1b8`): FPS/CPU/GPU/RAM + **"lâu nhất \<x.y\> ms"** (khung lâu
  nhất trong 1 giây qua) — chính là con số làm lộ cú giật mà FPS trung bình giấu đi.

**Nghi can số 1 (giả thuyết đang kiểm):** **nạp sprite đồng bộ ngay giữa khung vẽ.**
Khi một người mới lọt vào tầm nhìn, client phải nạp bộ ảnh của họ (thân + từng món
trang bị + vũ khí + tóc + ngựa — `MAX_NPC_IMAGE_NUM = 1 + MAX_PART + 6 + 1 + 1`) từ
`.pak`, giải nén, tạo surface. Nếu việc đó chạy **đồng bộ giữa `Wnd_RenderWindows`**
thì một khung vẽ kéo dài hàng chục ms ⇒ cả màn hình dừng ⇒ và vì sau đó đã có cache
nên **chỉ treo đúng một lần** — khớp chính xác dấu hiệu "tới gần người thì đơ 1 nhịp
rồi thôi". Nặng hơn nữa nếu nó chạy **trong lúc surface đang bị `RepresentBegin` khoá**.

Đối chiếu: `b5092ade` đã trải `PrerenderGround` ra 1 region/khung vẽ để hết cú khung
20-40 ms sau nạp region — tức **nền đất đã được trải**, nhưng **nhân vật/trang bị thì
chưa rõ đã trải chưa**. Đó có thể là lỗ hổng còn lại.

**Các nghi can khác đang được quét:** kho ảnh `KImageStore2` (trần `KIS_HARD_IMAGE_CEIL`
2048, cơ chế đuổi ảnh, nguy cơ thrash nạp-đuổi-nạp lại); tạo/khôi phục surface
DirectDraw giữa khung; cây cảnh suy biến khi nhiều đối tượng cùng toạ độ; luồng nền nạp
cảnh giữ critical section chặn luồng vẽ; cache glyph khi vẽ tên người mới.

**Đã sửa rồi ở các phiên trước (đánh giá xem đã đủ chưa là một phần của điều tra này):**
`ce8c4d49` (ánh sáng động vô ích, vẽ nền đất không cắt cạnh, cờ Preprocess) ·
`b5092ade` (trải PrerenderGround) · `1d16b8ed` (bỏ `SetPos`→`MoveObject` mỗi khung vẽ,
nhịp khung đều `PAINT_LEAD_MS`, tràn stack `szPackName` — **mục này đòi build lại
`engine.dll` mới hết**) · `54779c0c` · `ab7b5b4c`.

⚠️ **Cảnh báo khi đo:** chính probe gọi `fopen("jx_paint.log","a")` + `fclose` **mỗi lần
spike**. Bật `PaintLog=1` là tự thêm chi phí ghi đĩa vào đúng lúc đang giật. Đọc số phải
trừ hao phần đó, và **tắt lại sau khi đo xong**.

---

# 12. VẾ A — KẾT QUẢ ĐIỀU TRA (workflow 6 lăng kính, 55 giả thuyết) — **ĐÃ CHỐT THỦ PHẠM**

> Mục 11 là giả thuyết ban đầu. Mục 12 này là **kết quả đo thật**, thay thế mục 11.
> Vẫn **CHƯA SỬA GÌ**.

## 12.1 KẾT LUẬN

**Thủ phạm số 1: nạp tệp SPR nguyên khối + giải nén UCL chạy ĐỒNG BỘ ngay giữa vòng vẽ.**

Có agent đã **đo thật**: biên dịch nguyên bản `ucl_nrv2b_decompress_8` từ
`Sources/Engine/Src/ucl/n2b_d.c` + `getbit.h` bằng MSVC, chạy trên **khối nén thật lấy
từ pak của game** (`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\data`), kiểm
chứng cỡ bung ra khớp từng byte với bảng chỉ mục pak.

**Con số đo được:**

| Kịch bản | Kết quả |
|---|---|
| MỘT người mới lọt vào tầm nhìn (bộ MainMan, tư thế đứng) | 7 tệp `.spr`, 540 KB bung ra, 260 KB đọc đĩa |
| MỘT người có trang bị ngẫu nhiên + tư thế chạy | trung bình **9,4 tệp, 918 KB bung ra** |
| Thời gian 1 người | 3,7 ms (x64 `/O2`) — **5,3 ms (x86 `/Od`, đúng cấu hình `engine.dll` đang ship)** |
| **10 người cùng lọt vào tầm nhìn** | 94 tệp, 9,4 MB bung ra → **37,9 ms (x64) / 54,3 ms (x86)** cache nóng, +6,4 ms IO khi nguội |

**Trùng khít dải 25-48 ms mà `402bbead` đã đo với LOGIC=0.** Và vì sau đó đã vào cache
nên **chỉ treo đúng một lần cho mỗi người** — khớp 100% lời anh mô tả.

Băng thông UCL đo thật (best-of-20, cache nóng): 41 KB→0,163 ms · 206 KB→0,858 ms ·
443 KB→1,153 ms · 1,12 MB→4,872 ms · 2,44 MB→6,952 ms · 4,87 MB→18,584 ms ·
9,28 MB→31,922 ms · 22,06 MB→**79,248 ms**.
Lý do chậm: dùng biến thể `getbit_8` chậm nhất — lấy **từng BIT** qua bộ đệm 8-bit
(`getbit.h:35`), chỉ đạt 220-370 MB/s.

## 12.2 Chuỗi gọi đầy đủ (không đứt mắt xích nào)

```
UiPaint (UiShell.cpp:282)
 -> RepresentBegin
 -> Wnd_RenderWindows (Wnds.cpp:105)
 -> KUiGameSpace::PaintWindow (UiGame.cpp:182)
 -> KCoreShell::DrawGameSpace (CoreShell.cpp:12456)
 -> g_ScenePlace.Paint()
 -> m_ObjectsTree.Paint(IPOT_RL_OBJECT) (KScenePlaceC.cpp:1118)
 -> CoreDrawGameObj (KIpotLeaf.cpp:82)
 -> Npc[uId].Paint() (CoreDrawGameObj.cpp:57)
 -> m_DataRes.Draw(...) (KNpc.cpp:6973)
 -> KNpcRes::Draw (KNpcRes.cpp:180) — bắn 6 lô DrawPrimitives (:437,:462,:493,:524,:596,:625)
 -> KRepresentShell2::DrawPrimitives (KRepresentShell2.cpp:198-210) — GetImage cho TỪNG ảnh
 -> KImageStore2::GetImage (KImageStore2.cpp:272)
      ↳ TRƯỢT cache => gọi thẳng LoadImage (:334) NGAY TẠI CHỖ, đang giữ m_ImageProcessLock (:282)
 -> LoadImage (:801) -> SprGetHeader (:808) -> XPackFile::GetSprHeader (XPackFile.cpp:481)
 -> ReadElemFile (:334) = malloc(cỡ gốc) + ExtractRead (:242)
      = malloc(cỡ nén) + SetFilePointer + ReadFile (:188-196) + ucl_nrv2b_decompress_8 (:261)
```

**Bằng chứng client KHÔNG nạp lúc tạo NPC — chỉ ghi tên:** `KSprControl::SetSprFile`
(`KSprControl.cpp:40-56`) chỉ `strcpy(m_szName, ...)` + băm id. **Toàn bộ việc nạp bị
trì hoãn sang lần VẼ đầu tiên.**

Một người mới kéo theo 1 bóng + tới 20 phần thân + 20 phần hiệu ứng, mỗi phần là MỘT
tệp spr riêng theo (bộ phận, động tác, trang bị) — `KNpcResNode::GetFileName`
(`KNpcResNode.cpp:464`) ⇒ **cả chùm nạp trong đúng một lượt vẽ**.

## 12.3 BỘ KHUẾCH ĐẠI — pak không nén theo khung (phí 99,2%)

`XPackFile::GetSprHeader` có **hai nhánh** (`XPackFile.cpp:490-497`):
- Nhánh `TYPE_FRAME` (nén theo khung): chỉ đọc `SPRHEAD` + bảng khung, rồi lấy **từng
  khung theo nhu cầu** qua `GetSprFrame` (`:546`) / `bSingleFrameLoad`.
- Nhánh còn lại: đọc + giải nén **NGUYÊN TỆP**.

**Đếm cờ `TYPE_FRAME = 0x10000000` trong bảng chỉ mục pak đang chạy:**

| pak | tổng mục | nén-theo-khung |
|---|---|---|
| `spr.pak` | 14.616 | 43 (**0,29%**) |
| `update01.pak` | 17.609 | 25 (**0,14%**) |
| `updatejx14.pak` | 20.245 | **0** |
| `updatejx15.pak` | 22.209 | **0** |
| `maps.pak` | 64.447 | **0** |

7/7 tệp thân người MainMan đo được đều có cờ frame = 0.

**Phí đo thật:**
```
MA_BD_001_ST01.spr   199.539 B  Frames=120 Dir=8  khung TB 1.648 B -> phí 99,2%
MA_BD_001_RN01.spr   142.843 B  Frames= 88 Dir=8  khung TB 1.606 B -> phí 98,9%
MA_HD_001_ST01.spr    21.062 B  Frames=120 Dir=8  khung TB   161 B -> phí 99,2%
20100607001_ST01.spr 203.589 B  Frames=120 Dir=8  khung TB 1.682 B -> phí 99,2%
```

⇒ Để vẽ **đúng 1 khung hình (~1,6 KB)**, client phải malloc ~200 KB, đọc ~130 KB từ đĩa
và giải nén ~200 KB.

🟢 **ĐÂY LÀ CHỖ SỬA RẺ NHẤT: engine ĐÃ CÓ SẴN đường nén-theo-khung. Chỉ cần ĐÓNG GÓI LẠI
PAK ở chế độ `TYPE_FRAME` — KHÔNG ĐỤNG MỘT DÒNG MÃ NÀO.** Ước tính: phần giải nén mỗi
người tụt từ ~918 KB xuống cỡ chục KB, tức từ **3,7-5,3 ms/người xuống dưới 0,1 ms/người**.

## 12.4 LỖ HỔNG CÒN LẠI — nhân vật không có ngân sách trải

Nền đất **CÓ** ngân sách (`b5092ade`, `KScenePlaceC.cpp:1243-1279`):
```cpp
int nFarBudget = 1; // far off-screen regions: at most one per paint frame
...
if (m_pInProcessAreaRegions[i]->PrerenderGround(false)) nFarBudget--;
...
if (nDeferred && bForce == false) m_bRenderGround = true;
```

Tài nguyên **nhân vật/trang bị KHÔNG có gì tương đương**. Quét `Budget|Pending|Deferred|
Async|PreLoad|Preload` trên `Represent/Represent2/*.cpp *.h`, `KNpcRes.cpp`,
`KNpcResList.cpp`, `XPackFile.cpp`, `KPakFile.cpp` ⇒ **0 kết quả**.

Cơ chế nạp trước duy nhất từng tồn tại **đã bị tắt có chủ ý** (`KScenePlaceC.cpp:818-823`):
```cpp
// TAT theo ban goc: client JX2 goc KHONG preload SPR tren luong nap canh
// Ham nay goi GetImageFrameParam -> doc + giai nen pak TU LUONG NEN, ma
// KImageStore2::GetImage giu m_ImageProcessLock => LUONG VE DUNG CHO.
//PreLoadProcess();
```

⇒ N người cùng lọt vào tầm nhìn ⇒ **N × ~918 KB giải nén dồn vào ĐÚNG một khung** thay
vì rải ra N khung. Trải ra: 10 người = 10 khung × +4-5 ms (không ai thấy). Dồn lại:
**1 khung +38-54 ms (thấy rõ)**.

## 12.5 🔴 MÙ ĐO — "paint 25-48ms" CHƯA chứng minh cú treo nằm trong `UiPaint`

`S3Client.cpp:1328-1363`: `nLogTick` chốt **ngay sau** khối logic; rồi `:1354` gọi
`POSSHIFT`; rồi `:1357` mới `UiPaint()`; cuối cùng `:1362` tính
`nLogPaint = timeGetTime() - nLogT0 - nLogTick`.

⇒ **Toàn bộ chi phí POSSHIFT bị TÍNH VÀO "paint="**, dù nó chạy **trước** `RepresentBegin`
và không hề nằm trong khâu vẽ thật. Ngược lại `[PDET]` (`UiShell.cpp:344-359`) chỉ ghi
khi tổng thời gian **bên trong** `UiPaint` ≥ 20 ms.

**PHÉP THỬ DỨT ĐIỂM:** nếu `jx_paint.log` có `[SPIKE] paint=25-48ms` mà **KHÔNG** có
`[PDET]` đi kèm cùng dấu thời gian ⇒ cú treo nằm ở **POSSHIFT** (mục 12.6), không nằm ở
`RepresentBegin`/`Wnd_RenderWindows`/`RepresentEnd`.

## 12.6 Các nghi can khác đã tìm được (đều CHƯA sửa)

| ID | Phát hiện | Bằng chứng | Chi phí |
|---|---|---|---|
| **L4-01** | 🔴 **Khung vẽ TỰ GỌI `WaitForSingleObject` chờ luồng nạp cảnh, timeout 30 GIÂY**, mỗi lần qua ranh region. `SPWP_TRIGGER_LOADING_RANGE=1` nên chỉ nhích qua MỘT ranh là kích hoạt. **Regression từ `9f759631`** (bản gốc chỉ gọi `Breathe` từ nhịp logic 18Hz) | `KScenePlaceC.cpp:1056-1061`, `CoreShell.cpp:11836-11841`, `KScenePlaceC.h:354,373` | cache ấm 1-5 ms; nguội/tranh khoá 20-200 ms; trần 30.000 ms. Mỗi 1,5-3 s lúc chạy |
| **L4-04** | 🔴 **Bản vá N2 của `ce8c4d49` ("vẽ nền đất không cắt cảnh") CHƯA HỀ ĐƯỢC ÁP VÀO MÃ.** Commit message ghi có, nhưng `git show` chỉ chứa việc dời khối `SetLoadingStatus`. Vòng vẽ **vẫn duyệt đủ 49 region mỗi khung** trong khi vùng nhìn chỉ ~2×2 region | `KScenePlaceC.cpp:1110-1115` (không có phép thử hình chữ nhật nào), `KScenePlaceRegionC.cpp:725-736, 416-418` | ~5-40 ms/khung trong ~9 khung sau mỗi lần nạp region |
| **L4-02 / L5-01** | 🔴 **`KNpc::PaintInfo` NẠP LẠI + PHÂN TÍCH LẠI nguyên tệp `.txt`/`.ini` cho TỪNG NPC, TỪNG KHUNG VẼ.** `KIniFile pIni; KTabFile pTab;` là biến cục bộ, `Load()` ở 3 chỗ. `KIniFile::Load` còn **giải mã AES toàn bộ** | `KNpc.cpp:5803-5804, 6045, 6072, 6193, 6201`; `KTabFile.cpp:48-72`; `KIniFile.cpp:121-189` | nguội 1-10 ms/lần; ấm ~0,1-0,4 ms × mỗi NPC × mỗi khung ⇒ **5-20 ms/khung liên tục** với 50-90 người có danh hiệu. `PlayerTitle.txt` = 18.266 B / 287 dòng |
| **L5-05** | `CoreDrawGameObj` truyền `bSelect = TRUE` (mã gốc là `false`, còn nằm trong chú thích cùng dòng) ⇒ **mọi con quái cũng bị vẽ tên + thanh máu + sprite ngũ hành**. Vòng vẽ **không có trần số đối tượng** | `CoreDrawGameObj.cpp:37, 48-50`; `KNpc.cpp:6592-6594, 6310, 6357-6359` | ~20-50 µs/NPC ⇒ **4-10 ms MỖI KHUNG** với 200 đối tượng; riêng phần do `bSelect=true` chiếm 1/4-1/3 |
| **F2-SPR-RỜI** | `m_nPakFileMode = 0` ("ưu tiên đọc từ ổ đĩa") và **`g_SetPakFileMode` KHÔNG được gọi ở bất kỳ đâu**. Nếu tệp spr rời tồn tại thì nó thắng pak ⇒ `malloc(File.Size())` + `fread` **nguyên tệp** giữa lượt vẽ. Cây client thật có `Spr\` **887 MB**, `Spr\npcres` 683 MB/376 tệp (TB 1,8 MB/tệp) | `KPakFile.cpp:18, 60-84` | Đo thật: `rongxanh_wlk.spr` 15,8 MB → **11,65 ms** (đĩa lạnh) / 5,05 ms (ấm); `thiensu_wlk.spr` 11,4 MB → 8,09/3,26 ms. **Trên HDD: 100-300 ms** |
| **L4-06** | Luồng vẽ **bị chặn được** bởi luồng nạp cảnh ở HAI khoá (`m_ProcessCritical`, `m_RegionListAdjustCritical`), cả hai đều **sau `RepresentBegin`**. `SetRegionsToLoad` còn tự chặn mình bằng `Enter/LeaveCriticalSection(&m_LoadCritical)` rỗng | `KScenePlaceC.cpp:1634-1651, 1240-1242, 950-957` | thường <1 ms tới vài ms; **không có trần** nếu OS đẩy luồng nạp ra khỏi CPU khi đang giữ khoá |
| **F5** | 🔴 **`PerfHud=1` ĐANG BẬT trong `config.ini` thật** ⇒ gọi PDH GPU Engine (đếm ký tự đại diện) **mỗi giây ngay trong `UiPaint`**, nằm gọn trong ô `end` của `[PDET]` | — | vài ms tới **vài chục ms** mỗi lần thu thập |
| **F6-CHECKBALANCE / F8 / KIS-01** | Vượt trần cứng `KIS_HARD_IMAGE_CEIL = 2048` thì `dwIdleLimit` **tụt từ 100.000 ms về 0** ⇒ cache thoái hoá thành "xoá sạch mỗi lượt quét" ⇒ biến cú đơ MỘT LẦN thành **đơ LẶP LẠI** | `KImageStore2.cpp:574-585` | mỗi ảnh bị đuổi oan phải trả lại đúng giá mục 12.1. **CẦN ĐO: chưa xác nhận có thật sự vượt 2048 hay không** |
| **F7** | Rò rỉ `new KPAL24[]` 768 byte **mỗi lần nạp SPR** (98,7% spr rơi vào nhánh rò rỉ), trên client 32-bit **KHÔNG `LARGE_ADDRESS_AWARE`** (trần 2 GB) | — | 0 ms trực tiếp; gián tiếp: malloc khối lớn trên heap phân mảnh tốn thêm vài ms |
| **F10** | `timeBeginPeriod(1)` **chỉ được gọi khi `PaintFps > 30`**. Với `PaintFps = 30` thì `Sleep(1)` nhảy bậc ~15,6 ms | `S3Client.cpp:511, 611` | dấu vết: `[SPIKE]` có `total=16..31` mà `logic=0 paint=0 painted=0` |
| **F4-ÂM THANH** | `KNpcRes::Draw` gọi thẳng nạp `.wav` + tạo 3 buffer DirectSound | — | đọc pak 0,05-0,15 ms; `CreateSoundBuffer` + 2× `DuplicateSoundBuffer` **chưa đo được**, ước 0,2-3 ms |
| **KIS-06** | HAI hàm băm tên tệp khác nhau cùng làm khoá một mảng: `ImageNameToId` **thiếu `i++`** và gộp hoa-thường; `g_FileName2Id` có `i++` nhưng **phân biệt hoa-thường** ⇒ cùng một tệp nằm hai khe, `FreeImage` không bao giờ tìm thấy ảnh NPC | — | mỗi tệp bị nhân đôi khoá = thêm một lần nạp nguội đầy đủ + chiếm thêm khe trong tổng 2048 |
| **L4-09** | `KNpc::PaintSeriesNpc` dùng **con trỏ chưa khởi tạo** rồi `strcpy`, chạy cho MỌI NPC MỌI KHUNG | — | nếu là rác thật: ~0,01-0,05 ms/NPC/khung + **nguy cơ sập** |
| **L4-08** | **Đua dữ liệu**: luồng nạp `realloc` mảng vật phủ nền trong khi luồng vẽ đang duyệt **chính mảng đó** (hai khoá KHÁC NHAU) | — | khi trúng: hoặc sập, hoặc tra pak thất bại hàng loạt |

## 12.7 ĐÃ BÁC BỎ cho vế A (đừng điều tra lại)

| Nghi can | Vì sao loại |
|---|---|
| **`RepresentBegin` khoá surface** (giả thuyết ban đầu của tôi) | **SAI.** `RepresentBegin` **không khoá surface nào**, và trong đường game nó là **hàm rỗng**. ⇒ ô `begin` của `[PDET]` **luôn ≈ 0** — đừng dùng nó làm bằng chứng cho bất cứ kết luận nào |
| Đám đông ⇒ nhiều sprite ⇒ nhiều surface ⇒ hết VRAM | **SAI.** Represent2 **không tạo surface nào cho sprite**; canvas nằm ở **SYSTEM MEMORY**. Số surface DirectDraw = 3 + (số ảnh BITMAP16 của UI), **cố định** |
| Giải phóng ảnh kéo theo giải phóng surface DirectDraw | **SAI.** Không có thao tác surface nào trong kho ảnh. (Nhưng **có rò rỉ surface thật** ở chỗ khác: 1 surface mỗi lần `Free`/`FreeImage`/`Init` — theo lần đổi map, không theo khung vẽ) |
| Cache glyph — "tên mới = glyph mới = phải dựng lại bitmap chữ" | **SAI.** Bảng chữ nạp **một lần lúc khởi động**, tra cứu O(1). Không có đường nạp glyph lười. Phí tổn thật của khâu chữ là `Lock/Unlock` ~1-3 µs/ký tự, **ổn định, không đột biến lần đầu** |
| `DDERR_SURFACELOST` gây treo từng khung | **SAI.** Chỉ xử lý ở `WM_ACTIVATEAPP`, không có `IsLost`/`Restore` nào trong đường vẽ ⇒ mất surface gây **màn hình đứng hẳn**, không gây treo từng khung |
| `RepresentEnd` (Blt/DWM) là cú treo | **KHÔNG.** Là 2 cú `Blt` toàn màn hình mỗi khung (system→video→primary), `Flip` bị tắt ⇒ ~1-4 ms **ĐỀU**, không tỉ lệ với số người mới ⇒ `end` lớn **không giải thích được** triệu chứng |
| Tra cứu kho ảnh O(N) | **SAI.** O(log N), ~11 so sánh, <0,1 µs. Chèn ~1-2 µs/ảnh. Cả hai **không giải thích** cú 25-48 ms |
| `CheckBalance` nổ vài lần mỗi khung ở trạng thái bình thường | 0,06-0,5 ms/khung — **chấp nhận được**, chỉ nguy hiểm khi vượt trần 2048 |
| `PaintMana` | **mã chết** |

## 12.8 CÁCH ĐO ĐỂ CHỐT (chưa cần build lại)

⚠️ **`PaintLog` hiện đang là 0 trên máy thật ⇒ chưa có log nào được ghi.**

1. `config.ini` mục `[Client]`: đặt `PaintLog=1`. **Tạm đặt `PerfHud=0`** (đang bật, làm
   nhiễu ô `end`).
2. Chạy vào chỗ đông, đi tới gần nhóm người **lần đầu**, rồi đi lại chỗ đó **lần hai**.
3. Đọc `jx_paint.log` cạnh `Game.exe`:
   - **`[SPIKE] paint=25-48` mà CÓ `[PDET]` cùng lúc, `render` lớn** ⇒ **xác nhận mục 12.1**
     (nạp SPR trong vòng vẽ). Đây là kết quả dự đoán.
   - **`[SPIKE] paint=25-48` mà KHÔNG có `[PDET]`** ⇒ cú treo ở **POSSHIFT** ⇒ nghi can
     **L4-01** (`WaitForSingleObject` chờ luồng nạp cảnh).
   - **`begin` lớn** ⇒ không thể xảy ra (hàm rỗng) — nếu thấy thì phép đo sai.
   - **`end` lớn** ⇒ PerfHud/DWM, **không** phải triệu chứng này.
   - **`total=16..31` mà `logic=0 paint=0 painted=0`** ⇒ nghi can **F10** (độ phân giải timer).
4. **Phép thử vàng:** đi lần hai qua đúng chỗ đó. Nếu **hết đơ** ⇒ khẳng định là chu kỳ
   nạp-rồi-cache ⇒ mục 12.1. Nếu **vẫn đơ** ⇒ cache đang bị đuổi ⇒ nghi can **F6/F8**
   (vượt trần 2048).
5. **Tắt `PaintLog` lại sau khi đo** — mỗi lần spike nó `fopen`+`fprintf`+`fclose`
   (~0,1-3 ms) và bị tính vào `paint=` của `[SPIKE]`.

## 12.9 HƯỚNG SỬA ĐỀ XUẤT (mô tả — **CHƯA LÀM**)

| Ưu tiên | Việc | Rủi ro | Khối lượng | Hiệu quả ước tính |
|---|---|---|---|---|
| **1** | **Đóng gói lại pak ở chế độ `TYPE_FRAME`** (nén theo khung). **KHÔNG đụng một dòng mã** — engine đã có sẵn đường `GetSprFrame`/`bSingleFrameLoad` | **Rất thấp** | công cụ đóng gói + kiểm thử | **3,7-5,3 ms/người → dưới 0,1 ms/người**. Diệt ~99% chi phí |
| **2** | **Ngân sách trải cho tài nguyên nhân vật** — giống `nFarBudget` của `PrerenderGround`: mỗi khung chỉ cho nạp N tệp / M byte, phần còn lại vẽ tạm (bỏ qua bộ phận chưa có) rồi làm nốt khung sau | Vừa — người mới có thể thiếu vài bộ phận trong 1-2 khung đầu | Vừa | Biến 1 khung +54 ms thành 10 khung +5 ms |
| **3** | **Bỏ `Load()` file khỏi `PaintInfo`** — nạp `HonorSetting.txt`/`PlayerTitle.txt`/`NpcBobo` **một lần lúc khởi động** vào bảng tra tĩnh | Thấp | Nhỏ | −5 đến −20 ms/khung liên tục |
| **4** | **Trả `bSelect` về `false`** trong `CoreDrawGameObj.cpp:37,48-50` (mã gốc còn trong chú thích cùng dòng) | Thấp — nhưng **đổi hành vi nhìn thấy được** (quái không còn hiện tên/thanh máu). **Phải hỏi ý chủ game trước** | Rất nhỏ | −1 đến −3 ms/khung |
| **5** | **Áp lại bản vá N2 của `ce8c4d49`** (cắt cảnh vòng `PaintGround` theo `m_RepresentArea`) — nó **chưa từng được áp** | Thấp | Nhỏ | −5 đến −40 ms/khung trong ~9 khung sau mỗi lần nạp region |
| **6** | **Gỡ `Breathe()` khỏi khung vẽ** (`CoreShell.cpp:11836-11841`) hoặc bỏ `WaitForSingleObject` trong `Breathe` khi gọi từ đường vẽ | **Cao** — chính `9f759631` thêm nó vào để chữa "giật lùi camera tại biên region". Gỡ ra là lỗi cũ quay lại. Cần thiết kế khác | Vừa | Diệt cú chờ đĩa 20-200 ms |
| **7** | `g_SetPakFileMode(1)` để **thôi ưu tiên đọc đĩa trước** (bỏ `CreateFile` chắc chắn thất bại mỗi lần nạp) | Thấp — nhưng phải chắc mọi tệp đều có trong pak | Rất nhỏ | −0,6 ms mỗi cụm 94 tệp + bỏ nguy cơ đọc tệp `Spr\` rời 15,8 MB |
| **8** | Vá rò rỉ `KPAL24` 768 B/lần nạp; bật `LARGE_ADDRESS_AWARE`; vá `PaintSeriesNpc` con trỏ chưa khởi tạo; vá đua dữ liệu `realloc` (L4-08) | Thấp-vừa | Nhỏ mỗi cái | Chống sập + chống phân mảnh dài hạn |

## 12.10 Còn bỏ ngỏ

- Workflow **bị cắt giữa chừng do hết hạn mức phiên** — 55 giả thuyết thu được nhưng chỉ
  16 có phán quyết phản biện. Các mục đánh "(chưa xét)" **chưa qua phản biện độc lập**,
  cần kiểm lại trước khi tin tuyệt đối. Riêng mục 12.1/12.3 đã có **số đo thật** nên
  đáng tin nhất.
- Chưa xác nhận `m_nNumImages` có thật sự vượt 2048 lúc chơi đông hay không (quyết định
  nghi can F6/F8 sống hay chết).
- Chưa đo `CreateSoundBuffer` + `DuplicateSoundBuffer` (cần chạy trong tiến trình game).
- Chưa đo chi phí một cặp `Lock/Unlock` DirectDraw trên Win10/11 (ddraw mô phỏng qua
  D3D9) — cần benchmark tại chỗ. Ước 1.500-2.500 cặp/khung trong đám đông.
- **Dữ liệu thô đầy đủ** (55 giả thuyết, có trích mã nguồn): `journal.jsonl` trong
  `...\subagents\workflows\wf_7e5f00dc-45f\`. Có thể resume workflow bằng
  `Workflow({scriptPath: "...\dieu-tra-dung-khung-hinh-ve-wf_7e5f00dc-45f.js",
  resumeFromRunId: "wf_7e5f00dc-45f"})` — các agent đã xong sẽ trả kết quả từ cache.

---

# 13. ĐÃ THI CÔNG (phiên 20/08 tối) — 4 commit mã + công cụ repack pak

> Mục này thay trạng thái "CHƯA SỬA GÌ" ở đầu tài liệu. Tất cả build PASS
> (Client Release|Win32 + Server Release|x64 + Game.exe), **chưa test trong game**.

## 13.1 Vế B — `8474e504` áp lại bản vá bị revert oan

`git revert eba30b04` sạch, không conflict. `m_nNeedFixPos` nằm trong
`#ifndef _SERVER` (KNpc.h:523-548) — server không đổi layout. Nội dung đúng như
mô tả mục 2: người khác bị chắn thì giương cờ + giữ trạng thái chạy thay vì
`DoStand()`; `SyncNpcMin` thấy cờ thì ghi đè toạ độ thật từ gói rồi hạ cờ.

## 13.2 Vế A — 3 fix mã đã vào

| Commit | Fix | Nội dung |
|---|---|---|
| `59de0016` | **#3 (L4-02/L5-01)** | `KNpc::PaintInfo` thôi `Load()` + parse (kèm AES) 3 tệp cấu hình cho từng NPC từng khung vẽ — chuyển thành 3 bảng `static` nạp đúng một lần. Vị trí cũ: honor :6057, bobo :6084, rank :6205/:6213. Ước −5..−20 ms/khung chỗ đông người có danh hiệu. |
| `5d6c5381` | **#5 (L4-04)** | **Áp thật sự N2 của `ce8c4d49`** (xác nhận lại: diff commit đó CHỈ chứa N4, N2 chưa từng vào mã). Vòng `PaintGround` giờ bỏ qua region không giao `m_RepresentArea` (rect region = `GetRegionIdx()` × 512×1024, nới 64px). KHÔNG thêm khoá mới — trong mã có sẵn HAI thứ tự lấy khoá ngược nhau (`:1208` Process→RegionList vs `:1746` RegionList→Process), thêm cặp lồng nữa là tăng nguy cơ deadlock; vụ đua realloc (L4-08) để xử riêng. |
| `ae76b2e7` | **12.5 (mù đo)** | `[SPIKE]` giờ có ô `shift=` riêng cho POSSHIFT; `paint=` chỉ còn UiPaint thật. Phép thử dứt điểm thành: `shift` lớn ⇒ L4-01; `paint` lớn + `[PDET] render` lớn ⇒ nạp SPR trong vòng vẽ (12.1). |

## 13.3 Vế A ưu tiên 1 — công cụ repack pak `TYPE_FRAME` ĐÃ DỰNG XONG

`ReverseTools\repack_typeframe\` (tool + build_repack.cmd + verify_pak.py).

- **Cách hoạt động**: transcode TẠI CHỖ từng mục — mục UCL-nguyên-tệp nào giải nén
  ra SPR hợp lệ (magic `SPR\0`), `Frames ≥ 2`, cỡ ≥ ngưỡng (mặc định 64 KB) thì
  tái nén theo khung đúng bố cục `Sources\Pack\main.cpp:115-155` (khung ≥256B nén
  NRV2B lvl 10, <256B lưu thô size âm, cờ `0x11`). **Không cần biết tên tệp** (uId
  giữ nguyên) — hoá giải "mắt xích thiếu" tưởng phải dựng lại cây thư mục.
- **An toàn**: mọi mục chuyển đổi được VERIFY vòng tròn ngay trong tool (mô phỏng
  `GetSprFrame` từng khung, so từng byte với bản gốc; lệch = abort). Kèm verifier
  python độc lập so header/index/mục-copy. Mục có khung 0-byte / vượt 16 MiB /
  pak >65535 mục (trần WORD `Reserved[2]`) tự động giữ nguyên.
- **Đã kiểm chứng**: `skills.pak` 257 mục → 131 chuyển, verify PASS cả hai tầng.
  `ucl_init()` của ucl-1.01 kêu fail oan trên MSVC 2022 (assert đời 2000) — thay
  bằng self-test chức năng nén↔giải nén 300 KB; **tool phải build x86** (x64 hỏng
  thật do truncation con trỏ trong thư viện).
- Engine đọc frame-mode **tự kích hoạt theo cờ trong pak** — không cần đổi mã,
  không cần config (bằng chứng: `KImageStore2.cpp:811/:833` rẽ nhánh theo
  `pOffsTable == NULL`, chỉ nhánh TYPE_FRAME trả NULL).
- ⚠️ Mục TYPE_FRAME **không đọc được bằng `KPakFile::Read` phẳng** — đường duy
  nhất trong client là `KSprite::Load` bọc `#ifdef TOOLVERSION` (chưa từng define
  = mã chết), nhưng **công cụ ngoài đọc pak (extractor/viewer) sẽ phải cập nhật**.

**Triển khai (việc của chủ game, tool KHÔNG tự ghi đè pak đang chạy):**
1. Chạy `repack_typeframe.exe <pak gốc> <pak mới> 65536` cho: `spr.pak`,
   `update01.pak`, `updatejx14.pak`, `updatejx15.pak` (4 pak chứa SPR nhân vật/trang
   bị theo mục 12.3; các pak updatejx khác cũng lợi nhưng ưu tiên 4 cái này).
2. Chạy `python verify_pak.py <pak gốc> <pak mới>` — phải in OK.
3. Backup pak gốc, thay bằng pak mới, vào chỗ đông test. Payload nén sẽ phình
   ~3-6% (mất nén liên-khung) — đổi lấy nạp 1 khung ~1,6 KB thay vì 200 KB.

## 13.4 Phát hiện mới trong phiên (chưa xử lý)

- 🔴 **`_EXCLUDE_OUTSIDE_OBJECT` không được define ở đâu cả** ⇒ macro cắt cảnh
  `SM_IsOutsideRepresentArea` (SceneMath.h:24-31) là `false` cứng toàn dự án —
  `PaintAboveHeadObj` vẽ mọi vật thể trên-đầu kể cả ngoài màn. Không phải
  regression (chết từ gốc), nhưng là tối ưu tiềm năng; bật define là đổi hành vi
  rộng, cần đo riêng trước.
- `Sources\S3Client` post-build copy trỏ `..\..\..\bin` = `D:\bin` (ngoài cây) ⇒
  build Game.exe luôn "fail" ở MSB3073 dù link xong; binary thật nằm ở
  `Sources\S3Client\Release\Game.exe`.
- Fix #7 (`g_SetPakFileMode(1)`): mode 1 VẪN fallback đĩa khi pak miss
  (KPakFile.cpp:260-266) nên không mất tài nguyên, NHƯNG nếu tệp rời trong `Spr\`
  (887 MB trên client thật) là bản override cố ý khác nội dung pak thì đổi mode
  đổi hình hiển thị ⇒ chờ chủ game xác nhận (hoặc so hash) rồi mới làm.
- Fix #4 (`bSelect=false` trong CoreDrawGameObj) — đổi hành vi nhìn thấy (quái
  hết hiện tên/máu khi không chọn) ⇒ **chờ ý chủ game**, chưa làm.
- L4-01 (`Breathe()`/`WaitForSingleObject` trong khung vẽ) — rủi ro cao (chính
  `9f759631` thêm vào để chữa giật lùi camera biên region), cần thiết kế khác,
  chưa đụng.

## 13.5 Việc chủ game cần làm để ĐO

1. `config.ini` `[Client]`: `PaintLog=1`, tạm `PerfHud=0`. Binary mới có ô
   `shift=` trong `[SPIKE]` — đọc theo bảng 12.8 (đã hết mù đo POSSHIFT).
2. Vào chỗ đông lần đầu / lần hai như mục 12.8, gửi lại `jx_paint.log`.
3. Vế B: đứng yên giữa đám đông nhìn người khác — kỳ vọng hết cảnh
   đứng-khựng-rồi-nhảy (bản vá `m_nNeedFixPos` + nội suy sẵn có kéo mượt).
4. Sau khi thay pak TYPE_FRAME: lặp lại phép đo — kỳ vọng `[PDET] render` giảm
   mạnh, cú đơ khi người mới lọt tầm nhìn còn dưới ngưỡng mắt thấy.
