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

> **06/09 đêm — ĐÃ ĐO TOÀN BỘ ở ĐỢT 3 (Phần F bên dưới).**

* `KNpc::NewPath` / đường đi thực tế của lệnh `do_walk` **bản Linux** — có tìm đường không (B.4).
* Thân hàm AI 2, 5, 6, 9, 10 bản Linux (mới có địa chỉ + danh sách hàm gọi).
* Ý nghĩa hai cờ `+0x19A0/+0x19A4` (cổng lọc mục tiêu `0x08079200`) và `+0x168C` (cổng người chơi trong `0x0808C640`), `+0x1479` (cờ định thân).
* Phía **client** (`Patch\game_y_unpacked.bin`): cách vẽ đạn truy đuổi — client tự mô phỏng đạn từ gói `s2c_skillcast`, nên nếu port bước đáp đích thì **phải port cả hai phía** mới thấy hiệu ứng.

---

## PHẦN D — THIÊN NHẪN "120" (đối chiếu hai bản)

Bộ ba kỹ năng 120 của Thiên Nhẫn (`ReqLevel = 120`), dữ liệu cấp nằm ở
`script/skill/tianren.lua` (Linux) và `script/nhanvat/kynang/tianren.lua` (JX1),
khoá `tianren120`:

| SkillId | Tên | Ghi chú |
|---|---|---|
| 711 | Hấp Tinh Yểm | thế thân (SkillStyle 3) |
| 714 | Hỗn Thiên Khí Công | thế thân (SkillStyle 3) |
| 715 | Ma Âm Phệ Phách | trạng thái (SkillStyle 2) |
| 719 / 720 | …_Quyết Chú | bản chủ động |
| 723 | Ma Âm Phệ Phách – Hoảng loạn | chiêu nổ ra khi chết |
| 1223 / 1236 | biến thể | |

### Khác biệt đo được

| # | Kỹ năng | Linux | Dự án JX1 |
|---|---|---|---|
| TN-1 | **720** Hỗn Thiên Khí Công_Quyết Chú | có thêm `fastwalkrun_p` (**tăng tốc chạy**), dữ liệu `gaibang120zuzhou` | **KHÔNG có** |
| TN-2 | **720** | `LvlSetting1/2` = `physicsresmax_p` / `fireresmax_p`, `LvlSetting4/5` = `physicsres_p` / `fireres_p` | **đảo ngược**: 1/2 = `physicsres_p` / `fireres_p`, 4/5 = `…resmax_p` — cùng `LvlData` nhưng **áp sai thuộc tính** (kháng thường ↔ trần kháng) |
| TN-3 | **723** Ma Âm Phệ Phách – Hoảng loạn | `SkillStyle = 14` | `SkillStyle = 0` |
| TN-4 | **715** Ma Âm Phệ Phách | không có | JX1 **thêm** `skill_eventskilllevel = tianren120` |
| TN-5 | **711** Hấp Tinh Yểm | không có | JX1 **thêm** `autoattackskillplus = wudu120` |
| TN-6 | **714** Hỗn Thiên Khí Công | không có | JX1 **thêm** `autoattackskillplus = gaibang120` |
| TN-7 | bảng `tianren120` | không có | JX1 **thêm** `skill_eventskilllevel={{{1,1},{20,20}}}` |
| TN-8 | 719 · 1223 · 1236 | — | **giống hệt** |

Phần lõi (`skill_cost_v`, `skill_mintimepercast_v` 45→20 giây, `autodeathskill = 723*256+41…60`,
`randmove`/`missle_missrate` đều bị chú thích ở **cả hai bản**) là **như nhau**.

### ⚠️ Một chỗ hỏng mã hoá trong tệp JX1 (không phải khác biệt thiết kế)

`script/nhanvat/kynang/tianren.lua` của dự án bị **mất byte** ở 4 chỗ trong `skill_desc`
của `tianren120` — so với bản Linux:

| Bản Linux | Bản JX1 |
|---|---|
| `m?c ti�u g?n b? ho?ng lo?n` (bị + dấu cách) | `m?c ti�u g?n b?ho?ng lo?n` |
| `c? x�c su?t` | `c?x�c su?t` |
| `kh�ng th? t?n c�ng v? di chuy?n` | `kh�ng th?t?n c�ng v?di chuy?n` |

Đúng dấu hiệu của Gate 1 (sửa tệp TCVN3 bằng công cụ đọc UTF-8): ký tự `ị`/`ề` và dấu cách
sau nó bị nuốt. **Chỉ ảnh hưởng dòng mô tả kỹ năng hiện trên giao diện**, không ảnh hưởng số liệu.
Chưa sửa — chờ chủ game quyết (sửa được bằng `vn_edit.py`).

---

## PHẦN E — ĐÃ THI CÔNG (06/09, commit `0ffc42dd`, đã lên origin/main `cbceded5`)

| # | Việc | Tệp | Bộ vá |
|---|---|---|---|
| 1 | **Bước đáp đích** cho đạn truy đuổi (A.2) | `Sources/Core/Src/KMissle.h` + `KMissle.cpp` | `ReverseTools/goi_va_dan_dapdich_0609.py` |
| 2 | **Bật AI quái 7/8/9/10** (B.1) | `Sources/Core/Src/KNpcAI.cpp` | `ReverseTools/goi_va_ai_7_10_0609.py` |
| 3 | **Thêm cột `AIMaxTime`** vào `settings/npcs.txt` (máy chủ + khách) | cây chạy thật `E:/SourceTuanLe/.../bin/{server,client}/settings/npcs.txt` | `ReverseTools/goi_va_npcs_aimaxtime_0609.py` |

**Giữ nguyên theo yêu cầu**: cửa chặn huỷ bám mục tiêu **bất tử / ẩn thân** của dự án
(`KMissle.cpp:506-511`) — không đụng một dòng.

**Không làm** (vì sẽ *sửa* hành vi đang có chứ không phải *thêm* phần thiếu — chờ chủ game):
* bỏ nhánh `IsCanInput()` trong `CommonAction` (B.5 mục 2);
* mở lại khối `MINI_ATTACK_RANGE 32` đang bị chú thích (B.5 mục 3);
* `MISSLE_MMK_Circle` Speed+30 → +50;
* 4 chỗ hỏng mã hoá trong `tianren.lua` (Phần D).

**Số liệu nhịp AI sau khi thêm cột** (2 090/2 643 dòng được chép, 553 dòng để trống = giữ 25):

| AIMode | số mẫu | nhịp cũ | nhịp mới (trung bình) |
|---|---|---|---|
| **1** (quái thường) | 1 245 | 25 khung (1,39 s) | **11,6 khung (0,65 s)** |
| 2 | 38 | 25 | 33,8 (chậm lại) |
| 3 | 78 | 25 | 34,0 (chậm lại) |
| 9 | 50 | 25 | **6,9 (0,39 s)** |
| 10 | 62 | 25 | 28,1 (chậm lại) |

⇒ quái luyện công chạy AI **~2,15 lần dày hơn**, không phải 8 lần như ước lượng thô ban đầu.
Nền hiện tại (`jx_perf_server.log`, 1 001 người/bot): `TICK` 6,9–7,2 ms (12–13 % ngân sách),
`SW_ACTIVATE` 4,1–4,5 ms, `tre = 0`. Dự kiến `TICK` lên khoảng 9–11 ms — vẫn dư ngân sách,
nhưng **đo lại `jx_perf_server.log` sau khi swap** rồi hãy kết luận.

### Nhị phân chờ swap

| Tệp | MD5 | Cỡ |
|---|---|---|
| `bin/server/CoreServer.dll.moi` | `b43c85e89f8726b5e86fbafb5aa853b4` | 18 466 816 |
| `bin/client/CoreClient.dll.moi` | `964da287399ce9ffa05d4efdf929e58e` | 2 607 104 |

> Bản `.moi` cũ của phiên khác (`d0bc87a4…`, sửa mất đồ Cái Bang) đã được giữ lại ở
> `CoreServer.dll.moi.truoc_dandap_0609`. Bản mới **build từ origin/main đã gộp**, nên
> **đã bao gồm** bản vá mất đồ đó — không mất gì.

**Cần khởi động lại máy chủ** (đổi `npcs.txt` + đổi DLL), và client cần chạy `ChoiGame.bat`.

### Cách lùi từng phần

* Đạn + AI 7-10: `git revert 0ffc42dd` rồi build lại.
* Nhịp AI quái: khôi phục `npcs.txt.truoc_aimaxtime_0609` ở cả `bin/server/settings` và `bin/client/settings` (không cần build lại).

---

# ĐỢT 3 (06/09 đêm) — LÀM TIẾP PHẦN C + PHÂN TÍCH SÂU THIÊN NHẪN 120
> **CHỈ PHÂN TÍCH, KHÔNG SỬA GÌ.** Công cụ phiên này lưu ở `ReverseTools/mo_nhi_phan_0609/`
> (`elfre.py` dis/xref, `fnfull.py` dis trọn hàm, `sweep.py` → `dis_all.txt` 3,1 triệu lệnh,
> `magicmap2.py` → `linux_magicnames.txt` 336 tên thuộc tính phép theo đúng chỉ số enum Linux,
> `cl.py`/`clsweep.py` cho client `Patch/game_y_unpacked.bin` (VA = 0x401000 + offset), `skdiff.py` so `skills.txt`).

---

## PHẦN F — KẾT QUẢ CÁC VIỆC CÒN LẠI CỦA PHẦN C

### F.1 `NewPath` / `do_walk` bản Linux: **CŨNG KHÔNG TÌM ĐƯỜNG** (B.4 đóng)

`KNpc::ProcCommand` Linux = `0x08088640`, bảng nhảy lệnh `0x08254AD8` (22 mục, `cmd 0..21`):

| cmd | Linux | Làm gì |
|---|---|---|
| 1 stand | `0x0808871A` | `DoStand` (`0x08080030`) |
| **2 walk** | **`0x0808874C`** | **`m_DesX(+0x14A0) = Param_X; m_DesY(+0x14A4) = Param_Y;` rồi gọi thẳng `DoWalk 0x0807B430`** |
| 3 run | `0x08088727` | ghi `m_DesX/Y` như trên rồi `DoRun 0x0807B620` |
| 4 jump | `0x080887E8` | `0x08087F00(x, y)` |
| 5 skill | `0x0808879B` | tìm trong `m_SkillList`(+0x248) `0x080E4290` → `SetActiveSkill 0x08086D90` → `DoSkill 0x08088350` |
| 8 sit · 11 · 12 hurt · 21 revive | `0x0808878E` · `0x08088771` · `0x080886D0` · `0x080886F0` | như JX1 |

