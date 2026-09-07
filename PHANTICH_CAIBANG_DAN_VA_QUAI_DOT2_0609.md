# ĐỢT 2 — CÁI BANG "BỔNG & RỒNG" (ĐẠN TRUY ĐUỔI) + QUÁI DI CHUYỂN (ĐÀO SÂU)
> 06/09/2026 · **CHỈ PHÂN TÍCH, KHÔNG SỬA GÌ** · tiếp nối `PHANTICH_QUAI_DICHUYEN_VA_DAN_TRUYDUOI_0609.md`

---

## PHẦN A — HAI CHIÊU CÁI BANG "90"

### A.1 Xác định đúng hai chiêu

| | Tên | SkillId | ReqLevel | MisslesForm | ChildSkillId → viên đạn | MoveKind của đạn |
|---|---|---|---|---|---|---|
| **rồng** | Phi Long Tại Thiên (飞龙在天) | **357** | 80 | 0 = Wall | 166 | **5 = truy đuổi** |
| **bổng** | Thiên Hạ Vô Cẩu (天下无狗, Đả Cẩu Bổng) | **359** | 80 | 0 = Wall | 168 | **5 = truy đuổi** |

Cả hai đều bắn **đạn TRUY ĐUỔI (`MoveKind = 5`)** — đúng cơ chế đã mổ ở đợt 1.
Nhật ký máy chủ đang chạy (`jx_auto_server.log`) có sẵn bằng chứng: `[MIS-FLY-FOLLOWSTATE] skill=357 …` và `skill=359 …` với `follow=<chỉ số NPC hợp lệ>` ⇒ **khoá mục tiêu ở JX1 vẫn hoạt động**, không phải lỗi mất mục tiêu.

Dữ liệu hai viên đạn 166/168 **giống hệt nhau** ở hai bản (đã đối chiếu 16 cột động học ở đợt 1).

### A.2 GỐC KHÁC BIỆT: bản Linux có **BƯỚC ĐÁP ĐÍCH**, JX1 không có

Nhánh `MISSLE_MMK_Follow` trong `KMissle::Fly` bản Linux (`0x080758E0`, nhánh `0x08075D58`):

```c
// ---- mỗi 8 nhịp: NẮN HƯỚNG (JX1 CÓ) ----
n = m_nParam1;  m_nParam1 = n + 1;
if (n > 7) {
    m_nParam1 = 0;
    if (m_nFollowNpcIdx > 0 && Npc[m_nFollowNpcIdx].m_dwID == m_dwFollowNpcID) {
        GetMpsPos(this, &sx, &sy);                                   // 0x08074940
        GetMpsPos(&Npc[m_nFollowNpcIdx], &m_nDesX, &m_nDesY);        // 0x08076BE0  ← GHI LẠI TOẠ ĐỘ ĐỊCH (+0x88/+0x8C)
        dist = sqrt(dx*dx + dy*dy);
        if (dist) {
            m_nXFactor = (dx << 10) / dist;
            m_nYFactor = (dy << 10) / dist;
            m_nParam2  = dist / m_nSpeed + 1;                        // ← SỐ NHỊP CÒN LẠI TỚI ĐÍCH
            m_nDirIndex/m_nDir = g_GetDirIndex / g_DirIndex2Dir;
        }
    }
}
// ---- MỖI NHỊP: ĐẾM LÙI + BƯỚC ĐÁP ĐÍCH (JX1 KHÔNG CÓ) ----   0x08075DA0
if (m_nParam2 != 0 && --m_nParam2 == 0)
{
    GetMpsPos(this, &x, &y);
    nDOffsetX = (m_nDesX - x) << 10;      // MỘT BƯỚC DUY NHẤT, ĐẶT ĐÚNG LÊN TOẠ ĐỘ ĐÃ GHI
    nDOffsetY = (m_nDesY - y) << 10;
}
else
{
    nDOffsetX = m_nXFactor * m_nSpeed;    // ← JX1 CHỈ CÓ ĐÚNG HAI DÒNG NÀY
    nDOffsetY = m_nYFactor * m_nSpeed;
}
```

