# MỔ NHỊ PHÂN BẢN LINUX — QUÁI DI CHUYỂN & ĐẠN SKILL TRUY ĐUỔI
> 06/09/2026 · **CHỈ PHÂN TÍCH, KHÔNG SỬA GÌ**
> Nguồn đo: `D:\ServerLinux\server1\jx_linux_y` (ELF32 i386, không section header, 8,9 MB)
> Đối chiếu: `D:\GAMEDEVNEW\Sources\Core\Src\*` + `E:\SourceTuanLe\...\bin\server\settings\*`
> Công cụ dựng trong phiên: `…\scratchpad\quaidan\{elfre.py, loader.py, findfn.py, dec.py, mis.py}`

---

## 0. BẢN ĐỒ CẤU TRÚC ĐO ĐƯỢC TỪ NHỊ PHÂN LINUX

| Toàn cục | Địa chỉ | Ghi chú |
|---|---|---|
| `Npc[]` | `0x0836EAE0` | `sizeof(KNpc)` = **0x1A4C** (6 732 B) |
| `Missle[]` | `0x0836EA50` | |
| `SubWorld[]` | `0x08FC81E0` | `sizeof(KSubWorld)` = **0x63FC8**; `m_dwCurrentTime` ở `+0x38` |
| `sizeof(KRegion)` | `0xCC4` | |

### KNpc (Linux) — các trường AI
| Offset | Trường (theo tên JX1) | Bằng chứng |
|---|---|---|
| `+0x24` | `m_Kind` | so với 1 (player) / 3 (dialoger) |
| `+0x224` | `m_Doing` | so với 10 (`do_death`) và 21 (`do_revive`) = đúng `IsAlive()` của JX1 |
| `+0x240` | `m_ActiveSkillID` | tham số của `SendCommand(do_skill…)` |
| `+0x1180 / +0x1184` | `m_SubWorldIndex` / `m_RegionIndex` | nhân `0x63FC8` / `0xCC4` |
| `+0x148C/90/98/9C` | `m_MapX/m_MapY/m_OffX/m_OffY` | 5 tham số vào `Map2Mps` |
| `+0x1288` | bán kính thân (dùng cho khoảng cách tối thiểu) | |
| `+0x12A8` | `m_CurrentAttackRadius` | |
| `+0x12AC` | `m_CurrentActiveRadius` | |
| `+0x1594` | `m_nPeopleIdx` (địch đang khoá) | |
| `+0x14B4 / +0x14B8` | `m_OriginX / m_OriginY` | điểm sinh — tâm vùng hoạt động |
| `+0x14BC` | `m_NextAITime` | |
| `+0x14C0` | `m_AIMAXTime` (**BYTE**) | |
| `+0x1630` | `m_VisionRadius` | |
| `+0x1638` | `m_ActiveRadius` | |
| `+0x1654` | `m_AiMode` | `SetNpcAI` (Lua) ghi thẳng vào đây |
| `+0x1658` | `m_AiParam[11]` | nhánh xa đọc `[edi+0x28]` = `pAIParam[10]` = `MAX_AI_PARAM-1` ✔ |
| `+0x1A14 / +0x1A18` | cặp máu tối đa | cổng `if (max == 0) return` |

### Hàm KNpcAI (Linux)
| Hàm (tên JX1) | Địa chỉ |
|---|---|
| `KNpcAI::Activate` | **0x08095780** (bảng nhảy `0x08255340`, 25 mục) |
| `ProcessAIType01` | **0x08093E30** |
| `CommonAction` (tuần tra) | **0x0808F480** |
| `KeepActiveRange` (dây xích) | **0x0808F700** |
| `FollowAttack` | **0x08092A60** |
| `KeepAttackRange` | **0x0808F820** |
| `InEyeshot` | **0x08095910** |
| `GetNearestNpc` | **0x0808ED10** |
| Cập nhật nhịp AI + bỏ mục tiêu chết | **0x0808C640** |
| `SendCommand` | `0x08078AA0` · `g_Random` `0x08226AD0` · `GetDistanceSquare` `0x0809E660` · `Map2Mps` `0x080EF710` |