`DoWalk 0x0807B430`: `nTotalFrame = m_WalkFrame(+0x161C) × m_WalkSpeed(+0x1920) / m_CurrentWalkSpeed(+0x1288) + 1`, phát gói `0x50` (`s2c_npcwalk`, 13 byte = kiểu + ID + X + Y, đúng `NPC_WALK_SYNC` của JX1) rồi `m_Doing = 2`. **Không A\*, không gọi `FindPath`.** `DoRun 0x0807B620` tương tự (gói `0x51`, `m_Doing = 3`; người chơi kiểm `Player+0x5A50` để rơi về `DoWalk`).

⇒ **Hai bản y hệt: quái đi thẳng tới đích, đâm vật cản.** Không phải hồi quy của dự án. Khác nhỏ: Linux khi `m_Doing` là chết/hồi sinh chỉ nhận `do_revive` ngay đầu hàm; nhánh không‑AI của Linux chỉ xử `do_hurt` / `do_revive` / `do_skill` (`0x080820F0`, chỉ cho người chơi) — JX1 có thêm `do_walk` khi hoảng loạn và các khối `[S13]`.

### F.2 Ba cờ chưa rõ nghĩa — đã đặt tên

| Offset KNpc (Linux) | Tên | Bằng chứng | Tương đương JX1 |
|---|---|---|---|
| `+0x19A0` | **`hide`** (giá trị ẩn thân, cộng dồn) | thuộc tính phép **idx 200 `hide`**, handler `0x08097860` (chuỗi log `"Hide + %d = %d"`): `hide += nValue[0]`; Lua `SetHide(v)` (`0x0810AD80`) và `NpcSetHide(idx, v)` (`0x08101C10`) gọi `0x0807FF80` | `m_HideState.nTime` (KState theo thời gian) |
| `+0x19A4` | cờ tạm "đang phát tin ẩn" | `=1` chỉ trong lúc `0x0807FF80` gọi `0x0807A870` (phát gói `'O'` 5 byte) rồi `=0` | không có |
| `[+4]` trong cổng lọc | `m_Index` | bố cục KNpc Linux: `+0 m_dwID`, `+4 m_Index` | `m_Index` |
| `+0x168C` | **`m_FightMode`** | `0x08079B30` = `SetFightMode`: người chơi → gọi `Player(+0x8788·idx)…(9)` trước, đổi giá trị → hai hàm PK/Team (`0x080CBE00`, `0x080C32C0`), sau đó `0x08162110(Player+0x8078, v, 0)`; JX1 `SetFightMode` cũng gọi `m_cPK.CloseAll()` | `m_FightMode` |
| `+0x1479` (BYTE) | **`frozen_action`** | handler `0x08096170` = `ProcessFunc[251]`. Bảng handler Linux **bước 8 byte** (con trỏ hàm thành viên GCC), gốc `+4`: kiểm `[+0x644] = hide (200)`, `[+0x63C] = randmove (199)`. Kề bên: `+0x1478 = forbit_attack` (250, handler `0x08096150`), idx 252 `forbit_takemedicine` (handler `0x08096190`) | `m_FrozenAction.nTime` (có thời gian) |

**Cổng lọc `0x08079200(pNpc, me)`**: `if (+0x19A4) return pNpc->m_Index == me; if (hide == 0) return 0; return pNpc->m_Index != me` ⇒ **địch đang ẩn thì AI không thấy** (trừ chính nó). Được gọi ở 10 chỗ: AI1/2/3/10, `GetNearestNpc` (`0x0808EEDE`), … JX1: `GetNearestNpc` bỏ `m_HideState.nTime > 0` nhưng `ProcessAIType01..05` **không kiểm lại mục tiêu đã khoá** khi nó ẩn (chỉ `ProcessAIType06` có).

**Hệ quả cho `SendCommand` (B.3)**: cổng định thân Linux là thuộc tính `frozen_action` — chặn stand/walk/run/jump/sit, **vẫn cho ra chiêu**; cấm đánh là thuộc tính riêng `forbit_attack` (`+0x1478`, JX1 cũng có `magic_forbit_attack` 324 từ 02/09). JX1 `FrozenAction` chặn cả di chuyển lẫn chiêu.

**Nhịp AI `0x0808C640`** còn bỏ mục tiêu là **người chơi chưa bật chiến đấu** (`m_FightMode == 0`) — JX1 chỉ làm việc này trong `ProcessAIType06`.

### F.3 Thân AI 2–10 bản Linux — đối chiếu ĐÚNG bộ hàm JX1 đang gọi

> `KNpcAI.cpp` của JX1 có **HAI bộ**: `ProcessAIType01..06` (dòng 1340‑1937, **`Activate` gọi**) và `ProcessAIType1..10` (dòng 1955‑2743; **1..6 không được gọi ở đâu**, 7..10 vừa được bản vá `[AI710 06/09]` nối vào). Đợt này so với bộ đang chạy.

| AIMode | Linux | JX1 (hàm đang gọi) | Kết luận |
|---|---|---|---|
| 2 | `0x08093B00` | `ProcessAIType02` | **TRÙNG KHÍT**: p0 tuần tra khi không địch; máu < p1 → p2 % → (`m_AiAddLifeTime < p9` && p3 %) hồi máu bằng skill 1 (`do_skill` lên mình, `AiAddLifeTime++`) ngược lại `Flee`; xa hơn p10 → p7 đứng / p8 tuần tra / `FollowAttack`; gần → p4/p5/p6 skill 2/3/4. Linux thêm: tự khoá mình → huỷ; kiểm tầm nhìn + cổng ẩn khi giữ mục tiêu cũ |
| 3 | `0x08093850` | `ProcessAIType03` | **TRÙNG KHÍT** (máu thấp: p3 % → skill 1 + `FollowAttack`, ngược lại `Flee`) |
| 4 | `0x080936E0` | `ProcessAIType04` | **TRÙNG KHÍT** (không tự tìm địch, chỉ đánh kẻ đã khoá) |
| 5 | `0x08093490` | `ProcessAIType05` | **TRÙNG KHÍT** + Linux huỷ khi tự khoá mình |
| 6 | `0x08093290` | `ProcessAIType06` | Lõi giống. **JX1 thêm 3 thứ**: bỏ mục tiêu chết/hồi sinh/ẩn; bỏ người chơi chưa bật chiến đấu; khi địch xa thì bốc skill ngẫu nhiên 1..4 (`GetRandomNumber(1,4)`) thay vì `FollowAttack` thẳng |
| **7** | `0x08094040` | `ProcessAIType7` | **KHÁC HẲN.** Linux: `if (p0 == 0) return; t = 0x0807A1F0(this, p0)` (duyệt vùng hiện tại + 8 vùng kề, gọi `0x080E1FD0(region, me, p0)` — tìm NPC theo tham số p0); có → chọn skill 1..4 theo p1..p4 → `FollowAttack(t)`. = **"Công thành Xung xa" tìm và phá mục tiêu p0** (cổng/thành). JX1: "máu thấp chạy về phía đồng minh gần nhất" |
| **8** | `0x0808F1C0` | `ProcessAIType8` | **KHÁC HẲN.** Linux: `if (p0 == 0 || p1 == 0) return;` chọn skill theo p3..p6; `SendCommand(do_skill, skill, X = p0 − p2/2 + rand(p2), Y = p1 − p2/2 + rand(p2))` = **"Đầu Thạch Xa" bắn vào ô vuông cạnh p2 quanh (p0, p1)**, không bao giờ đi. JX1: "tự sát": đi tới địch p0 %, skill p1..p3, `FollowAttack` |
| **9** | `0x08092E30` | `ProcessAIType9` | **KHÁC HẲN. Linux = HÀNH QUÂN**: (a) `KeepActiveRange` vượt → bỏ mục tiêu, thôi; (b) chọn skill theo p2/p3/p4 (còn lại → skill 4), thất bại → `CommonAction`; (c) có mục tiêu và `(+0x12A4) ≠ 0` → `FollowAttack`, thất bại → bỏ mục tiêu; (d) p0 % → **tìm địch ngẫu nhiên** (`0x0808DBA0`: gom tối đa 10 địch trong tầm nhìn, bốc 1) → **gốc `m_OriginX/Y` := vị trí hiện tại** → `FollowAttack`; (e) không địch: `p6 == 1` → **bám thủ lĩnh** `Npc[p7]` (id p8, còn sống, cùng subworld): gốc := vị trí thủ lĩnh, `FollowAttack(p7)`; `p6 ≠ 1` → **đi tới điểm (p7, p8)**: hướng = `g_GetDirIndex(me → đích)` (bảng sin/cos 64 hướng), `r = min(+0x12A8, +0x12A4)/2`, gốc := vị trí hiện tại, `SendCommand(do_walk, p7 − sin·r, p8 − cos·r)`. Vì gốc dời theo nên dây xích không cản. Script Linux dùng: `missions/newcitydefence/smalltimer.lua` `SetNpcAI(idx, 9, 20, -1,-1,-1,-1,-1, 0, aimx, aimy)` (lính hành quân tới `aimx, aimy`), `trap/trap_onroad.lua`. JX1: "càng đánh càng hăng" (skill theo ngưỡng máu, `GetNearestNpc`, `CommonAction`) |
| **10** | `0x08091EB0` | `ProcessAIType10` | **KHÁC HẲN. Linux = ĐỨNG YÊN BẮN**: khoá/tìm địch gần nhất (kiểm ẩn + tầm nhìn), chọn skill theo p1..p4 cộng dồn, thất bại → thôi; `0x0808F360(me, e)`: `dist² ≥ m_CurrentAttackRadius²` → **không làm gì (không đi)**; trong tầm và `InEyeshot` → kiểm hồi chiêu `0x080E4540` → `SendCommand(do_skill, skill, -1, e)`. **Không bao giờ `do_walk`.** JX1: "bỏ chạy" (skill theo ngưỡng máu, `Flee` p5, `FollowAttack` → **đi tới địch**) |