**JX1 (`KMissle.cpp:985-1017`)**:

```c
case MISSLE_MMK_Follow:
    if (m_nFollowNpcIdx > 0) {
        if (m_nTempParam1++ >= 8) {
            m_nTempParam1 = 0;
            if (m_nFollowNpcIdx > 0 && Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID)) {
                ... Map2Mps hai đầu; nDistance = GetDistance(...)
                m_nXFactor = ((nDesMpsX - nSrcMpsX) << 10) / nDistance;
                m_nYFactor = ((nDesMpsY - nSrcMpsY) << 10) / nDistance;
                m_nDirIndex/m_nDir = ...
                // KHÔNG ghi lại toạ độ đích, KHÔNG tính số nhịp tới đích
            }
        }
    }
    nDOffsetX = m_nXFactor * m_nSpeed;   // luôn luôn
    nDOffsetY = m_nYFactor * m_nSpeed;
    break;
```

**Hệ quả trực quan**

| | Linux | Dự án JX1 |
|---|---|---|
| Nắn hướng | mỗi 8 nhịp | mỗi 8 nhịp (giống) |
| Ghi lại vị trí địch | **có** (`m_nDesX/Y`) | **không** |
| Đếm nhịp tới đích | **có** (`m_nParam2`) | **không** |
| Bước cuối | **nhảy đúng lên vị trí địch đã ghi** — nếu đạn đã vọt qua thì bước này đi **NGƯỢC LẠI** | không có |
| Kết quả mắt thấy | đạn lao tới, vọt qua, **giật/bay lui đúng vào người**, lặp lại sau mỗi lần nắn hướng | đạn lao thẳng **một lượt**, vọt qua rồi hết `LifeTime` là tan |

Đạn 166 sống 24 nhịp / 168 sống 32 nhịp ⇒ **3–4 lần nắn hướng + 3–4 lần "đáp đích"** trong một viên. Với `MisslesForm = 0` (Wall) bắn 3–4 viên cùng lúc, bản Linux cho hình ảnh cả chùm đạn "quần" quanh đối thủ; JX1 chỉ có một lượt bay thẳng.

> **Dấu vết còn sót trong mã JX1**: `KMissle.h:110-112` khai báo đúng ba trường
> `m_nDesMapX / m_nDesMapY / m_nDesRegion` — chú thích gốc *"生成子弹时的目标坐标"* (toạ độ mục tiêu lúc sinh đạn).
> Toàn bộ mã JX1 **chỉ gán 0** cho chúng (`KMissle.cpp:229-231` và `1263-1265`), **không có một chỗ đọc nào**.
> Đây chính là ba trường mà bản Linux dùng ở `+0x88/+0x8C`. Cơ chế bị bỏ dở, không phải bị hỏng.

### A.3 Các khác biệt phụ đã đo được (đều nhỏ hơn A.2)