---

## 1. QUÁI DI CHUYỂN TRÊN MAP LUYỆN CÔNG — BẢN LINUX

### 1.1 Cổng nhịp (`KNpcAI::Activate`, 0x08095780)

```c
p = &Npc[nIndex];
if (p->m_Kind == kind_player) { ProcessPlayer(); return; }
if (p->m_nPeopleIdx == nIndex) p->m_nPeopleIdx = 0;          // tự khoá mình -> huỷ
if (max(p->[+0x1A14], p->[+0x1A18]) == 0) return;            // chưa có máu tối đa
if (SubWorld[p->m_SubWorldIndex].m_dwCurrentTime < p->m_NextAITime) return;
if (p->m_AiMode <= 24) goto jumptable[0x08255340][p->m_AiMode];
```

**Bảng nhảy 25 mục (AIMode 0..24)** — đây là điểm khác cấu trúc lớn nhất so với JX1:

| AIMode | Linux | JX1 (`KNpcAI::Activate`) |
|---|---|---|
| 0 | không AI | không AI |
| **1** | `0x08093E30` | `ProcessAIType01()` |
| 2 | `0x08093B00` | `ProcessAIType02()` |
| 3 | `0x08093850` | `ProcessAIType03()` |
| 4 | `0x080936E0` | `ProcessAIType04()` |
| 5 | `0x08093490` | `ProcessAIType05()` |
| 6 | `0x08093290` | `ProcessAIType06()` |
| **7** | `0x08094040` | **bị chú thích** (`/* case 7: ProcessAIType7 */`) |
| **8** | `0x0808F1C0` | **bị chú thích** |
| **9** | `0x08092E30` | **bị chú thích** |
| **10** | `0x08091EB0` | **bị chú thích** |
| 11–20 | không AI trên máy chủ (là 11..17 kiểu "diễn" của client) | như vậy |
| **21–24** | `0x080956B0 / 0x08094350 / 0x08094D80 / 0x08094170` | JX1 **không có trong switch**; dự án tự thêm nhánh riêng `kind_partner` → `Partner_ProcessAI` (bạn đồng hành, 4 tính cách 21..24) |

`npcs.txt` bản Linux đang thật sự dùng AIMode **9 (50 con), 10 (51 con), 12 (3 con), 15 (6 con)**, `npcs.txt` của JX1 cũng có **9 (50), 10 (62), 12 (3), 15 (5)** → **JX1 nạp những con quái này nhưng engine không chạy AI của chúng** (case 7–10 bị chú thích, 11–20 rỗng) ⇒ chúng đứng im.

### 1.2 AI 1 — quái thường (1 064/2 352 dòng ở Linux, 1 245/2 642 ở JX1)

`0x08093E30`, dịch ra ngôn ngữ nguồn, **thuật toán trùng khít `KNpcAI::ProcessAIType01` của JX1**:

```c
CapNhatNhipAI(this);                       // 0x0808C640  <-- JX1 làm việc này trong Activate
if (KeepActiveRange()) return;             // vượt bán kính -> đi bộ về gốc, hết lượt

e = Npc[me].m_nPeopleIdx;
if (e <= 0 || Npc[e].m_dwID == 0
    || GetDistanceSquare(me,e) >= m_VisionRadius²
    || KiemTraAn(&Npc[e], me) != 0)        // 0x08079200  <-- JX1 KHÔNG có
{
    e = GetNearestNpc(relation_enemy /*8*/);
    Npc[me].m_nPeopleIdx = e;
}

if (e <= 0) {                              // không có địch quanh
    if (pAIParam[0] > 0 && g_Random(100) < pAIParam[0]) CommonAction();   // tuần tra
    return;
}
if (GetDistanceSquare(me,e) > pAIParam[10]) {          // địch ở XA
    r = g_Random(100);
    if (r < pAIParam[5]) return;                       // đứng yên
    if (r < pAIParam[5]+pAIParam[6]) { CommonAction(); return; }   // tuần tra
    FollowAttack(e); return;                           // tiến tới
}
r = g_Random(100);                                     // địch trong tầm
if      (r < p1)          { if (!SetActiveSkill(1)) { CommonAction(); return; } }
else if (r < p1+p2)       { if (!SetActiveSkill(2)) { CommonAction(); return; } }
else if (r < p1+p2+p3)    { if (!SetActiveSkill(3)) { CommonAction(); return; } }
else if (r < p1+p2+p3+p4) { if (!SetActiveSkill(4)) { CommonAction(); return; } }
else return;
FollowAttack(e);
```