**Ai dùng mode 7‑10** (npcs.txt hai bản trùng id):
* mode 7 = `Công thành Xung xa` (1 mẫu, p0 = 0), mode 8 = `Đầu Thạch Xa` (1 mẫu, p0 = p1 = 0) ⇒ ở Linux hai AI này **không làm gì cho tới khi script đặt tham số**.
* **mode 9 = 50 mẫu**: Tống/Kim `Tiểu Hiệu`, `Đội trưởng`, `Tiên phong`, `Chủ tướng 1‑2`, `Chủ Soái` (10 phái × 2) [id 1067‑1099]; `Mộc nhân` 1161/2318, `Công Thành Chiến Xa` 1337, `Tuần La Quái Vật` 1807‑1808, `Hình nhân` 2319, `Trụ (ải 6)` 2325, `Trụ Kim/Mộc/Thủy/Hỏa/Thổ (ải 11)` 2331‑2335, `Tế Đài` 2336, `Thần Thú` 2337, `Tiểu Ngưu` 2340, `Quang Đoàn ×5` 2342‑2346. Tham số chung `50|25|25|25|25|30|50|0|0` (p7 = p8 = 0), Walk 6‑15.
* **mode 10 = 62 mẫu**: `Tuyệt Sát 1‑8 (Đơn)`, `Trụ (Phản đòn đơn)`, `Trụ (ải 13)`, `Tri Thù Tơ`, `cung kỵ binh`, `弓兵1`, `天池` cơ quan/bẫy… — **bẫy, trụ, cung binh**; Walk 6‑15 (chỉ 1 mẫu Walk 0).
* Script JX1: **không có `AddNpc` nào sinh mẫu 1067‑1099** (trừ 1077 trong `event_cauhoi/lib.lua`); Tống Kim JX1 (`tinhnang/tong_kim_tcap/lib_tktc.lua`) dùng mẫu 631‑641 (`Tống binh … Đại tướng`, **AIMode 1**). Các mẫu mode 9/10 vào game qua đặt sẵn trong bản đồ (`maps.pak`) hoặc `AddNpc` theo id (42 dòng, phần lớn mode 10: Tần Lăng "Tuyệt Sát", ải 13…). Bản Linux cũng không có script `AddNpc` cho 1067‑1099.

> ⚠️ **HỆ QUẢ CHO PHẦN E MỤC 2** (`[AI710 06/09]`, đang nằm trong `CoreServer.dll.moi` `b43c85e8…` **CHƯA SWAP**): bản vá chỉ nối `case 7..10` vào `ProcessAIType7..10` **của JX1**, mà bốn hàm này là **thuật toán khác** với bản Linux (bốn tên hàm giống nhau nhưng thân hàm không phải bản dịch của Linux). Nếu swap:
> 1. **62 bẫy/trụ mode 10** (Walk ≠ 0) sẽ `Flee`/`FollowAttack` = **đi theo người chơi** thay vì đứng bắn khi vào tầm.
> 2. **Trụ ải 6/11, Tế Đài, Quang Đoàn, Mộc nhân, quan quân Tống Kim mode 9** sẽ tuần tra (`CommonAction`: 20 % đi ngẫu nhiên trong nửa bán kính hoạt động) và đuổi đánh theo ngưỡng máu.
> Trước bản vá chúng **đứng im hoàn toàn và không đánh** (cũng không đúng Linux, nhưng vô hại hơn).
> **Đề nghị (chờ chủ game quyết):** (a) KHÔNG swap phần AI 7‑10 như hiện tại — chỉ swap bước đáp đích + `AIMaxTime`; hoặc (b) viết lại 9/10 theo Linux (AI10 ~40 dòng: đứng yên, bắn khi trong tầm; AI9 ~120 dòng: hành quân/bám thủ lĩnh) rồi mới bật; hoặc (c) lùi `case 9/10`, giữ 7/8 (vô hại vì p0 = 0 nhưng cũng không giống Linux). Lùi nhanh: `git revert 0ffc42dd` hoặc chạy lại đoạn `goi_va_ai_7_10_0609.py` theo chiều ngược (tệp lưu `KNpcAI.cpp.truoc_ai710_0609`).

### F.4 Client `game_y` (2022): `KMissle::Fly` **CÓ bước đáp đích**

`Fly` client = **`0x00617480`** (this = `esi`; bảng nhảy MoveKind `0x00617928`, `cmp ecx, 0x63` = 100 mục đúng như `switch` JX1). Bố cục KMissle client **trùng Linux**: `+0x14 MoveKind · +0x28 Speed · +0x60 CurrentLife · +0x88/+0x8C DesX/DesY · +0xC8/+0xCC XFactor/YFactor · +0xD4/+0xD8 FollowNpcIdx/ID · +0x11C Param1 · +0x120 Param2 · +0x130/+0x134 TempParam1/2 · +0x144/+0x148/+0x14C DirIndex/Dir/Angle`.

* Nhánh truy đuổi `0x006177F7`: `Param1++ ≥ 8` → nắn hướng, **ghi toạ độ địch vào `+0x88/+0x8C`** (`lea edi,[esi+0x88]; lea ebp,[esi+0x8C]` truyền vào `GetMpsPos`), `XFactor/YFactor = (d << 10)/dist`, **`Param2 = dist/speed + 1`** (`0x006178B7`).
* Mỗi nhịp `0x006178D9`: `Param2 ≠ 0` → `--Param2 == 0` → **bước `(Des − pos) << 10`** (`0x00617909‑0x00617920`).
* Nhánh bay về `0x00617586` (`TempParam1/2`, `Dir −= 32`) y hệt JX1/Linux.

⇒ **Client tham chiếu cũng có bước đáp đích** — bản port đợt 2 cho cả `CoreClient.dll.moi` là đúng hướng; mục "phải port cả hai phía" của Phần C được xác nhận.

### F.5 `randmove` (hoảng loạn) — cơ chế Linux vs JX1

* **Linux**: handler idx 199 (`0x080978D0`): **bộ đếm** `+0x14C4 += nValue[0]` (trạng thái hết hạn trừ lại); đổi `0 ↔ ≠0` → `0x080791E0`/`0x080791C0` tắt/bật `m_ProcessAI` (`+0x194C`); **NPC AiMode 10 được miễn**. Mỗi khung (`0x08087770`): đếm ≠ 0, không chết/hồi sinh, AiMode ≠ 10 → `0x08081DF0`: đang `do_walk` hoặc `frozen_action` → thôi; ngược lại `m_DesX/Y = vị trí + rand(100) − 50` → `DoWalk` (ghi thẳng, không qua `SendCommand`).
* **JX1** (`KNpc.cpp:1535`): `m_ProcessAI = 0`; mỗi `GAME_UPDATE_TIME` khung: đích = vị trí ± `rand(100)` (dấu ngẫu nhiên từng trục) → `SendCommand(do_walk)`; hết giờ → `m_ProcessAI = 1`.
* Khác: biên độ **±50** (Linux) vs **±100** (JX1); Linux chỉ chọn đích mới khi đã đứng lại, JX1 theo chu kỳ; Linux miễn cho AiMode 10.

---

## PHẦN G — THIÊN NHẪN 120: PHÂN TÍCH SÂU (đính chính Phần D)

### G.1 Đính chính: chỉ **715 / 723 (+ 1236)** là của Thiên Nhẫn

Phần D gộp theo `ReqLevel = 120` nên lẫn kỹ năng phái khác. Theo `LvlSetScript`, icon và bảng dữ liệu:

| Id | Tên | Phái thật | `LvlSetScript` → bảng | Icon |
|---|---|---|---|---|
| 711 | Hấp Tinh Yểm | **Ngũ Độc** | `wudu.lua` → `wudu120` | `icon_sk_wd_120` |
| 714 | Hỗn Thiên Khí Công | **Cái Bang** | `gaibang.lua` → `gaibang120` | `icon_sk_gb_120` |
| **715** | **Ma Âm Phệ Phách** | **THIÊN NHẪN** | `tianren.lua` → `tianren120` | `icon_sk_tr_120` |
| 719 | Hấp Tinh Yểm_Quyết Chú | Ngũ Độc (trạng thái áp lên địch) | `wudu120zuzhou` | |
| 720 | Hỗn Thiên Khí Công_Quyết Chú | Cái Bang | `gaibang120zuzhou` | |
| **723** | **Ma Âm Phệ Phách – Hoảng loạn** | **THIÊN NHẪN** (chiêu con của 715) | `quntisuijizoudong` | |
| 1223 | Hấp Tinh trận | Đường Môn (theo `SkillDesc`) | `autorescueskill` | |
| 1236 | Ma Âm Phệ Phách (bản sao) | Thiên Nhẫn | `tianren120` | |

JX1 cấp 715 qua `vatpham/lvl120skillbook.lua` `[7] = {715, …, "Ma Âm Phệ Phách"}`.
⇒ **TN‑1, TN‑2, TN‑5, TN‑6 là chuyện của Cái Bang / Ngũ Độc**, không phải Thiên Nhẫn (xem G.5).

### G.2 Cơ chế 715 → 723 (giống nhau ở hai bản, trừ `SkillStyle` của 723)

Dòng 715 trong `skills.txt` (114 cột, hai bản chỉ khác 2 ô: JX1 thêm `LvlSetting7/LvlData7 = skill_eventskilllevel / tianren120` và `SkillDesc` có thêm dấu `"`):
`SkillStyle = 2` (InitiativeNpcState – trạng thái chủ động lên mình) · `TargetSelf = 1` · `CharAnimId = 11` · `StartEvent = 1, StartSkillId = 723, EventSkillLevel = −1` · `MaxLevel = 20` · `IsExpSkill = 1` · `LvlSetting`: `skill_cost_v`, `autodeathskill`, `skill_mintimepercastonhorse_v`, `skill_mintimepercast_v`, `skill_desc`, `skill_skillexp_v`.

Luồng khi bấm chiêu (`KSkill::Cast` — JX1 `KSkills.cpp:486`; Linux `0x080EA920`, bảng nhảy `SkillStyle` `0x0825843C`):