| # | Điểm | Linux | JX1 |
|---|---|---|---|
| A-1 | **Huỷ bám mục tiêu** mỗi nhịp trong `Activate` | chỉ khi sai `m_dwID` **hoặc** khác subworld | thêm `m_nProtectedTime > 0` (vòng tròn bất tử) và `m_HideState.nTime > 0` (ẩn thân) ⇒ **JX1 rơi khoá khi địch bật bất tử / ẩn thân**, Linux vẫn bám |
| A-2 | `DoHurt` của **357 Phi Long Tại Thiên** | **10** (10 % gây choáng `do_hurt`) | **0** |
| A-3 | `ChildSkillNum` gốc của **359** | 16 | 3 — *nhưng* `skill_misslenum_v` trong `gaibang.lua` của **cả hai** đều `{1,1},{20,3}` nên ở cấp tối đa hai bên đều 3 viên; cột gốc chỉ lộ ở cấp 0 |
| A-4 | `skill_param1_v` của **357** | không có | dự án **thêm** `{{1,0},{11,0},{11,32},{20,32}}` (chú thích "khoang cach 2 tia plus") |
| A-5 | `skill_misslesform_v` của 357 | `{1,1},{11,1},{11,0},{20,0}` | **giống hệt** (cấp 1-10 form 1 Line, cấp 11-20 form 0 Wall) |
| A-6 | Chọn `CastExtractiveLineMissle` ở nhánh `SKILL_MF_Line` | **chỉ** `MoveKind == 7` (Parabola) — `0x080ECEC8  cmp [g_MisslesLib[child]+0x14], 7` | `m_nChildSkillNum == 1 && (MoveKind == 1 \|\| MoveKind == 7)` (KSkills.cpp:790) — **JX1 mở thêm cho đạn bay thẳng** |
| A-7 | Khối "reclaim" cho `MoveKind==Follow` trong `CheckCollision` | **không có** trong nhị phân | **bị chú thích** (`KMissle.cpp:706-722`) — tức JX1 kế thừa đúng bản gốc, không phải hồi quy |
| A-8 | `m_bNeedReclaim / m_nFirstReclaimTime / m_nEndReclaimTime` | — | chỉ được đặt ở `CastExtractiveLineMissle` (KSkills.cpp:1507-1510) và **chỉ được đọc trong khối đã chú thích** ⇒ hiện **vô tác dụng** |

> A-6 **không** ảnh hưởng tới hai chiêu Cái Bang: ở cấp dùng thật (11-20) `skill_misslesform_v` ép 357 về form 0, còn 359 vốn đã form 0 ⇒ cả hai đi đường `CastWall` (`KSkills.cpp:1554`), nơi mục tiêu bám vẫn được gán bình thường (dòng 1612).

### A.4 Bản đồ mã liên quan (bản Linux)

| Việc | Địa chỉ |
|---|---|
| `KMissle::Breathe` (ZAxisMove nội tuyến) | `0x080760E0` |
| `KMissle::Fly` | `0x080758E0` |
| ├ MoveKind 0 (tại chỗ) / 1 (thẳng) | `0x08075D38` |
| ├ MoveKind 3 (vòng) | `0x080759AF` — dùng `(Speed + **50**)`, JX1 dùng `+30` |
| ├ MoveKind 4 (xoắn) | `0x08075AA0` — `(Speed + CurrentLife + 50)`, giống JX1 |
| ├ **MoveKind 5 (truy đuổi)** | **`0x08075D58`** · nắn hướng `0x08075EAF` · **bước đáp đích `0x08075DA0`** |
| └ MoveKind 100 (bay về) | `0x08075CC8` — `TempParam1/2` (+0x130/+0x134), giống JX1 |
| `KMissle::Activate` (huỷ bám) | `0x080769DA` |
| `KSkill::CastMissles` — bảng nhảy `MisslesForm` | `0x080ECB86`, bảng `0x08258478` (8 mục, `m_eMisslesForm` ở KSkill+0xC8) |
| `KSkill::CastLine` | `0x080EC2F0` |
| 6 chỗ gán `m_nFollowNpcIdx` | `0x080EB4B8` `0x080EB8EC` `0x080EBCA4` `0x080EC031` `0x080EC4A2` `0x080EC843` |
| `g_MisslesLib[]` | base `0x0830ED80`, `sizeof(KMissle) = 0x188` |
| `Missle[]` | `0x0836EA50` |

Bố cục KMissle (Linux): `+0x14` MoveKind · `+0x18` FollowKind · `+0x1C` cao độ · `+0x20` Zspeed · `+0x24` LifeTime ·
`+0x28` Speed · `+0x2C` SkillId · `+0x34` trạng thái · `+0x40` CollidRange · `+0x44` DmgRange · `+0x60` CurrentLife ·
`+0x6C/0x70` MapX/MapY · `+0x74` MapZ · `+0x78/0x7C` OffX/OffY · **`+0x88/+0x8C` DesX/DesY (toạ độ địch đã ghi)** ·
`+0xC8/+0xCC` XFactor/YFactor · `+0xD4/0xD8` FollowNpcIdx/ID · `+0xEC` Zacc · `+0xFC` SubWorldId · `+0x100` RegionId ·
`+0x11C/0x120/0x124` Param1/2/3 · `+0x130/0x134` TempParam1/2 · `+0x144/0x148/0x14C` DirIndex/Dir/Angle · `+0x154` MissRate.