### 1.3 `CommonAction` — chính là "cách quái đi lang thang" (0x0808F480)

```c
if (Npc[me].m_Kind == kind_dialoger) { SendCommand(do_stand); return; }
nOffX = nOffY = 0;
if (g_Random(100) > 79)                                 // 20 % mới nhúc nhích
{
    nOffX = g_Random(m_CurrentActiveRadius / 2);
    nOffY = g_Random(m_CurrentActiveRadius / 2);
    if (nOffX & 1) nOffX = -nOffX;                      // bit 0 quyết định dấu
    if (nOffY & 1) nOffY = -nOffY;
}
SendCommand(do_walk, m_OriginX + nOffX, m_OriginY + nOffY);
```

Tức là: **80 % số lần "tuần tra" là lệnh đi bộ về đúng điểm sinh**, 20 % là một điểm ngẫu nhiên trong nửa bán kính hoạt động. Đây là toàn bộ chuyển động lúc quái rảnh.

### 1.4 `KeepActiveRange` — dây xích (0x0808F700)

```c
Map2Mps(...) -> (x,y);
d2 = (m_OriginX-x)² + (m_OriginY-y)²;   R = m_ActiveRadius;
if (d2 > R*R)              m_CurrentActiveRadius = R/2;      // thu vùng để khỏi đảo mép
if (d2 > m_CurrentActiveRadius²) { SendCommand(do_walk, m_OriginX, m_OriginY); return TRUE; }
m_CurrentActiveRadius = R;  return FALSE;
```
Giống hệt JX1, chỉ khác **JX1 dùng khoảng cách có căn (`g_GetDistance`), Linux dùng bình phương** (không gọi `sqrt`).

### 1.5 `FollowAttack` — đuổi & đánh (0x08092A60)

```c
if (Npc[i].m_RegionIndex < 0) return;
if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive) return;   // == CheckNpc
d2 = GetDistanceSquare(me, i);
if (d2 <= 1024 /* 32² */) { KeepAttackRange(i, Npc[me].[+0x1288] + 32); return; }  // ★
if (d2 <  m_CurrentAttackRadius²) {
    if (InEyeshot(i)) { SendCommand(do_skill, m_ActiveSkillID, -1, i); return; }
}
Map2Mps(me) ; Map2Mps(i) ; ... ; SendCommand(do_walk, <phía địch>);
```

★ **Đây là điểm khác biệt hành vi rõ nhất**: bản Linux còn giữ khối "giữ khoảng cách tối thiểu 32" — quái áp sát quá thì **lùi ra** cho đủ `bán_kính_thân + 32`. Trong JX1 khối này **đang bị chú thích** (`KNpcAI.cpp:1180-1186`, `//#define MINI_ATTACK_RANGE 32`) ⇒ quái JX1 dán sát vào người chơi.

### 1.6 Lệnh mà AI phát ra

Quét toàn module AI (`0x0808E000–0x08096000`) các lời gọi `SendCommand`:
`do_stand(1)` ×1 · `do_walk(2)` ×6 · `do_skill(5)` ×6 — **không có `do_run(3)`**.
⇒ Ở **cả hai bản**, quái chỉ **đi bộ**; cột `RunSpeed` trong `npcs.txt` không được AI dùng tới.