1. **Style 2** → áp trạng thái của 715 lên chính mình. `autodeathskill = {723·256 + (40 + cấp), −1, 100}` → `HS_AutoSkillModify` (`KNpcAttribModify.cpp:1061`, port chuẩn Linux `0x08189000` ngày 02/09): thêm mục vào `m_DeathSkill` {skill 723, cấp 41..60, tỷ lệ 100 %, hồi 0}; thời gian −1 = giữ tới khi trạng thái bị gỡ (gỡ → giá trị đảo dấu → tỷ lệ về 0 → xoá mục).
2. **`StartEvent`** → `Cast(723, cấp)` với cấp = cấp của 715 (Linux `EventSkillLevel = −1` = cấp chính nó; JX1 `VhEventLevel` `[VHTD 02/09m]` + bảng `skill_eventskilllevel = {{1,1},{20,20}}` — hai cách, cùng kết quả).
3. **723 cấp 1‑20**: `randmove = {1, 18..72 khung}` (1→4 giây), `missle_missrate` 65→15 (tức 35 %→85 % trúng), `missle_hitcount = 6`; `ChildSkillId = 273` "Khu vực đối phương" (`MoveKind 0, LifeTime 1, CollidRange = DmgRange = 7, AutoExplode 1, ColVanish 1` — **hai bản y hệt**), `MisslesForm = 7` (AtFirer), `AttackRadius = 180`.
   * **Linux style 14** (`0x080EAAD0` → `0x080EA720`): cấp phát **một** viên 273 tại chỗ người phát (`0x08076F00`), gán skill/launcher/subworld/attribs, `Activate` (`0x08075710`) rồi **`DoVanish(1)` ngay** (`0x08075210`) = nổ vùng tức thì.
   * **JX1 style 0** (Missles) → `CastMissles` form AtFirer → một viên 273 tại chỗ, sống 1 khung rồi `AutoExplode`. **Cùng kết quả, chậm đúng 1 khung.**
4. **Trúng đòn** (`KNpc::ReceiveDamage`, JX1 `KNpc.cpp:5142`): ô `randmove[16]` có giá trị → `g_RandPercent(missrate)` → **bỏ qua toàn bộ đòn** (không sát thương, không trạng thái); trúng → `m_RandMove.nTime += khung` (cơ chế F.5).
5. **Khi chết** (`KNpc.cpp:4700 / 5033`): `m_DeathSkill[0]` có và `m_Level ≥ LEVEL_EXPLOSIVE (120)` → `DeathSkill()` → `Cast(723, 41..60)` → **đoạn 2** của bảng `quntisuijizoudong`: `randmove` 18→72 khung (cấp 41→60), `missrate = 0` = **100 % trúng**, 6 mục tiêu, bán kính 180.

Hồi chiêu 45 s (cấp 1) → 25 s (cấp 15) → 20 s (cấp 20); nội lực 25→80. Mô tả trên giao diện sinh từ `skill_desc`.

### G.3 Khác biệt **thật** về Thiên Nhẫn 120

| # | Điểm | Linux | JX1 | Ảnh hưởng |
|---|---|---|---|---|
| TN‑3 | `SkillStyle` của 723 | 14 (nổ vùng tức thì) | 0 (đạn 273 sống 1 khung) | không đáng kể — JX1 vẫn chạy đúng |
| TN‑4/7 | `skill_eventskilllevel` | không | thêm | vô hại (tương đương −1) |
| TN‑9 | 4 chỗ hỏng mã hoá trong `skill_desc` (Gate 1) | đúng | mất byte "bị", "có", "thể", "và" | chỉ mô tả trên giao diện |
| TN‑10 | `PreCastSpr` của 1236 | `spr/skill/门派进阶/火.spr` | `.spr/…` (thừa dấu chấm đầu) | client không hiện hiệu ứng vung chiêu của 1236; không thấy script/npc nào dùng 1236 |
| TN‑11 | `skill_skillexp_v` | thuộc tính idx 8, engine nạp | `KMagicDesc` JX1 **không có tên này** (326 vs 336 tên, lệch từ idx 8) → `ParseString2MagicAttrib` bỏ qua | JX1 tính exp kỹ năng qua `PlayerSet.m_cMagicLevelExp` (`KSkillList.cpp:248`), không qua bảng Lua; bảng `tianren120` hai bản giống nhau nên không lệch |
| TN‑12 | cửa `m_Level ≥ 120` cho `autodeathskill` | chưa đo | `LEVEL_EXPLOSIVE = 120` | người 120 mới học được nên không ảnh hưởng |

**Kết luận:** Thiên Nhẫn 120 ở JX1 **không thiếu gì** so với Linux về số liệu lẫn cơ chế; chỉ còn mô tả hỏng font (TN‑9) và dấu chấm thừa của 1236 (TN‑10).

### G.4 ⚠️ Phát hiện kèm: 5 kỹ năng `SkillStyle 14/15` đang **chết** trong JX1

Bảng nhảy Linux `0x0825843C`: `0` Missles · `1` (không) · `2` Initiative · `3` Passivity · `4` CreateNpc · `5‑13` không làm gì · **`14` → `0x080EAAD0`** (nổ vùng tức thì, G.2) · **`15` → `0x080ECBD8`** (biến thể trong `CastMissles`: đạn đặt tại vị trí mục tiêu). `switch` của JX1 (`KSkills.cpp:486‑566`) chỉ có `case 0..12`, **không `default`** ⇒ style 14/15 chỉ chạy `StartEvent` rồi trả `TRUE`.

`skills.txt` JX1 còn 5 dòng như vậy:

| Id | Tên | Style | Ai dùng |
|---|---|---|---|
| 876 | 魔音噬魄_BOSS专用 (Ma Âm Phệ Phách bản boss, Child 273) | 14 | **31 NPC**: Ngưu Ma Vương, Tiểu/Đại Xuân Ngưu, Thiết Ngưu Đại Vương, Lãnh Băng (+Đơn, Anh Hùng Tháp), Hoàng Nhan Quảng Dương, Lưu Tuấn, Tây Vực Phạn Hoàng, Thủ Lĩnh Kiếp Phỉ, Tiểu Đạo Tặc, Cửu Châu Thị Vệ, 4 Bào Quái Khách… |
| 1322 | Hư Nhược Vô Lực (`firofworld.lua`, `skill_enhance`) | 14 | sự kiện |
| 1406 | 超级魔音噬魄 (`boss_superskill.lua`, Child 409 bán kính 14) | 14 | boss |
| 1493 | 魔音噬魄_随机走动（单人）(`jianzhong.lua`) | 14 | Kiếm Trủng |
| 400 | (`kunlun.lua` `tianji_xunlei`, ReqLevel 30) | 15 | Côn Lôn |

Linux cũng có 721/722 style 14 — JX1 đã đổi thành 2; 723 → 0; 1545 → 3; **4 dòng còn lại bị bỏ sót**. ⇒ **Boss dùng 876 ở JX1 không bao giờ gây hoảng loạn.** Với 876/1406/1493 (đều Child 273/409 form 7) chỉ cần đổi dữ liệu `14 → 0` như 723 đã làm; 1322 và 400 cần xem riêng. **Chờ chủ game.**

### G.5 Cái Bang / Ngũ Độc 120 (đính chính TN‑1/2/5/6)

* **TN‑2 SAI**: `GetSkillLevelData(levelname, data, level)` (`advancedskill.lua:349`) tra `SKILLS[data][levelname]` **theo tên thuộc tính** → đổi vị trí cột `LvlSetting` **không đổi giá trị**, chỉ đổi thứ tự trong `m_StateAttribs`. Bốn thuộc tính kháng của `gaibang120zuzhou` hai bản **giống hệt**.
* **TN‑1 ĐÚNG, thuộc Cái Bang**: Linux `gaibang120zuzhou.fastwalkrun_p = {{1,−9},{23,−50}}` (**giảm tốc chạy nạn nhân 9→50 %**) + cột `LvlSetting6` của 720; JX1 thiếu cả bảng lẫn cột → quyết chú Cái Bang 120 ở JX1 **không làm chậm**; mô tả JX1 cũng bỏ câu "giảm tốc độ di chuyển".
* **TN‑5/6 vô hại**: `autoattackskillplus` **không có trong `KMagicDesc` cả hai bản** (Linux 0 chuỗi) → `GetSkillLevelData` trả `""` → `ParseString2MagicAttrib` không khớp tên → bỏ. Dữ liệu chết.
* `wudu120.skill_skillexp_v` JX1 khác Linux (180…789 321, thêm cấp 21‑28 = 1 910 800) — engine JX1 không đọc (TN‑11) → vô hại.
* `tianren.lua` ngoài 120: JX1 thiếu các thuộc tính `_yan_` (`fasthitrecover_yan_v`, `attackspeed_yan_v`, `lifemax_yan_p`) và mốc cấp 33‑43 của `tianmo_jieti` (Thiên Ma Giải Thể) — `KMagicDesc` JX1 có sẵn 22 tên `_yan_` nên chỉ là dữ liệu; `attackratingenhance_p` Thiên Ma Giải Thể cấp 30: Linux 600 vs JX1 500; `moren150` hoả 80→1500 (Linux) vs 45→1098 (JX1), `fatallystrike_p` 25 vs 30; JX1 đổi `addskillexp1` nhiều chiêu sang `{361, EXP_PER = 14}` (engine JX1 không có `addskillexp1` → vô hại).

---

## PHẦN H — VIỆC CÒN LẠI / CHỜ QUYẾT

> **07/09 — chủ game duyệt "hãy làm": mục 1‑4 ĐÃ THI CÔNG ở ĐỢT 4 bên dưới (phương án (b) cho AI 7‑10).**

1. **Bản vá AI 7‑10** (F.3): chọn (a)/(b)/(c) **trước khi swap** `CoreServer.dll.moi b43c85e8…`.
2. Có sửa dữ liệu 876/1406/1493 `SkillStyle 14 → 0` không (G.4).
3. 4 chỗ hỏng mã hoá `tianren.lua` + dấu `.` thừa của 1236 (`vn_edit.py`).
4. Cái Bang 120: thêm `fastwalkrun_p` (bảng + cột) nếu muốn như Linux.
5. Chưa đo: AI 21‑24 Linux; cửa cấp 120 của `autodeathskill` bên Linux; `0x0807A1F0` (AI7) tìm mục tiêu theo mẫu hay theo `Kind`.

---

# ĐỢT 4 (07/09 sáng) — THI CÔNG THEO PHẦN H (chủ game: "hãy làm")

## I.1 AI 7/8/9/10 dịch từ Linux — commit `68d7591b` (origin/main)