---

## PHẦN B — QUÁI DI CHUYỂN, ĐÀO SÂU THÊM

### B.1 Toàn cảnh 14 kiểu AI của bản Linux (đã đo từng hàm)

| AIMode | Địa chỉ | Số lệnh | Gọi những gì |
|---|---|---|---|
| **1** | `0x08093E30` | 117 | NhịpAI, KeepActiveRange, GetNearestNpc, GetDistSq, KiểmTraẨn, g_Random, SetActiveSkill, **CommonAction**, **FollowAttack** |
| 2 | `0x08093B00` | 73 | NhịpAI, KeepActiveRange, GetNearestNpc, g_Random, **Flee (0x0808F590)** |
| 3 | `0x08093850` | 96 | như AI1 nhưng không có cổng KiểmTraẨn |
| 4 | `0x080936E0` | 24 | NhịpAI, KeepActiveRange |
| 5 | `0x08093490` | 68 | NhịpAI, KeepActiveRange, GetDistSq, g_Random |
| 6 | `0x08093290` | 64 | NhịpAI, KeepActiveRange, GetDistSq, g_Random |
| **7** | `0x08094040` | 20 | NhịpAI |
| **8** | `0x0808F1C0` | 24 | NhịpAI |
| **9** | `0x08092E30` | 24 | NhịpAI, KeepActiveRange |
| **10** | `0x08091EB0` | 48 | NhịpAI, GetNearestNpc, g_Random |
| 21 | `0x080956B0` | 50 | NhịpAI, FollowAttack |
| 22 | `0x08094350` | 35 | NhịpAI, FollowAttack |
| 23 | `0x08094D80` | 84 | NhịpAI, InEyeshot, FollowAttack |
| 24 | `0x08094170` | 89 | NhịpAI, InEyeshot, FollowAttack, g_Random |

**Dự án JX1 chú thích `case 7..10`** trong `KNpcAI::Activate` ⇒ số quái trong `npcs.txt` JX1 khai các mode này mà **không có AI nào chạy**: mode 7/8 = 0 con, **mode 9 = 50 con**, **mode 10 = 62 con** (tổng **112 con**). Chúng chỉ đứng yên tại điểm sinh.

### B.2 Hàm "nhịp AI" `0x0808C640` — Linux làm 2 việc, JX1 làm 1

Mọi `ProcessAIType` bản Linux **gọi hàm này ở dòng đầu**:

```c
cur = SubWorld[m_SubWorldIndex].m_dwCurrentTime;
p->m_NextAITime = cur + (BYTE)p->m_AIMAXTime;                 // (1) hẹn nhịp sau
e = p->m_nPeopleIdx;
if (Npc[e].m_Doing == do_death || Npc[e].m_Doing == do_revive) // (2) BỎ MỤC TIÊU ĐÃ CHẾT
    p->m_nPeopleIdx = 0;
else if (Npc[e].m_Kind == kind_player && Npc[e].[+0x168C] == 0)
    p->m_nPeopleIdx = 0;
```

JX1 chỉ có (1), đặt trong `Activate` trước `switch`. Việc (2) ở JX1 chỉ xảy ra muộn hơn, trong `FollowAttack → CheckNpc`. Hệ quả: quái JX1 còn **giữ khoá vào xác** thêm một nhịp AI (1,39 s) trước khi tìm mục tiêu mới; quái Linux đổi mục tiêu ngay nhịp kế.

### B.3 `SendCommand` — cổng "định thân" khác nhau về BẢN CHẤT