---

## 2. KHÁC BIỆT QUÁI DI CHUYỂN — LINUX vs DỰ ÁN JX1

### KB-1 (LỚN NHẤT, thuộc **dữ liệu**): `AIMaxTime` — nhịp suy nghĩ của quái

`m_dwCurrentTime` là **bộ đếm khung hình** (`KSubWorld.cpp:1107  m_dwCurrentTime++`), nhịp 18 khung/giây.
`m_NextAITime = m_dwCurrentTime + m_AIMAXTime` ⇒ `AIMaxTime` = **số khung giữa hai lần quái ra quyết định**.

| | Linux (JX2) | Dự án JX1 |
|---|---|---|
| Cột `AIMaxTime` trong `npcs.txt` | **CÓ** | **KHÔNG CÓ** |
| Engine đọc | có | có — `KNpcTemplate.cpp:136`, `GetInteger(…, "AIMaxTime", 25, …)` |
| Giá trị thực tế | 36 (997 con) · 6 (459) · 3 (396) · 9 (181) · 12 (174) · 10 (41) · 18 (25) · 24 (5) | **25 cho TẤT CẢ** (rơi về mặc định) |

Tổ hợp phổ biến nhất của quái `AIMode=1` bên Linux:

| AIMaxTime | Vision | Active | Walk/Run | số con |
|---|---|---|---|---|
| **3** (0,17 s) | 1800 | 2000 | 12/12 | 95 |
| **9** (0,50 s) | 400 | 700 | 6/6 | 75 |
| **6** (0,33 s) | 800 | 1000 | 6/6 | 47 |
| **12** (0,67 s) | 400 | 800 | 6/12 | 30 |
| **36** (2,0 s) | 400 | 700 | 6/6 | 29 |

Hệ quả: quái ở map luyện công bản Linux ra quyết định nhanh gấp **2–8 lần** JX1 (0,17–0,67 s so với 1,39 s). Vì mỗi lần quyết định mới phát một lệnh `do_walk`, quái Linux đổi hướng/bám theo mượt hơn hẳn; quái JX1 "giật cục", đứng ngẩn ~1,4 s mới đuổi tiếp.
`npcs.txt` bản Linux có **16 cột JX1 không có**: `AIMaxTime, DropRateFile, AuraSkillId, AuraSkillLevel, PasstSkillId, PasstSkillLevel, {Fire,Cold,Lighting,Physical,Poison}DamageBase, {Fire,Cold,Lighting,Physical,Poison}MagicBase`. Chiều ngược lại: **0 cột** — JX1 là tập con.

### KB-2: `CommonAction` của JX1 có thêm một nhánh mà Linux không có

JX1 (`KNpcAI.cpp:1247-1258`, đã có từ trước bản lưu `.truoc_bdh_2708`):
```c
if (Npc[m_nIndex].IsCanInput()) {           // IsCanInput() == m_ProcessAI
    GetMpsPos(&x,&y);
    nRange = g_GetDistance(GetCommand().Param_X, GetCommand().Param_Y, x, y);
    if (nRange > 3) ProcCommand(1);         // TIẾP TỤC lệnh cũ
    else SendCommand(do_stand);             // tới nơi thì đứng
}
else
    SendCommand(do_walk, m_OriginX+nOffX, m_OriginY+nOffY);   // <-- CHỈ nhánh này mới lang thang
```
Bản Linux **không có `if/else` này**, luôn phát thẳng `do_walk` tới `Origin ± offset`.

Nghĩa là ở JX1, khi quái đang rảnh (`m_ProcessAI = 1` — trạng thái bình thường lúc không diễn hoạt ảnh), lệnh tuần tra vừa bốc thăm bị **vứt đi**, thay bằng "chạy lại lệnh cũ". Điểm ngẫu nhiên mới chỉ được gửi khi quái **đang bận**. Đây là lý do cấu trúc khiến quái JX1 lang thang ít và hay đứng yên tại chỗ.