| Tệp | Thay đổi |
|---|---|
| `Sources/Core/Src/KNpcAI.h` | thêm 8 khai báo trong `#ifdef _SERVER`: `AI07_XungXa`, `AI08_ThachXa`, `AI09_HanhQuan`, `AI10_DungBan`, `AI_BoMucTieuLinux`, `AI_TimDichNgauNhien`, `AI_TimNpcTheoIdGan`, `AI_BanTaiCho` |
| `Sources/Core/Src/KNpcAI.cpp` | `Activate`: `case 7..10` gọi bốn hàm mới; thêm ~300 dòng thân hàm (toàn bộ trong `#ifdef _SERVER` ⇒ **CoreClient.dll không đổi**). Bốn hàm cũ `ProcessAIType7..10` giữ nguyên, không còn được gọi (giống `ProcessAIType1..6`) |
| `ReverseTools/goi_va_ai_7_10_linux_0709.py` | bộ vá (latin‑1, idempotent, sao lưu `*.truoc_ai710l_0709`) |

Đối chiếu từng bước với nhị phân (F.3):

* **AI07_XungXa** = `0x08094040`: `p0` là `m_dwID` của NPC mục tiêu (tra `NpcSet.SearchID`, phải cùng subworld, ở vùng hiện tại hoặc 1 trong 8 vùng kề, không ẩn — đúng `0x0807A1F0`/`0x080E1FD0`); chọn skill 1..4 theo `p1..p4` cộng dồn; `FollowAttack`. `p0 = 0` → không làm gì.
* **AI08_ThachXa** = `0x0808F1C0`: `p0` hoặc `p1` = 0 → thôi; skill theo `p3..p6`; `SendCommand(do_skill, skill, p0 − p2/2 + rand(p2), p1 − p2/2 + rand(p2))`; không bao giờ đi. Thêm chặn `rand(0)`.
* **AI09_HanhQuan** = `0x08092E30`: đúng thứ tự Linux (KeepActiveRange → chọn skill `p2/p3/p4`, thất bại → `CommonAction` → mục tiêu cũ & `m_CurrentVisionRadius ≠ 0` → `FollowAttack` (mất mục tiêu → bỏ) → `p0 %` tìm địch **ngẫu nhiên** trong tối đa 10 con (quét ô như `GetNpcNumber`, bỏ ẩn) → gốc := vị trí → `FollowAttack` → `p6 == 1` bám thủ lĩnh `Npc[p7]` id `p8` (gốc := vị trí thủ lĩnh, `FollowAttack` rồi **rơi tiếp** `CommonAction` như Linux) → đi tới `(p7,p8)` dừng cách `min(AttackRadius, VisionRadius)/2` theo bảng 64 hướng `g_DirCos/g_DirSin`). **Một cửa JX1 thêm**: `p6 ∉ {0,1}` hoặc `p7 = p8 = 0` → không hành quân, chỉ `CommonAction` — vì 50 mẫu trong `npcs.txt` (quan quân Tống Kim, Mộc nhân, Trụ ải…) mang `0|0` hoặc `20|50` từ thời AI9 cũ, dịch nguyên văn sẽ kéo chúng về góc bản đồ.
* **AI10_DungBan** = `0x08091EB0` + `0x0808F360`: giữ/tìm địch gần nhất (kiểm `m_dwID`, `dist² < Vision²`, không ẩn), skill `p1..p4` cộng dồn (hết → thôi), chỉ `do_skill` khi `dist² < m_CurrentAttackRadius²` và trong tầm nhìn. **Không bao giờ `do_walk`.**
* **AI_BoMucTieuLinux** = phần bỏ mục tiêu của nhịp `0x0808C640` (chết / hồi sinh / người chơi chưa bật chiến đấu).

Bỏ qua có chủ ý: kiểm hồi chiêu `0x080E4540` trước `do_skill` (JX1 `DoSkill` tự kiểm `CanCast`); `randmove` miễn AiMode 10 (F.5) — chưa làm.

## I.2 Dữ liệu trên cây chạy thật (sao lưu `*.truoc_sk120_0709`; **đã có hiệu lực cả hai phía**: máy chủ restart 00:56:55, client restart 00:57:02 ngày 07/09 — theo đính chính của phiên DELTA, `GameServer.exe` b98d9f31 và `CoreClient.dll` 5c359b16 cũng đã được áp, không còn `.moi` nào trong `bin/server` lẫn `bin/client`)

| Tệp (server và client) | Sửa |
|---|---|
| `settings/skills.txt` | 876 · 1406 · 1493 · 1322: `SkillStyle 14 → 0` (như 723 đã đổi từ trước); 1236: `PreCastSpr` bỏ dấu `.` đầu; 720: `LvlSetting6 = fastwalkrun_p`, `LvlData6 = gaibang120zuzhou` |
| `script/nhanvat/kynang/tianren.lua` (client: `script/skill/tianren.lua`) | 5 chỗ Gate‑1: `gần bị hoảng loạn` ×2, `chết có xác suất`, `không thể tấn công và di chuyển`, `hình thức thứ hai`, `tỷ lệ khiến cho đối phương khiếp sợ ` (thêm dấu cách trước `<color`) |
| `script/nhanvat/kynang/gaibang.lua` | `giá trị PTVL`, `giá trị KH`, `khi bị tấn công`, `có thể thi triển`; `gaibang120zuzhou` thêm `fastwalkrun_p={{{1,-9},{23,-50}},{{1,3*18},{15,8*18},{20,9*18},{21,9*18}}}` (đúng số Linux); mô tả thêm "giảm tốc độ di chuyển X%" |
| `script/nhanvat/kynang/wudu.lua` | `bị độc sát`, `có thể thi triển` |

Cách làm: `vn_edit.py --old/--new` (TCVN3), `safe_edit.py` cho dòng ASCII, Python sửa từng ô `skills.txt`; bản client là bản sao byte‑một của server (đã kiểm bằng `cmp` trước và sau).

Skill 400 (`SkillStyle 15`, Côn Lôn) **không đổi**: bảng nhảy Linux cũng chỉ tới 14 (`cmp ecx, 0xe`) — Linux cũng không chạy.

## I.3 Nhị phân

| Tệp | MD5 | Cỡ | Nguồn |
|---|---|---|---|
| `bin/server/CoreServer.dll` **ĐANG CHẠY từ 00:56 07/09** | `4b89f185` | 18 478 080 | **phiên DELTA** build 00:48 ở `D:/GAMEDEVNEW_wt_delta` từ main `68d7591b` + `delta-0709` (origin/main `61857a53`); `KNpcAI.obj` của worktree đó có `AI09_HanhQuan` ×12 ⇒ **gồm AI710L**; kèm `heaven.dll` 096fdeb2. Bản cũ `b43c85e8` thành `CoreServer.dll.truoc` |
| `CoreServer.dll.moi.matdo_a509a089_0037` | `a509a089bcc2c3e6a6102fc879c7103e` | 18 475 008 | phiên MATDO build 00:37 từ main `68d7591b` + MATDO (commit `ddb8b058`); `KNpcAI.obj` cây chính 00:36:58 có `AI09_HanhQuan` ×12 — đã bị DELTA thay, giữ làm sao lưu |
| `D:/GAMEDEVNEW_wt_ai710l/Sources/Core/x64/ServerRelease/CoreServer.dll` | `bcec43b20dd3a1e46783be7aaa56c0de` | 18 475 008 | bản của tôi, đúng `68d7591b`, thiếu MATDO và DELTA — chỉ dự phòng, **không dùng** |

Bản `b43c85e8…` (AI kiểu JX1, swap 23:15 06/09) chạy tới 00:50 07/09; **từ 00:56 07/09 máy chủ chạy `4b89f185`** (`ChayGameServer.bat` nuốt `.moi`, đổi bản cũ thành `.truoc`) — AI 7‑10 dịch từ Linux, `skills.txt` và 3 tệp Lua phía server đã có hiệu lực. Khe `.moi` lúc tôi kiểm còn trống; 00:37 phiên MATDO đặt bản gộp `a509a089`, 00:50 phiên DELTA thay bằng bản gộp `4b89f185` (giữ bản MATDO dưới tên `.moi.matdo_a509a089_0037`) — tôi không đặt bản nào, đúng luật khe dùng chung. Client **không cần** DLL mới (chỉ `skills.txt` + 3 tệp Lua).

Lùi: `git revert 68d7591b` + build lại; dữ liệu: đổi tên `*.truoc_sk120_0709` về tên gốc (6 tệp server + 6 tệp client). Lùi riêng AI 7‑10 mà vẫn giữ Linux‑AI khác: đổi 4 dòng `case` về `ProcessAIType7..10` (bản cũ vẫn còn).

## I.4 Cần chủ game kiểm sau restart

1. Tần Lăng / ải có "Tuyệt Sát", "Trụ", "Tri Thù Tơ", cung binh (mode 10): **đứng yên**, bắn khi người chơi vào tầm.
2. Vượt ải: "Trụ (ải 6/11)", "Tế Đài", "Quang Đoàn", "Mộc nhân" (mode 9): không tuần tra lung tung; NPC có skill sẽ đánh người chơi tới gần (50 %/nhịp) như Linux.
3. Boss dùng 876 (Ngưu Ma Vương, Lãnh Băng, Hoàng Nhan Quảng Dương, Tây Vực Phạn Hoàng…): giờ **có** gây hoảng loạn.
4. Cái Bang 120: hoại thương làm chậm mục tiêu 9–50 %; mô tả kỹ năng Thiên Nhẫn/Cái Bang/Ngũ Độc 120 hết ký tự `?`.
5. `jx_perf_server.log`: `TICK`/`SW_ACTIVATE` không tăng bất thường (AI9 quét ô tối đa 10 địch mỗi nhịp, tương đương `GetNpcNumber`).

---

# ĐỢT 5 (07/09 sáng) — VÕ ĐANG 120 "XUẤT Ứ BẤT NHIỄM": MỔ NHỊ PHÂN LINUX SO VỚI DỰ ÁN
> **CHỈ PHÂN TÍCH, KHÔNG SỬA GÌ.** Công cụ như đợt 3 (`ReverseTools/mo_nhi_phan_0609/`), thêm `linux_magic_handlers.txt` (336 tên thuộc tính phép ↔ địa chỉ handler, bảng `ProcessFunc` bước 8 byte gốc `+4`).

## J.1 Bộ kỹ năng (skills.txt hai bản: 114 cột, sai lệch rất nhỏ)

