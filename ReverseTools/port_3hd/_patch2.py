# -*- coding: utf-8 -*-
import io
P="01_satthu.md"
d=io.open(P,"r",encoding="utf-8",newline="").read()
M="[đã sửa theo phản biện]"
subs=[
(r"| Mẫu NPC | `settings\npcs.txt` id **761–820** + **769** | **CÓ, TRÙNG KHỚP 100 %** (đã diff 6 cột đầu của cả 60 id: 0 khác biệt) | `LevelScript = \script\npclevelscript\task_killboss.lua`; `ReviveFrame = 16200`; `PasstSkillId = 547`; `Skill1..4 = 53` |",
 r"| Mẫu NPC | `settings\npcs.txt` id **761–820** + **769** | **CÓ DÒNG nhưng KHÔNG TRÙNG KHỚP** " + M + r" — JX1 chỉ **87 cột**, Linux **103 cột**; JX1 **thiếu hẳn 16 cột**, trong đó có **`PasstSkillId` / `PasstSkillLevel` / `AuraSkillId` / `DropRateFile`**. Cả **60/60 id đều lệch**: 5 cột `*ResistMax` **85 (Linux) → 25 (JX1)**, 10 id lệch `LifeParam3`. Cột chung khớp: `LevelScript = \script\npclevelscript\task_killboss.lua`, `ReviveFrame = 16200` (JX1 ở **cột 86**, Linux cột 85), `Kind=0/Camp=5/Skill1..4=53` | ⚠️ **Bị động 547 KHÔNG gắn được qua `npcs.txt` của JX1** (không có cột `PasstSkillId`) — phải thêm cột hoặc gắn skill bằng đường khác |"),
(r"| Script skill 547 | `script\skill\npc\killerbossmianyi.lua` | CHƯA KIỂM | 5 thuộc tính bị động |",
 r"| Script skill 547 | `script\skill\npc\killerbossmianyi.lua` | **JX1 ĐÃ CÓ** (956 B) nhưng **LỆCH 1 GIÁ TRỊ** " + M + r" | Đã kiểm đủ 5 thuộc tính: `fasthitrecover_v=100`, `fatallystrikeres_p=99`, `freezetimereduce_p=200`, `poisontimereduce_p=`**180 (Linux) / 280 (JX1)**, `stuntimereduce_p=200` |"),
]
miss=0
for a,b in subs:
    if a in d: d=d.replace(a,b,1)
    else: miss+=1; print("!! KHONG KHOP:", a[:60])
io.open(P,"w",encoding="utf-8",newline="").write(d)
print("miss",miss,"len",len(d))