**Linux `0x08078AA0`**:
```c
if (cmd <= 8 && ((1 << cmd) & 0x11E) && this->[+0x1479] != 0) return;   // bỏ lệnh
// 0x11E = bit 1,2,3,4,8 = do_stand, do_walk, do_run, do_jump, do_sit
m_Command = { cmd, x, y, z };                                            // +0x193C..+0x1948
```
⇒ quái bị "đóng băng" ở Linux **không đi/đứng/ngồi được nhưng VẪN RA CHIÊU**.

**JX1 `KNpc.cpp:5857`**:
```c
if (m_FrozenAction.nTime > 0) {
    if (cmd == do_walk || do_run || do_runattack || do_jump || do_jumpattack ||
        do_skill || do_magic || do_attack || do_blurmove) return;
}
if (m_RandMove.nTime > 0) { ... }
```
⇒ quái bị đóng băng ở JX1 **không đi VÀ không đánh**, nhưng vẫn nhận `do_stand`.

Đây là hai luật khống chế khác nhau, ảnh hưởng trực tiếp cảm giác "quái phản ứng".
JX1 còn có thêm trạng thái `m_RandMove` (thuộc tính phép `randmove`, số 75) mà cổng Linux ở đây không có.

### B.4 Quái **KHÔNG tìm đường** — cả hai bản (đã xác nhận phía JX1)

`KNpc::Goto(x,y)` → `NewPath(x,y)` → `DoWalk()`, mà `KNpc::NewPath` (`KNpc.cpp:5928`) là **hàm rỗng**:

```c
BOOL KNpc::NewPath(int nMpsX, int nMpsY) { m_DesX = nMpsX; m_DesY = nMpsY; return TRUE; }
```

Không A*, không né vật cản. Đã đối chiếu **3 bản lưu cũ nhất** (`KNpc.cpp.truoc_p16_2708`, `.truoc_5loi`, `.truoc_khanglinux_0109`) — **y hệt** ⇒ đây là hành vi gốc của engine, **không phải hồi quy của dự án**.
`SubWorld::FindPath` (A*) trong JX1 **chỉ được client dùng** (`CoreShell.cpp`, auto/WAuto), engine AI quái không gọi.
> ⚠️ **Chưa đo** phía Linux: chưa xác định `NewPath` bản Linux có tìm đường hay không. Nếu bản Linux có, đó sẽ là khác biệt "thông minh" lớn nhất còn lại. Đây là việc đầu tiên nên làm ở đợt sau.

### B.5 Nhắc lại 3 khác biệt lớn của đợt 1 (vẫn là gốc chính)

1. `npcs.txt` JX1 **thiếu cột `AIMaxTime`** ⇒ mọi quái 25 khung (1,39 s/lượt); Linux 3–36 khung.
2. `CommonAction` JX1 có nhánh `IsCanInput()` vứt lệnh tuần tra khi quái rảnh; Linux không có.
3. `MINI_ATTACK_RANGE 32` (lùi giữ khoảng cách) còn chạy ở Linux, **bị chú thích** ở JX1.

---

## C — VIỆC CÒN LẠI (chưa đo, đừng kết luận)

* `KNpc::NewPath` / đường đi thực tế của lệnh `do_walk` **bản Linux** — có tìm đường không (B.4).
* Thân hàm AI 2, 5, 6, 9, 10 bản Linux (mới có địa chỉ + danh sách hàm gọi).
* Ý nghĩa hai cờ `+0x19A0/+0x19A4` (cổng lọc mục tiêu `0x08079200`) và `+0x168C` (cổng người chơi trong `0x0808C640`), `+0x1479` (cờ định thân).
* Phía **client** (`Patch\game_y_unpacked.bin`): cách vẽ đạn truy đuổi — client tự mô phỏng đạn từ gói `s2c_skillcast`, nên nếu port bước đáp đích thì **phải port cả hai phía** mới thấy hiệu ứng.