| Id | Tên | Vai trò | Khác biệt JX1 |
|---|---|---|---|
| **716** | Xuất Ứ Bất Nhiễm (`icon_sk_wa_120`) | `SkillStyle 0`, `TargetAlly + TargetSelf`, đạn **274** "Khu vực đồng đội" (`MisslesForm 7` tại chỗ, `CollidRange = DmgRange = 10`, `CanColFriend 1`, sống 1 khung, nổ), `StartEvent → 738`, `EventSkillLevel −1`; LvlSetting: `skill_cost_v`, `missle_missrate`, hồi chiêu (ngựa/bộ), `skill_desc`, `skill_skillexp_v`, `ignorenegativestate_p` | JX1 thêm `LvlSetting8 = skill_eventskilllevel/wudang120` (vô hại, như 715) và dấu `"` quanh `SkillDesc` |
| **738** | Kỹ năng phụ – Xuất Ứ Bất Nhiễm | chiêu con lên **chính mình**: đạn **275** (một viên, `CollidRange 1`), `ByMissle 1`; LvlSetting: `ignorenegativestate_p`, `missle_missrate` (bảng `wudang120_child`) | giống hệt |
| 899 | 出淤不染_BOSS专用 | bản boss của 716 (`ReqLevel 1`) | giống hệt; không NPC nào dùng ở cả hai bản |
| 1396 | 超级出淤不染 (`boss_superskill.lua`) | bản siêu boss | `AttackRadius` Linux 0 / JX1 400; không NPC nào dùng |
| 1521 | Linux: **Miễn dịch khống chế** (thụ động `SkillStyle 3`, `wudang120_child` + `statusimmunity_b`, `fatallystrikeres_p`, `freeze/poison/stuntimereduce_p`, `fasthitrecover_v`) | | **JX1 đã dùng lại id 1521 cho "摩诃无量boss"** (Thiếu Lâm, `biggoldboss.lua`) — hai bản khác hẳn; không NPC nào dùng ở cả hai bản, `statusimmunity_b` không có trong KMagicDesc JX1 |

## J.2 Dữ liệu cấp (`wudang.lua`, khoá `wudang120` / `wudang120_child`)

| Bảng | Linux | JX1 |
|---|---|---|
| `wudang120.skill_cost_v` | 15 → 50 | giống |
| `wudang120.missle_missrate` | **85 → 25 (c15) → 15** = 15 % → 75 % → 85 % đồng đội được tẩy | giống |
| `wudang120.ignorenegativestate_p` | `{1, 18 khung}` | giống |
| `wudang120.skill_mintimepercast(_onhorse)_v` | 35 s → 18 s (c15) → 15 s | giống |
| `wudang120.skill_eventskilllevel` | không | **thêm** `{{1,1},{20,20}}` (tương đương −1) |
| `wudang120.skill_skillexp_v` | 17,8 tr → 620 tr | 180 → 789 321 (+cấp 21‑28) — engine JX1 không đọc (TN‑11) |
| `wudang120.skill_desc` | đúng | **5 byte hỏng Gate 1**: `loại b?trạng thái d?thường`, `t?loại b?v?miễn dịch … d?thường` (server = client, byte‑một) |
| `wudang120_child.missle_missrate` | **85 → 20 (c15) → 15** | **85 → 25 (c15) → 15** — cấp 15 JX1 tự tẩy 75 % thay vì 80 % (cấp 1 và 20 như nhau) |
| `wudang120_child.ignorenegativestate_p` | `{100, 1 s → 3 s (c20)}` | giống |
| `clearnegativestate` | bị chú thích | bị chú thích (JX1 có tên thuộc tính 185, Linux không) |

⇒ **Số liệu hai bản trùng, trừ một mốc cấp 15 (75 % vs 80 %) và 5 byte hỏng mô tả.**

## J.3 Cơ chế trong engine — đây mới là chỗ khác nhau

### Linux (đo từ nhị phân)
1. **Trượt đòn** xử ở hàm va chạm đạn `0x080753F0`: `m_nMissRate (+0x154) > rand(100)` ⇒ log `"Missle nMissRate = %d%%, Miss!"`, **bỏ cả đòn** (không sát thương, không trạng thái). Trúng ⇒ `ReceiveDamage 0x0808A4A0` (tham số thứ 7 là **quan hệ** của chiêu, không có tham số miss‑rate) ⇒ nếu trả về khác 0 ⇒ áp danh sách trạng thái `0x08086260` ⇒ áp tức thì `0x0807D5A0`.
2. **`ignorenegativestate_p` là thuộc tính TRẠNG THÁI bình thường**, handler idx 201 = `0x080977D0`:
   * lúc áp (cờ tham số 1 = 0): gọi `0x080823B0(pNpc, 0, 1, 1, 0)` = duyệt `m_StateSkillList` (+0x234) **gỡ nút trạng thái do kỹ năng địch** (tương tự `IgnoreState(TRUE)` của JX1) và **đặt 0 hai ô `+0x1D8` và `+0x1E8`** (+0x1E8 là choáng — được ghi ngay sau khối tính tỷ lệ choáng `0x0808A8C5‑0x0808A941`; +0x1D8 là trạng thái khống chế kế bên, thời gian tính theo %);
   * luôn: `m_CurrentIgnoreNegativeStateP (+0x1474) += nValue[0]` (log `"IgnoreNegativeStateP + %d%% = %d%%"`); hết hạn thì nút trạng thái gỡ với giá trị đảo dấu ⇒ trở về 0.
3. **`+0x1474` được đọc ở 4 nơi**: (a) `ReceiveDamage 0x0808A8B6`: tỷ lệ choáng × (100 − ign)/100; (b) `ReceiveDamage 0x0808B21F`: ngay sau khi tính thời gian trạng thái `+0x1D8`, tung `rand(100)` so với ign ⇒ huỷ trạng thái đó; (c) **áp trạng thái `0x08086260` (0x08086C76‑0x08086CE8)**: nếu kỹ năng nhắm địch (`vfunc +0x2C`) và `ign > rand(100)` ⇒ **bỏ toàn bộ trạng thái của chiêu đó** (log `0x8254F68`); (d) **`DoHurt 0x0807F780`**: `hitrecover − anti ≥ 100` ⇒ không giật; `rand(100) ≤ 49` ⇒ không giật; **`ign > rand(100)` ⇒ không giật** (log `"IgnoreNegState(Hurt):%d%%, Hit!"`) — tức **miễn dịch còn chặn cả giật đòn**; (e) `GetPlayerMagicAttrib 0x08119060` (Lua đọc).

⇒ Ở Linux, Xuất Ứ Bất Nhiễm cho **mỗi đồng đội trong 10 ô** với xác suất `100 − missrate`: tẩy debuff + choáng/khống chế và +1 % (vô nghĩa) trong 1 s; **chính mình** với xác suất `100 − missrate_child`: tẩy + **miễn dịch 100 % trong 1→3 s** với mọi trạng thái từ chiêu địch, choáng, và giật đòn.

### JX1 (`KSkills.cpp:2756`, `KNpc.cpp:5094‑5140`, `KMissle.cpp:1410‑1444`)
1. `missle_missrate` → `KMissle::m_nMissRate` → **truyền vào `ReceiveDamage(…, nMissRate)`** (chữ ký khác Linux). **Không có "trượt cả đòn"**; miss‑rate chỉ dùng trong 3 chỗ: ô sát thương 15 (`ignorenegativestate_p`), ô 16 (`randmove` → `return FALSE`), và **nhánh `else if (g_RandPercent(nMissRate))` — xem J.4**.
2. `ignorenegativestate_p` được `ParseString2MagicAttrib` ghi vào **cả hai nơi**: `m_DamageAttribs[15]` (**JX1 tự viết thêm**) và, vì `nValue2 ≠ 0`, cũng vào `m_StateAttribs` (đường Linux).
3. Trúng đòn (`ReceiveDamage` 5096‑5115): ô 15 = ignoreneg ⇒ `g_RandPercent(100 − nMissRate)` ⇒ `ZeroMemory` **13 KState** (5 giáp bị phá, độc, băng, bỏng, choáng, say, ẩn, câm, hoảng loạn) + `IgnoreState(TRUE)` (gỡ nút debuff) + **`m_nTime_Ignorenegativestate = nValue[1]`**; trong lúc bộ đếm > 0, **mỗi lần bị đánh** lại xoá 13 KState + gỡ debuff (5123‑5140); bộ đếm giảm mỗi khung (1632).
4. Sau đó `ProcessDamage` áp `m_StateAttribs` **không tung xúc xắc** ⇒ `m_CurrentIgnoreNegativeStateP += nValue[0]` trong `nValue[1]` khung: 716 cho +1 %, 738 cho **+100 % trong 1→3 s, đúng 100 % số lần bấm chiêu** (Linux chỉ 15 %→85 % vì cả viên đạn có thể trượt).
5. `m_CurrentIgnoreNegativeStateP` được đọc ở **2 nơi**: `KMissle.cpp:1415` (bỏ trạng thái chiêu địch) và `KNpc.cpp:5069` (tỷ lệ choáng). **Không** có ở `DoHurt` (5149‑5154 chỉ `DoHurt% + do_hurt_p − anti_do_hurt_p`) và không có vòng tung thứ hai như Linux (b).
6. Điều kiện bỏ trạng thái ở `KMissle.cpp:1412` hẹp hơn Linux: chỉ khi **thuộc tính trạng thái ĐẦU TIÊN** của chiêu có `nValue[1] ≠ 0 && nValue[2] == 0` và chiêu là `Missles`/`InitiativeNpcState` nhắm địch. Chiêu địch mà thuộc tính đầu có 3 tham số (ví dụ `addphysicsdamage_p={{…},{…},{6}}` của Lịch Ma Đoạt Hồn / Nhiếp Hồn Loạn Tâm Thiên Nhẫn) **lọt qua miễn dịch** ở JX1; Linux chặn theo `IsTargetEnemy` không phân biệt.

### Bảng đối chiếu hành vi