### KB-3: khoảng cách tối thiểu 32 (mục 1.5 ★) — Linux còn, JX1 chú thích.

### KB-4: cổng kiểm mục tiêu phụ khi giữ địch cũ

Linux `ProcessAIType01` gọi thêm `0x08079200(&Npc[e], me)` để quyết định có bỏ mục tiêu không:
```c
if (Npc[e].[+0x19A4]) return (Npc[e].[+4] == me);
if (Npc[e].[+0x19A0] == 0) return 0;
return (Npc[e].[+4] != me);
```
(hai cờ `+0x19A0/+0x19A4` + một chỉ số ở `+4` — dạng "chỉ đối tượng X mới thấy/khoá được"). JX1 không có tầng lọc này ở AI 1.

### KB-5: chỗ đặt nhịp AI

* JX1: đặt `m_NextAITime` **ngay trong `Activate`**, trước `switch`.
* Linux: `Activate` chỉ **so sánh**; việc cộng nhịp nằm trong `0x0808C640` được **từng `ProcessAIType` gọi ở dòng đầu**, và hàm này còn kiêm việc **huỷ mục tiêu đã chết**:
```c
p->m_NextAITime = SubWorld[..].m_dwCurrentTime + (BYTE)p->m_AIMAXTime;
e = p->m_nPeopleIdx;
if (Npc[e].m_Doing == do_death || Npc[e].m_Doing == do_revive) p->m_nPeopleIdx = 0;
else if (Npc[e].m_Kind == kind_player && Npc[e].[+0x168C] == 0) p->m_nPeopleIdx = 0;
```
JX1 chỉ bỏ mục tiêu qua `m_dwID <= 0 || !InEyeshot()` trong `ProcessAIType01` — **không kiểm `IsAlive()` ở tầng này** (chỉ kiểm trong `FollowAttack` → `CheckNpc`).

### KB-6: cùng thuật toán, khác cách đo khoảng cách
`InEyeshot`, `KeepActiveRange`, `FollowAttack` bản Linux đều dùng **bình phương khoảng cách** (không `sqrt`); JX1 dùng `NpcSet.GetDistance` / `g_GetDistance` có căn ở `InEyeshot`, `KeepActiveRange`, `FollowAttack` (riêng cổng "địch xa" thì cả hai dùng `GetDistanceSquare` vs `pAIParam[10]`). Kết quả toán học tương đương, nhưng JX1 tốn `sqrt` mỗi lần và có sai số làm tròn ở mép.

### Giống nhau (đã đối chiếu từng dòng)
`MAX_AI_PARAM = 11` · `relation_enemy = 8` · `kind_dialoger = 3` · `do_stand/walk/skill = 1/2/5` ·
`do_death/do_revive = 10/21` · công thức bốc thăm `pAIParam[0]`, `[1..4]`, `[5]`, `[6]`, `[10]` ·
`CommonAction` 80/20 và mẹo lấy bit 0 làm dấu · `KeepActiveRange` thu vùng còn 1/2 · quái chỉ đi bộ.

---

## 3. ĐẠN SKILL "DÍ THEO" (TRUY ĐUỔI MỤC TIÊU)

### 3.1 Khung khái niệm (giống nhau ở hai bản)

`settings\missletemplate.txt` — hai cột quyết định:

* **`MoveKind`** (Loại hình hành động), 0..8 + 100:
  `0 原地` tại chỗ · `1 直线飞行` bay thẳng · `2 随机飞行` ngẫu nhiên · `3 环行飞行` bay vòng ·
  `4 阿基米德螺旋线` xoắn Archimedes · **`5 跟踪目标飞行` BAY TRUY ĐUỔI MỤC TIÊU** ·
  `6 玩家动作类` theo động tác · `7 抛物线` parabol · `8` một đường thẳng · `100 回旋` bay về.
