# -*- coding: utf-8 -*-
import io, sys
P = "01_satthu.md"
d = io.open(P, "r", encoding="utf-8", newline="").read()
orig = d
M = "[da sua theo phan bien]"
M = "[đã sửa theo phản biện]"

subs = [
# E1 huoyuedu Param1
("| Điểm hoạt động (huoyuedu) | tối đa **2 lần/ngày**, +5 rồi +3 | `settings\huoyuedu\huoyuedu.txt` dòng 12: `11 杀手任务 2871 2 5 3` |",
 "| Điểm hoạt động (huoyuedu) | tối đa **2 lần/ngày**, **+3 rồi +3** %s | `settings\huoyuedu\huoyuedu.txt` dòng 12: `11\t杀手任务\t2871\t2\t3\t3` (Param1=**3**, không phải 5) |" % M),

("`ActivityId=11, ActivityName=杀手任务, CountTask=2871, MaxCount=2, Param1=5, Param2=3` ⇒ **+5 điểm lần 1, +3 lần 2, tối đa 2 lần/ngày**.",
 "`ActivityId=11, ActivityName=杀手任务, CountTask=2871, MaxCount=2, Param1=**3**, Param2=3` ⇒ **+3 điểm lần 1, +3 lần 2, tối đa 2 lần/ngày**. %s (dòng 12 thực tế là `11\t杀手任务\t2871\t2\t3\t3`; dòng có Param1=5 là dòng 11 `10 信使任务 2870 2 5 2`)." % M),

# E2 npcs.txt
("| Mẫu NPC | `settings\npcs.txt` id **761–820** + **769** | **CÓ, TRÙNG KHỚP 100 %** (đã diff 6 cột đầu của cả 60 id: 0 khác biệt) | `LevelScript = \script\npclevelscript\task_killboss.lua`; `ReviveFrame = 16200`; `PasstSkillId = 547`; `Skill1..4 = 53` |",
 "| Mẫu NPC | `settings\npcs.txt` id **761–820** + **769** | **CÓ DÒNG nhưng KHÔNG TRÙNG KHỚP** %s — JX1 chỉ **87 cột**, Linux **103 cột**; JX1 **thiếu hẳn 16 cột** trong đó có **`PasstSkillId` / `PasstSkillLevel` / `AuraSkillId` / `DropRateFile`**. Cả **60/60 id đều lệch**: 5 cột `*ResistMax` = **85 (Linux) → 25 (JX1)**, 10 id lệch `LifeParam3`. Các cột chung khớp: `LevelScript = \script\npclevelscript\task_killboss.lua`, `ReviveFrame = 16200` (JX1 ở **cột 86**, Linux cột 85), `Kind/Camp/Skill1..4 = 53` | ⚠️ **Bị động 547 KHÔNG gắn được qua `npcs.txt` của JX1** (không có cột) — phải thêm cột hoặc gắn skill bằng đường khác |" % M),

# E3 map names
("> Tên bản đồ trong `BossInfo` cũng lệch với `maplist.ini` (vd map 12: `maplist.ini` = \"Đao Đao học viện\r\n> Tín Tường tự\", `killer.txt` = \"Mật đạo Tín Tướng tự\"; map 168: \"Phụng Nhân động\" vs \"Phụng Nhãn động\").",
 "> Tên bản đồ trong `BossInfo` cũng lệch với `maplist.ini` %s: map 12 `maplist.ini` = **\"Địa đạo hậu viện Tín Tướng tự\"** (KHÔNG phải \"Đao Đao học viện Tín Tường tự\"), `killer.txt` = \"Mật đạo Tín Tướng tự\". Map 168 **KHÔNG lệch**: cả `maplist.ini` lẫn `killer.txt` đều ghi \"Phụng Nhãn động\". Map 91 `maplist.ini` = \"Mê cung **Kê** Quán động\"." % M),

("| 40 | :49 | 50–69 | 41–60 | 45 | 5 Kinh Hoàng động · 168 Phụng Nhân động · 23 Thần Tiên động · 91 Mê cung Kỳ Quán động · 135 Kiếm Tinh Phong sơn động |",
 "| 40 | :49 | 50–69 | 41–60 | 45 | 5 Kinh Hoàng động · 168 **Phụng Nhãn** động · 23 Thần Tiên động · 91 Mê cung **Kê** Quán động · 135 Kiếm Tinh Phong sơn động %s |" % M),

("| 50 | :71 | 72–91 | 61–80 | 55 | 12 Đao Đao học viện Tín Tường tự · 24 Huyền Thủy động · 42 Thiên Tâm động · 66 Đẩy Đằng Đình hồ tầng 1 · 194 Ngọc Hoa động |",
 "| 50 | :71 | 72–91 | 61–80 | 55 | 12 **Địa đạo hậu viện Tín Tướng tự** · 24 Huyền Thủy động · 42 Thiên Tâm động · 66 Đẩy Đằng Đình hồ tầng 1 · 194 Ngọc Hoa động %s |" % M),

# E4 map spelling count
("| Bản đồ | 40 map boss + 7 map NPC | **CÓ ĐỦ 47/47**, tên khớp (3 sai chính tả vặt: map 83 \"Hoả/Khoả\", map 14, map 12) |",
 "| Bản đồ | 40 map boss + 7 map NPC | **CÓ ĐỦ 47/47** (đã đối chiếu `<id>_name=` của `maplist.ini` ↔ `MapList.ini`), **5** chỗ lệch tên: map 14, 83 (\"Hoả/Khoả\"), 91 (\"Kê/Kế\"), 12 (\"tự/Tự\"), 162 (\"Đại Lý phủ / Đại Lý\") %s |" % M),

# E5 skill script
("| Script skill 547 | `script\skill\npc\killerbossmianyi.lua` | CHƯA KIỂM | 5 thuộc tính bị động |",
 "| Script skill 547 | `script\skill\npc\killerbossmianyi.lua` | **JX1 ĐÃ CÓ** (956 B) nhưng **LỆCH 1 GIÁ TRỊ** %s | 5 thuộc tính bị động đã kiểm: `fasthitrecover_v=100`, `fatallystrikeres_p=99`, `freezetimereduce_p=200`, `poisontimereduce_p=`**180 (Linux) / 280 (JX1)**, `stuntimereduce_p=200` |" % M),

# E6 item names
("| **6,1,400** | **Sát thủ giản** (dòng 402) | **Sơ đồ thiết** (dòng 402) |",
 "| **6,1,400** | **Sát thủ giản** (dòng 402) | **Sư đồ thiếp** (dòng 402, script `\script\item\card\card_shitu.lua`) %s |" % M),

("| **6,1,2347** | **Sát Thủ Bí Bảo** | Khấp Đao Quần lễ hộp |",
 "| **6,1,2347** | **Sát Thủ Bí Bảo** | **Khấp Địa Quần** lễ hộp %s |" % M),

# E7 GetItemProp
("| 26 | `GetItemProp` | `nieshichen.lua:167` | ✔ | 5 giá trị trả về |",
 "| 26 | `GetItemProp` | `nieshichen.lua:167` | ✔ | JX1 trả **6** giá trị `(g,d,p,lv,series,luck)` (`KJx2WarInfra.cpp:769`, `return 6`); script chỉ nhận 5 — Lua bỏ giá trị thừa, không sao %s |" % M),

# E8 call sites
("`D:\ServerLinux\server1`: **0 định nghĩa**, ~40 điểm gọi.",
 "`D:\ServerLinux\server1`: **0 định nghĩa**, **82 điểm gọi trên 60 tệp** (không phải ~40) %s." % M),

# E9 reward-name typos
("| Tuyết Ảnh | 0,10,5,3,0,0 | 0.0021 | |", "| Tuyệt ảnh | 0,10,5,3,0,0 | 0.0021 | |"),
("| Đích Lư | 0,10,5,4,0,0 | 0.0021 | |", "| Đích Lô | 0,10,5,4,0,0 | 0.0021 | |"),
("| Đồ Phổ Đằng Long Hồi | 6,1,30530,1,0,0 | 0.0003 | |", "| Đồ Phổ Đằng Long Hài | 6,1,30530,1,0,0 | 0.0003 | |"),
("| Đồ Phổ Đằng Long Hồng Liên | 6,1,30533,1,0,0 | 0.0003 | |", "| Đồ Phổ Đằng Long Hạng Liên | 6,1,30533,1,0,0 | 0.0003 | |"),

# E10 bNoRevive naming
("| 6 | **tham số 6 của `AddNpc`** | số | **luôn = 0** | Bản Linux: cờ `bNoRevive` (`+0x1824`). **Bản JX1: nSeries!** (xem 9.2) |",
 "| 6 | **tham số 6 của `AddNpc`** | số | **luôn = 0** | Bản Linux: **một cờ BYTE tại `KNpc+0x1824`** — tên `bNoRevive` là **suy đoán chưa chứng minh** %s (mã chỉ cho thấy `setne`; ghi chú JX1 ở `ScriptFuns.cpp` lại ánh xạ cùng ô này sang **camp**). **Bản JX1: nSeries!** (xem 9.2) |" % M),

("| **tham số 6** | cờ `bNoRevive` → `setne [KNpc+0x1824]` (`0x0811BDB2`) | **`nSeries`** (`ScriptFuns.cpp:6833`) |",
 "| **tham số 6** | cờ BYTE → `setne [KNpc+0x1824]` (`0x0811BDB2`) — **tên \"bNoRevive\" CHƯA XÁC MINH** %s | **`nSeries`** (`ScriptFuns.cpp:6833`) |" % M),
]

miss = []
for a, b in subs:
    if a in d:
        d = d.replace(a, b, 1)
    else:
        miss.append(a[:70])
print("KHONG KHOP:", len(miss))
for m in miss: print("   !!", m)
io.open(P, "w", encoding="utf-8", newline="").write(d)
print("da ghi", len(d), "ky tu (truoc:", len(orig), ")")