| Điểm | Linux | JX1 | Ảnh hưởng |
|---|---|---|---|
| Xác suất cả đòn trượt theo `missle_missrate` | có, trước `ReceiveDamage` | **không có** | xem J.4 |
| Tẩy trạng thái đồng đội (716) | `100 − missrate` | `100 − missrate` | giống |
| Tẩy + miễn dịch bản thân (738) | tẩy **và** miễn dịch cùng xác suất `100 − missrate_child` | tẩy `100 − missrate_child`, **miễn dịch 100 %** | JX1 mạnh hơn: luôn có 1→3 s miễn dịch |
| Miễn dịch chặn trạng thái chiêu địch | mọi chiêu nhắm địch | chỉ khi thuộc tính đầu có 2 tham số | JX1 yếu hơn với chiêu 3 tham số |
| Miễn dịch chặn giật đòn (`do_hurt`) | có (0x0807F780) | không | JX1 yếu hơn |
| Miễn dịch vs choáng | tỷ lệ × (100 − ign) | giống | giống |
| Xoá KState khi tẩy | 2 ô (+0x1D8, +0x1E8) + gỡ nút debuff | 13 KState + gỡ nút debuff, **và lặp lại mỗi lần bị đánh** trong `nValue[1]` khung | JX1 tẩy rộng hơn |
| Tổng thể cho Võ Đang 120 | | | **JX1 không thiếu tính năng**; chênh lệch cân bằng hai chiều nhỏ |

## J.4 ⚠️ PHÁT HIỆN KÈM (engine JX1, ảnh hưởng chiêu có `missle_missrate` của phái KHÁC)

`KNpc.cpp:5117`:
```c
if (pTemp->nAttribType == magic_ignorenegativestate_p) { … }
else if (g_RandPercent(nMissRate))          // <-- ô 15 KHÔNG phải ignoreneg
{
    this->ClearNormalState();               // xoá 13 KState (kể cả giáp bị phá, độc, băng, bỏng, choáng, hoảng loạn)
    this->IgnoreState(TRUE);                // gỡ mọi nút trạng thái do kỹ năng địch
    m_nTime_Ignorenegativestate = pTemp->nValue[1];   // ô 15 rỗng -> 0
}
```
Nhánh này **không có bên Linux** (Linux không truyền miss‑rate vào `ReceiveDamage`). Hệ quả: mọi chiêu có `missle_missrate` mà ô 15 không phải `ignorenegativestate_p` sẽ **tẩy debuff cho NẠN NHÂN** với xác suất = missrate:

| Chiêu (JX1) | missrate | randmove? | Hành vi Linux | Hành vi JX1 |
|---|---|---|---|---|
| 723 Ma Âm Phệ Phách – Hoảng loạn (+876/1406/1493 boss) | 65 → 15 % | có | trượt cả đòn 65→15 % | **tẩy debuff nạn nhân 65→15 %** + tung riêng "trượt" 65→15 % (ô 16) |
| 1131 Ma Âm Kích (Thiên Nhẫn 150, `zhanren150`) | **99 → 80 %** | có | trượt 99→80 % | **tẩy debuff nạn nhân 99→80 % mỗi đòn trúng** + tung riêng trượt |
| 1190 Bộc phát Mê Túy Thiên Hương (vật phẩm) | theo `zhandan_randmove` | có | trượt | tẩy + trượt |
| 1201 Vi Đà Hộ Pháp (Thiếu Lâm 150) | Linux 99 → 50 %; **JX1 bảng `gunshaolin150` không có dòng `missle_missrate`** ⇒ `m_nMissRate = 0` | không | trượt 99→50 % | không bao giờ trượt, không tẩy |

Nghĩa là ở JX1, **Thiên Nhẫn đánh chiêu 120/150 lại giúp đối thủ sạch debuff** (kể cả debuff của chính Thiên Nhẫn vừa áp), và Vi Đà Hộ Pháp không trượt như Linux. Khuyến nghị (chờ chủ game): làm theo Linux — (1) `KMissle::ProcessDamage` tung `m_nMissRate` một lần, trượt ⇒ bỏ cả đòn; (2) ô 15 tẩy **không** tung lại; (3) bỏ nhánh `else if`; (4) bỏ tung ở ô 16 (`randmove`) vì (1) đã lo; (5) thêm `missle_missrate` cho `gunshaolin150` nếu muốn Vi Đà Hộ Pháp như Linux. Đây là thay đổi PvP, cần chủ duyệt trước.

## J.5 Việc nhỏ có thể sửa ngay (chưa làm)
* 5 byte hỏng trong `wudang120.skill_desc` (server + client, `vn_edit.py`).
* `wudang120_child.missle_missrate` cấp 15: 25 → 20 nếu muốn khớp Linux.
* Không cần sửa SkillStyle: 716/738/899/1396 đều `SkillStyle 0` ở cả hai bản, cơ chế đạn 274/275 giống nhau.


---

# PHẦN K (07/09 rạng sáng) — THI CÔNG "fix toàn bộ và kiểm tra còn skill lệch bản Linux thì làm lại như Linux"

Phạm vi đã chọn (chủ game chưa nêu rõ): **họ kỹ năng 120 của 10 phái** (112 dòng bên Linux / 106 bên JX1: kỹ năng chính 708‑723, quyết chú 724‑725/738, bản BOSS 876/899/1396/1405/1406/1493, bảng `*120*` trong Lua) + toàn bộ lỗi engine của Phần J. Lệch ngoài họ 120 (331 dòng `skills.txt`, ~250 khoá Lua) chỉ **liệt kê** ở K.4, chờ chủ quyết.

## K.1 Engine — commit `33b1dfa4` origin/main (server only, `CoreClient.dll` không đổi)

Bộ vá `ReverseTools/goi_va_sk120_linux_0709.py` (idempotent, `--check`), 8 hunk, mỗi hunk chú thích địa chỉ Linux tương ứng:

| # | Tệp / hàm | JX1 cũ | Sau vá (= Linux) | Linux |
|---|---|---|---|---|
| H1 | `KMissle.cpp` vòng quét va chạm (trước `nRet++`) | không có khe trượt; `m_nMissRate` chỉ chui vào `ReceiveDamage` | `m_nMissRate > 0 && g_RandPercent(m_nMissRate)` ⇒ `continue`: **bỏ cả đòn** (không sát thương, không trạng thái, không trừ `m_nHitCount`) | `0x080753F0`‑`0x08075600`: `missrate > rand(100)` ⇒ log `Missle nMissRate = %d%%, Miss!`, trả 0 |
| H2 | `KMissle::ProcessDamage` | `if (g_RandPercent(ign)) return TRUE;` bên trong điều kiện hẹp (`nValue[1] && !nValue[2] && style 0/2 && IsTargetEnemy`), bỏ luôn cả `SetImmediatelySkillEffect` | bỏ dòng này (chuyển sang H8) | — |
| H3 | `KNpc::ReceiveDamage` ô 15 `ignorenegativestate_p` | tẩy 13 KState với xác suất `100 − nMissRate` | tẩy **không tung** (khe trượt đã tung ở H1) | handler 201 `0x080977D0` |
| H4 | `ReceiveDamage` | `else if (g_RandPercent(nMissRate)) { ClearNormalState(); IgnoreState(TRUE); …}` — **tẩy sạch debuff của NẠN NHÂN** mỗi khi chiêu có `missle_missrate` mà ô 15 không phải ignoreneg (723/876/1406/1493 Ma Âm Phệ Phách 65 → 15 %, 1131 Ma Âm Kích 99 → 80 %, 1190…) | xoá nhánh | `0x0808A4A0` không đọc nMissRate |
| H5 | `ReceiveDamage` ô 16 `randmove` | `nValue[0] && nValue[1] && g_RandPercent(nMissRate) ⇒ return FALSE` | xoá (khe trượt đã tung ở H1) | — |
| H6 | `KNpc::DoHurt` | không có khe miễn dịch | sau khe cơ bản: `ign > 0 && g_RandPercent(ign)` ⇒ không thọ thương | `0x0807F821`‑`0x0807F85C` `IgnoreNegState(Hurt)` |
| H7 | `ReceiveDamage` băng (ô 10) | không có khe miễn dịch | ngay sau khi đặt `m_FreezeState.nTime`: `g_RandPercent(ign)` ⇒ `nTime = 0` | `0x0808B21F`‑`0x0808B273` `IgnoreNegState(Freeze)` |
| H8 | `KNpc::SetStateSkillEffect` (sau `GetSkill`) | — | `ign > 0 && nTime != 0 && pSkill && pSkill->IsTargetEnemy() && g_RandPercent(ign)` ⇒ `return` (không áp trạng thái nào, không đồng bộ client); `#ifdef _SERVER` | `0x08086410`‑`0x08086D13` (`忽略负面状态(魔法):%d%%, 忽略!`), chỉ xét `IsTargetEnemy` (vfunc `+0x2C`) |

Hệ quả cho người chơi:

| Tình huống | JX1 cũ | Sau vá |
|---|---|---|
| Thiên Nhẫn 723 (và 876/1406/1493) trúng địch | luôn trúng; xác suất `missrate` **tẩy debuff nạn nhân**; hoảng loạn áp với xác suất `100 − missrate` | trượt `missrate` (65 % c1 → 15 % c20) = không gì xảy ra; trúng = sát thương + hoảng loạn; không còn tẩy nạn nhân |
| Võ Đang 738 Xuất Ứ Bất Nhiễm (đạn tự thân) | tẩy `100 − missrate_child` nhưng **miễn dịch +100 % luôn áp** | tẩy **và** miễn dịch cùng xác suất `100 − missrate_child` (15 % c1 → 85 % c20) |
| Võ Đang 716 lên đồng đội | tẩy `100 − missrate` | giữ |
| Nạn nhân đang có ign% bị chiêu nhắm địch mang trạng thái (Cái Bang 720 làm chậm, Thúy Yên băng, Ngũ Độc độc‑trạng‑thái, Đường Môn…) | tung ign chỉ khi `nValue[1] && !nValue[2] && style 0/2` | tung ign cho **mọi** chiêu `IsTargetEnemy`; các thuộc tính tức thời vẫn áp |
| Thọ thương / băng / choáng khi có ign% | choáng đã nhân `(100 − ign)`; thọ thương, băng không có khe | cả ba đều có khe như Linux |

Cố ý **giữ khác Linux** (ghi để chủ quyết): `m_nTime_Ignorenegativestate` — sau khi 738 tẩy, JX1 zero 13 KState ở **mỗi** `ReceiveDamage` trong 1 → 3 s (kể cả độc, bỏng, phá giáp, ẩn, câm, hoảng loạn); Linux chỉ có `ign = 100 %` trong cùng thời gian, tức chỉ chặn choáng/băng/thọ thương/trạng thái phép, **không** chặn độc/bỏng/phá giáp qua ô sát thương. Muốn giống hệt: xoá khối `if (m_nTime_Ignorenegativestate > 0)` (KNpc.cpp ô 15) — một dòng, chưa làm vì đụng cảm giác "bất khả xâm phạm" của Võ Đang 120.