* **`FollowKind`** (Bám theo): `0 不跟踪` · `1 跟踪NPC` · `2 跟踪子弹`.

Enum trong JX1 `SkillDef.h` **trùng khớp từng mục** (`MISSLE_MMK_Stand … MISSLE_MMK_Follow … MISSLE_MMK_RollBack = 100`, `MISSLE_MFK_None/NPC/Missle`).

### 3.2 Dữ liệu `missles.txt` — **57 cột y hệt nhau**

| | Linux | JX1 |
|---|---|---|
| số dòng | 441 | 468 (dự án thêm 27 dòng) |
| `MoveKind` | 0:205 1:188 2:1 3:3 4:15 **5:13** 7:9 100:7 | 0:219 1:201 2:1 3:3 4:15 **5:13** 7:9 100:7 |
| `FollowKind` | 0:438 **1:2 2:1** | 0:465 **1:2 2:1** |

**13 viên đạn truy đuổi (MoveKind=5)** — trùng id ở cả hai bản:
`67` Thiên La Địa Võng (đạn con) · `144` Kim Đỉnh Phật Quang · `166` Phi Long Tại Thiên ·
`168` Thiên Hạ Vô Cẩu · `224` Xí Không Hoả Vũ · `270` Địa Lão Thử · `301` **kỹ năng cung kỵ** ·
`302` **kỹ năng cung binh** · `329` U Hồn Phệ Ảnh · `336` Thục Khuyển Phệ Nhật ·
`352` Tử Vi Băng Hoa d · `418` Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm · `426` Phách Thạch Phá Ngọc.

**3 viên bám theo (FollowKind≠0)**: `14` Tật Phong Chú (bám NPC) · `85` Thiên La Địa Võng Phi Đao (bám đạn) · `107` Tiểu Lý Phi Đao (bám NPC).

Đối chiếu 16 dòng này trên 16 cột động học: **15/16 giống hệt**. Chỉ lệch **id 426**:

| | Linux | JX1 |
|---|---|---|
| Speed | 25 | **30** |
| LifeTime | 15 | **24** |
| CollidRange | 1 | **3** |
| ColVanish | 0 | **1** |

### 3.3 Mã truy đuổi trong nhị phân Linux

`KMissle::Breathe` = **0x080760E0** → `KMissle::Fly` = **0x080758E0**.
Bên trong `Fly`, chuỗi so sánh `MoveKind` (`[this+0x14]`):
`==4` Helix `0x08075AA0` · `>4` → `0x08075CA0` (`==7` parabol, `==100` bay về `0x08075CC8`, **`==5` truy đuổi `0x08075D58`**) · `==1` thẳng `0x08075D38` · `==3` vòng `0x080759AF` · `==0` tại chỗ.

**Nhánh truy đuổi (0x08075D58)** dịch ngược:

```c
case MISSLE_MMK_Follow:
    n = m_nParam1;  m_nParam1 = n + 1;              // ★ Linux đếm bằng m_nParam1 (+0x11C)
    if (n > 7) {                                    // cứ 8 nhịp mới nắn hướng một lần
        m_nParam1 = 0;
        if (m_nFollowNpcIdx > 0 && Npc[m_nFollowNpcIdx].m_dwID == m_dwFollowNpcID) {
            Map2Mps(đạn) -> (sx,sy);  Map2Mps(mục tiêu) -> (dx,dy);
            dist = GetDistance(sx,sy,dx,dy);        // có sqrt (FPU)
            if (dist != 0) {
                m_nXFactor = ((dx-sx) << 10) / dist;
                m_nYFactor = ((dy-sy) << 10) / dist;
                m_nParam2  = dist / speed + 1;      // ★ số nhịp ước lượng tới đích — JX1 KHÔNG có
                m_nDirIndex = g_GetDirIndex(...);  m_nDir = g_DirIndex2Dir(..., 64);
            }
        }
    }
    nDOffsetX = m_nXFactor * m_nSpeed;
    nDOffsetY = m_nYFactor * m_nSpeed;
```