Kiểm mã hoá: `KNpc.cpp` high‑byte 2160 → 2156 (xoá đúng dòng chú thích `// 3 giây` trong nhánh H4), `KMissle.cpp` 638 → 638, FFFD = 0 cả hai. Build "Server Release|x64" worktree `D:\GAMEDEVNEW_wt_ai710l` từ origin/main: 0 lỗi, link thật; DLL chứa `[SK120-MISS]`, `[SK120-IGNSTATE]`.

## K.2 Dữ liệu (server **và** client, sao lưu `*.truoc_sk120b_0709`; gương `serverscript_live` đã cập nhật cùng commit)

| Tệp | Sửa | Linux |
|---|---|---|
| `settings/skills.txt` (sửa từng ô theo tên cột vì hai bản khác cấu trúc từ dòng 1602) | 713 Ngự Tuyết Ẩn: `LvlSetting7 = fastwalkrun_p`, `LvlData7 = cuiyan120`; 709 `LvlData5` (rác `dachengrulaizhou`, không có LvlSetting5) → rỗng; 717 `LvlData4` (rác) → rỗng | y hệt |
| `cuiyan.lua` `cuiyan120` | thêm `fastwalkrun_p={{{1,1},{20,30},{21,30}},{{1,5*18},{15,25*18},{20,30*18},{21,30*18}}}` — **Thúy Yên 120 chạy nhanh +1 → +30 % trong lúc ẩn** (JX1 hoàn toàn thiếu, cả bảng lẫn skills.txt) | chú thích Linux `20141013` |
| `kunlun.lua` | `kunlun120.autorescueskill` c15: 45 % → **60 %**; `kunlun120jiasu.fastwalkrun_p` thời gian 2/4/5/5 s → **3/9/10/10 s**; `kunlun120mofadun.staticmagicshield_p` 2/4/5/5 s → **5/9/10/10 s** (JX1 đã cắt nửa; mô tả tự tính theo bảng) | y hệt |
| `wudang.lua` | `wudang120_child.missle_missrate` c15 25 → **20**; 5 byte `?` hỏng ở mô tả 716/738 (`loại bỏ trạng thái dị thường`, `tự loại bỏ và miễn dịch …`) theo nguyên văn Linux dòng 429‑430 | y hệt |
| `shaolin.lua` `gunshaolin150` | thêm `missle_missrate={{{1,99},{20,50}}}` (1201 Vi Đà Hộ Pháp). **Lưu ý**: 1056 Vi Đà Hiến Xử bên JX1 thiếu `skill_startevent/showevent/eventskilllevel/anti_block_rate`, MaxLevel 20 (Linux 26) ⇒ 1201 hiện **không bao giờ được bắn** ở JX1 — thuộc họ 150, chưa đụng | y hệt |

`kiem_54.py` 4 tệp: 0 lỗi cú pháp 5.4. Client: `cuiyan/wudang/shaolin.lua` byte‑một với server; `kunlun.lua` chỉ khác dòng 176 `manareplenish_v` có sẵn từ trước (server −5/−70, client −1/−20 — của phiên khác, không đụng).

## K.3 Rà họ 120 — còn lệch và quyết định

| Mục | Linux | JX1 | Quyết định |
|---|---|---|---|
| 721/722 Côn Lôn con `SkillStyle` | 14 (bùng tức thời `0x080EAAD0`) | 2 (`CastInitiativeSkill`) | giữ 2: JX1 không có style 14; với chiêu tự thân, 2 là tương đương (723 loại nhắm địch đã đổi về 0) |
| 710 Mê Ảnh Tung `CharAnimId` | 11 | 0 (từ gốc JX1) | giữ, **hỏi chủ**: động tác thi triển trước khi dịch chuyển |
| 715/716/721 + `tianren120/wudang120/kunlun120mofadun.skill_eventskilllevel` | không | có | JX1 thêm, vô hại, giữ |
| 711/714 `autoattackskillplus = wudu120/gaibang120` | không | có trong skills.txt | **khoá không tồn tại** trong bảng Lua cả hai bản ⇒ tham chiếu chết, vô hại |
| 720 thứ tự `physicsres/resmax` | 1‑2‑4‑5 | hoán vị | vô hại (cùng khoá, tính sau khi cộng) |
| 1178‑1181/1192/1237/1239/1282/1294/1313‑1316/1318/1392/1393/1427/1428 `*_yan_*` | có | đổi sang bản không `_yan_` | JX1 bỏ hệ Âm/Dương (cả 331 dòng ngoài họ 120 cũng vậy) — giữ |
| 1396 `AttackRadius` 0 → 400, 1405 `StartSkillId` 1366 → 1406, 1427/1428 script boss khác + mất `meleedamagereturn/rangedamagereturn` | | | boss riêng JX1 — giữ, báo |
| 1521 "Miễn dịch khủng cụ" (npc, `wudang120_child`), 1545 "Ma Âm Phệ Phách_Di chuyển ngẫu nhiên (đơn)" | có | **id bị dùng lại**: 1521 = 摩诃无量boss (`biggoldboss.lua`), 1545 = Ngũ Hành Tý Hộ (`guta.lua`); bản đơn của 1545 được JX1 tạo lại ở **1493** | không thể phục hồi cùng id; 1521 Linux chỉ 1 NPC dùng — nếu cần thì cấp id mới |
| `emei120..emei120_4.skill_appendskill` cấp phụ `{20,40}` | | `{20,20}` | JX1 engine **không đọc** thành phần cấp (`KNpc.cpp:1421/10890` chỉ lấy id; cấp = cấp kỹ năng người chơi ≤ cấp 120) — không đổi |
| 1618‑1628 至尊印鉴 (Chí Tôn Ấn Giám, 11 phái, `zhizunyinjian.lua`, ReqLevel 120) | có | không | hệ mới (cần LvlSetting `dec_pskill_cdtime`, `dec_percasttime`, `enhance_709_auto`, `movedistanc_710_enhance`, `validtiem_1366_enhance`, `daoxutian_enhance`) — ngoài phạm vi |
| chỉ JX1: 1493, 1984 Trung Vũ Lưu Phong, 2127/2128 Tiêu Dao, 2132‑2134 Hoa Sơn | | | phái mới JX1 |
| `skill_skillexp_v` (wudang120/wudu120/…150) | khác số | | bảng exp — giữ JX1 |

## K.4 Ngoài họ 120 (chưa đụng — chờ chủ chọn phạm vi)

`ReverseTools/mo_nhi_phan_0609/audit_skills_diff.txt` (331 dòng `skills.txt` lệch cột cơ chế; 64 id chỉ Linux, 119 chỉ JX1) và `audit_lua_diff.txt` (khoá Lua lệch theo tệp). Nhóm chính: (1) bỏ `_yan_` hàng loạt; (2) JX1 thêm `seriesdamage_p`/`stun_p`…; (3) đổi `DoHurt`/`AttackRadius`/`ChildSkillNum`/`MaxLevel` (vd 1056 Vi Đà Hiến Xử 26 → 20 và mất sự kiện 1201; 353/355 Thiên Nhẫn; 359/128/165); (4) bảng số kỹ năng thường (xueying, huabu_liushou, qingfeng_fu, jingxin_jue, tianmo_jieti, longzhao_huzhua, rulai_qianye, yijin_jing, wangu_shixin, xuanyin_zhan…). Phần lớn có dấu hiệu **chỉnh cân bằng có chủ ý** của JX1, không nên đổi mù.

Phát hiện thêm khi mổ `SetStateSkillEffect` Linux: hai khe "bỏ qua bùa chú" (724) / "phản đòn bùa chú" (725) nằm **trong** hàm này, điều kiện là hai bảng cờ theo skill id (`0x8fc2420`, `0x8fc04e0`) chứ không phải `nValue[1] && !nValue[2] && style 0/2` như JX1 — chưa giải hai bảng cờ, JX1 giữ điều kiện cũ (mục mở).

## K.5 Nhị phân + kiểm sau restart

| Tệp | MD5 | Cỡ | Ghi chú |
|---|---|---|---|
| `bin/server/CoreServer.dll.moi.sk120_1a33f617_0318` | `1a33f617` | 18 479 104 | origin/main `33b1dfa4` (= `bc0fffc3` của DELTA + đợt này). Khe `.moi` lúc build đang là `6246967d` (DELTA 01:42, trước push của tôi) — xem phần trạng thái cuối để biết bản nào đang ở khe |

Lùi: `git revert 33b1dfa4`; dữ liệu: đổi tên `*.truoc_sk120b_0709` về tên gốc (5 tệp server + 5 tệp client).

Kiểm sau restart máy chủ + client:
1. Thiên Nhẫn 120 (723) đánh mộc nhân/người: có lần **trượt hẳn** (không sát thương, không hoảng loạn); nạn nhân đang mang debuff (chậm, độc) **không** còn được tẩy khi trúng 723.
2. Võ Đang 120 bấm 738 nhiều lần ở cấp thấp: có lần **không** tẩy và **không** có biểu tượng miễn dịch (trước đây luôn có miễn dịch).
3. Võ Đang đang miễn dịch bị Cái Bang 120 / Thúy Yên đánh: không bị làm chậm/đóng băng, không thọ thương (log `[SK120-IGNSTATE]`, `[SK120-MISS]` trong AUTOLOG).
4. Thúy Yên 120 ẩn thân: chạy nhanh hơn 1 → 30 %.
5. Côn Lôn 120: Ma Pháp Đôn 5 → 10 s, Gia Tốc 3 → 10 s, cứu nguy cấp 15 = 60 %.
6. Mô tả 716/738 hết ký tự `?`.

**Trạng thái khe `.moi` lúc 03:25 07/09**: `bin/server/CoreServer.dll.moi` = **`1a33f617`** (đợt này; superset của bản DELTA vì build từ origin/main `33b1dfa4` = `bc0fffc3` của DELTA + đợt này, cùng cỡ 18 479 104); bản DELTA giữ ở `CoreServer.dll.moi.delta_6246967d_0142`; đang chạy vẫn `4b89f185` (00:50). Hai phiên DELTA/MATDO đã được nhắn. Chờ chủ chạy `ChayGameServer.bat` + `ChoiGame.bat` (client chỉ cần dữ liệu, không cần DLL mới).