Offset KMissle (Linux): `+0x14` MoveKind · `+0x18` FollowKind · `+0x1C` cao độ · `+0x20` Zspeed ·
`+0x24` LifeTime · `+0x28` Speed · `+0x2C` ResponseSkill · `+0x34` trạng thái · `+0x40` CollidRange ·
`+0x44` DmgRange · `+0x60` m_nCurrentLife · `+0x74` m_nCurrentMapZ · `+0xC8/+0xCC` XFactor/YFactor ·
**`+0xD4` m_nFollowNpcIdx** · **`+0xD8` m_dwFollowNpcID** · `+0xEC` Zacc · `+0xFC` SubWorldId ·
`+0x100` RegionId · `+0x11C/0x120/0x124` Param1/2/3 · `+0x130/0x134` TempParam1/2 ·
`+0x144/0x148/0x14C` DirIndex/Dir/Angle · `+0x154` MissRate.

Giao mục tiêu bám: **6 chỗ** trong module kỹ năng (`0x080EB4B8`, `0x080EB8EC`, `0x080EBCA4`, `0x080EC031`, `0x080EC4A2`, `0x080EC843`), mẫu:
```c
Missle[i].m_nFollowNpcIdx = pSkillParam->nTargetId;
if (pSkillParam->nTargetId > 0) Missle[i].m_dwFollowNpcID = Npc[nTargetId].m_dwID;
```
→ **đúng 6 chỗ, đúng thứ tự** như `KSkills.cpp` của JX1 (dòng 1252, 1373, 1477, 1612, 1727, 1894).

---

## 4. KHÁC BIỆT ĐẠN TRUY ĐUỔI — LINUX vs DỰ ÁN JX1

| # | Điểm | Linux | JX1 (`KMissle.cpp`) |
|---|---|---|---|
| **Đ-1** | Chu kỳ nắn hướng | 8 nhịp | 8 nhịp — **giống** (`if (m_nTempParam1++ >= 8)`, dòng 996) |
| **Đ-2** | Biến đếm 8 nhịp | **`m_nParam1`** (`+0x11C`) | **`m_nTempParam1`** |
| **Đ-3** | Ghi `m_nParam2 = dist/speed + 1` | **CÓ** | **KHÔNG** |
| **Đ-4** | Công thức hướng | `((d)<<10)/dist` + `g_GetDirIndex/g_DirIndex2Dir` | **giống hệt** (dòng 1004-1013) |
| **Đ-5** | Điều kiện huỷ bám mỗi nhịp (`Activate`) | `!IsMatch(id)` **hoặc** khác `m_SubWorldId` | `!IsMatch` ‖ khác subworld ‖ **`m_nProtectedTime > 0`** ‖ **`m_HideState.nTime > 0`** (dòng 506-511) |
| **Đ-6** | `MISSLE_MMK_Circle` | `(Speed + **50**)` | `(m_nSpeed + **30**)` (dòng 887-888) |
| **Đ-7** | `MISSLE_MMK_Helix` | `(Speed + CurrentLife + 50)` | **giống** |
| **Đ-8** | `MISSLE_MMK_RollBack (100)` | `TempParam1/TempParam2`, đảo XFactor/YFactor, `m_nDir -= 32` | **giống hệt** (`MaxMissleDir/2 = 32`) |
| **Đ-9** | Cột `MissRate` của `missles.txt` | **có nạp** → `+0x154` | **không nạp** (JX1 chỉ lấy `m_nMissRate` từ thuộc tính phép — `magic_missle_missrate`) |
| **Đ-10** | Cột `CanColFriend`, `CanSlow` | **máy chủ không nạp** (chuỗi không tồn tại trong nhị phân) | **có nạp** (dòng 340-341, ngoài `#ifdef`) |
| **Đ-11** | `ColFollowTarget`, `CanDestroy`, `LoopPlay`, `MultiShow` | không có trong nhị phân máy chủ | chỉ nạp trong `#ifndef _SERVER` (client) — **tương đương** |

**Ý nghĩa thực chiến của Đ-2/Đ-3**: cả 13 dòng `MoveKind=5` đều có `Param1=Param2=Param3=0`, nên việc Linux mượn `m_nParam1` làm bộ đếm và ghi đè `m_nParam2` **không đụng gì tới dữ liệu** — hai bản chạy như nhau. Đây là khác biệt hiện thực, không phải khác biệt hành vi.

**Ý nghĩa thực chiến của Đ-5**: ở JX1, đạn đang truy đuổi sẽ **rơi khoá** khi mục tiêu bật vòng tròn bất tử (`m_nProtectedTime`) hoặc ẩn thân (`m_HideState`) → đạn bay thẳng theo hướng cũ. Ở Linux đạn **vẫn bám**. Đây là khác biệt **có thật, ảnh hưởng PK**.

**Ý nghĩa thực chiến của Đ-6**: chỉ 3 viên đạn dùng `MoveKind=3`; bán kính vòng bay JX1 nhỏ hơn Linux ~40 %.

---

## 5. TÓM TẮT 8 ĐIỂM ĐÁNG CHÚ Ý NHẤT

1. **`AIMaxTime` không có trong `npcs.txt` của JX1** ⇒ toàn bộ quái nhận nhịp mặc định **25 khung (1,39 s)**, trong khi bản Linux đặt riêng 3–36 khung. Đây là nguyên nhân số một khiến quái JX1 phản ứng chậm/giật.
2. **`CommonAction` của JX1 có nhánh `IsCanInput()`** làm lệnh tuần tra vừa bốc thăm bị bỏ khi quái đang rảnh — bản Linux không có nhánh này.
3. **Khoảng cách đánh tối thiểu 32** còn hoạt động ở Linux (`KeepAttackRange`), **bị chú thích** ở JX1.
4. **AIMode 7, 8, 9, 10 bị chú thích ở JX1** nhưng `npcs.txt` của JX1 vẫn còn ~120 con dùng các mode đó ⇒ nhóm quái này không có AI. Linux chạy đủ (thêm cả 21–24 sẵn trong switch).
5. Ngoài 4 điểm trên, **thuật toán AI 1 trùng khít từng bước** giữa hai bản.
6. **Bảng `missles.txt` và enum đạn giống hệt nhau** (57 cột, 13 đạn truy đuổi, 3 đạn bám). Chỉ dòng `426` lệch 4 ô.
7. **Cơ chế truy đuổi giống hệt** (nắn hướng mỗi 8 nhịp, `<<10 / dist`); khác biệt duy nhất có ảnh hưởng là **JX1 huỷ bám khi mục tiêu bất tử/ẩn thân**, Linux thì không.
8. `MISSLE_MMK_Circle`: Linux `Speed+50`, JX1 `Speed+30`.

---

## 6. NHỮNG THỨ **CHƯA** ĐO TRONG PHIÊN NÀY (đừng coi là đã kết luận)

* Chi tiết `GetNearestNpc` bản Linux (0x0808ED10) — mới xác nhận chữ ký `(this, relation=8)`, chưa đối chiếu vòng quét ô/vùng với JX1.
* Nội dung `ProcessAIType02..06` bản Linux (chỉ mới lấy địa chỉ).
* Nội dung AI 7/8/9/10/21/22/23/24 bản Linux.
* Ý nghĩa chính xác hai cờ `+0x19A0/+0x19A4` trong cổng lọc mục tiêu `0x08079200`.
* Đường đi thực tế sau `SendCommand(do_walk)` (có gọi `FindPath` hay không) ở hai bản — phần này quyết định quái có vòng qua vật cản hay đâm vào tường.
* Bản client `D:\ServerLinux\Patch\game_y_unpacked.bin` (phần vẽ đạn truy đuổi) chưa mổ.
