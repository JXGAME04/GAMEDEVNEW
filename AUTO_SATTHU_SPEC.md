# DAC TA HE SAN BOSS SAT THU - cho phien viet WAuto tu dong

> Sinh tu dong boi `ReverseTools/gen_spec_satthu.py` tu DU LIEU DANG CHAY.
> Nguon goc: ban Linux da port 25/08 (xem `BANGIAO_3HOATDONG_2508.md`).

## 1. Tong quan vong choi

```
NPC 769 "Nhiep Thi Tran" (7 thanh)
  |- menu "Nhiem vu cap 90"  ->  hien 20 boss (so 141..160), NGUOI CHOI TU CHON 1
  |     |- givetask(n)  ->  nt_setTask(1082, n)      [da nhan nhiem vu]
  |          |- toi map boss, giet DUNG con boss so n
  |               |- nhan: EXP + 1 Sat Thu lenh (cap nhom, NGU HANH THEO BOSS)
  |                    |- task 1082 ve 0, task 1193 +1   [1 luot / 8 luot moi ngay]
  |- menu "Hop thanh sat thu gian"  ->  bo 5 Sat Thu lenh CUNG CAP -> 1 Sat Thu Gian
```

## 2. Task ID (server dong bo sang client qua nt_setTask -> SyncTaskValue)

| Task | Y nghia | Dung cho auto |
|---|---|---|
| **1082** | so hieu boss dang truy na (1..160); **0 = chua nhan** | biet co nhiem vu khong, va con nao |
| **1193** | so lan da giet **hom nay** (tran 8) | biet con luot khong |
| 1192 | ngay nhan (yymmdd) - engine tu reset 1193 khi sang ngay | khong can dung |
| 1217 | he thuong phu | khong can |

Doc tu client: `g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, <task>, 0)`
(khuon `DTG_TaskVal` trong `UiTaskGuide.cpp`).

## 3. Gioi han / dieu kien

- **8 luot/ngay** (KILLER_MAXCOUNT; chinh o cauhinh_hoatdong.lua khoa HD3_ST_MAX_NGAY).
- Chi **nhom cap 90** con phat thuong (yeu cau cap nhan vat 90..350).
- Dang co nhiem vu (1082 != 0) thi **khong nhan them** - phai giet xong hoac huy.
- Huy nhiem vu: muc thoai "Huy nhiem vu." (/cancel) hoac nut **Bo nhiem vu** tren F11.

## 4. Vat pham

| Bo | Ten | Ghi chu |
|---|---|---|
| `6,1,398` | **Sat Thu lenh** | nLevel = cap nhom (20/30/.../90), nSeries = ngu hanh **theo con boss** (0 Kim, 1 Moc, 2 Thuy, 3 Hoa, 4 Tho) |
| `6,1,399` | **Sat Thu Gian** | ghep tu 5 lenh cung cap; ngu hanh **random** trong 5 he cua 5 lenh da bo vao |
| `6,1,2356` | Sat Thu Bi Bao | dung can **6 Huyen Thien Chuy** (6,1,2357) |

**LUU Y ENGINE**: tao item bang Lua phai du **>= 7 tham so**:
`AddItem(6,1,398,<cap>,<he>,0,0)` - thieu tham so thi JX1 **khong tao item va IM LANG**.

## 5. QUY TRINH GHEP SAT THU LENH (auto phai lam dung thu tu)

```
1. Mo thoai NPC 769  ->  chon muc "Hop thanh sat thu gian"   (ham compose)
2. Server mo GiveItemUI (giao dien bo do vao)  ->  callback exchange_token
3. Auto bo DUNG 5 Sat Thu lenh (6,1,398) CUNG MOT CAP vao o
4. Bam xac nhan  ->  server kiem theo thu tu:
      - co item KHAC 6,1,398 lan vao   -> bao loi, KHONG ghep
      - du 5 nhung KHAC CAP nhau       -> bao loi, KHONG ghep
      - nhieu hon 5                    -> bao "de qua nhieu"
      - it hon 5                       -> bao "de qua it"
      - DUNG 5 cai CUNG CAP            -> xoa 5 lenh, tao 1 Sat Thu Gian
5. Ngu hanh Gian = random 1 trong 5 he cua 5 lenh vua bo (ham givesword)
```

**Dieu kien phu**: tui can it nhat 1 o trong (nen chua 3 o).
Sat Thu Gian la **ve vao Vuot Ai (Thach thuc thoi gian)**: doi truong mang **2 Gian**
cap 90 di bao danh (so cap: 2 Gian duoi cap 90).

## 6. Vi tri NPC 769 (7 thanh)

| Map | O X | O Y |
|---|---|---|
| 11 | 3210 | 4974 |
| 1 | 1506 | 3198 |
| 37 | 1647 | 3050 |
| 176 | 1372 | 3010 |
| 162 | 1573 | 3227 |
| 78 | 1512 | 3206 |
| 80 | 1700 | 2963 |

Client tim NPC nen dung **template id 769** (Npc[i].m_NpcSettingIdx == 769),
KHONG so theo ten (ten TCVN3 co byte cao, g_StrLower co the doi byte).

## 7. Bang 160 boss

Du lieu da sinh san cho C++:
- `Sources/Core/Src/KSatThuBossPos.h` : s_nST3BossMap/X/Y, s_szST3BossMenu/Moc, s_nST3Npc*
- `Sources/S3Client/Ui/UiCase/UiTaskGuideSatThuBoss.h` : s_szST3BossName/Info
- Chay lai: `python ReverseTools/gen_satthu_boss_table.py`

**Nhom cap 90 = so hieu 141..160** (nhom duy nhat con thuong):

| So | Ten boss | Map | O X | O Y | Noi o (bang nhiem vu) |
|---|---|---|---|---|---|
| 141 | Gi¶ Giíi Nh©n | 93 | 1644 | 3279 | trong TiÕn Cóc ®éng (205,204) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 142 | TrÞnh Cöu NhËt | 93 | 1646 | 3058 | trong TiÕn Cóc ®éng (205,191) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 143 | Chu Së B¸ | 93 | 1736 | 3213 | trong TiÕn Cóc ®éng (217,200) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 144 | Trang Minh Trung | 93 | 1610 | 3152 | trong TiÕn Cóc ®éng (201,197) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 145 | Cam ChÝnh C« | 225 | 1590 | 3325 | trong Sa M¹c s¬n  ®éng (198,207) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 146 | Vò NhÊt ThÕ | 225 | 1261 | 3247 | trong Sa M¹c s¬n  ®éng (157,202) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 147 | D­¬ng Phong  DËt | 225 | 1452 | 3377 | trong Sa M¹c s¬n  ®éng (181,211) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 148 | Hµ Sinh Vong | 225 | 1425 | 3107 | trong Sa M¹c s¬n  ®éng (178,194) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 149 | T¨ng ChØ ãan | 75 | 1711 | 3187 | trong Kho¶ Lang ®éng (213,199) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 150 | VÖ Biªn Thµnh | 75 | 1752 | 3124 | trong Kho¶ Lang ®éng (219,195) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 151 | Cè Thñ §»ng | 75 | 1831 | 3190 | trong Kho¶ Lang ®éng (228,199) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 152 | Gia C¸t Kinh Hång | 75 | 1639 | 3159 | trong Kho¶ Lang ®éng (204,197) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 153 | Phan Ng¹t Nh¹n | 321 | 1253 | 3002 | ë Tr­êng B¹ch nam (156,187) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 154 | Liªn H×nh Th¸i | 321 | 1483 | 2742 | ë Tr­êng B¹ch nam (185,171) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 155 | Bµo TriÖt s¬n | 321 | 1289 | 2613 | ë Tr­êng B¹ch nam (161,163) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 156 | V¹n Hå Tinh | 321 | 1113 | 2569 | ë Tr­êng B¹ch nam (139,160) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 157 | Trö Thiªn MÉn | 340 | 1217 | 2740 | ë M¹c Cao QuËt (152,171) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 158 | §o¹n L¨ng NguyÖt | 340 | 1723 | 2765 | ë M¹c Cao QuËt (215,172) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 159 | T¶  DËt Minh | 340 | 1275 | 2749 | ë M¹c Cao QuËt (159,171) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |
| 160 | NhËm Th­¬ng Khung | 340 | 1932 | 2759 | ë M¹c Cao QuËt (241,172) . Ta kh«ng tin ng­¬i th¾ng ®­îc h¾n! |

## 8. Da co san trong client (auto dung lai duoc ngay)

| Thu | O dau | Viec |
|---|---|---|
| GOI_TASKGUIDE_GOTO_SATTHU | CoreShell.h | bat/tat dan duong toi boss (Xa Phu -> menu -> map -> di bo) |
| TG_SatThuStart / TG_SatThuTick | CoreShell.cpp | may 6 pha + pha 10/11 (chua nhan nhiem vu -> ve thanh gap NPC) |
| DT_WalkTo / DT_FindNpcName / DT_UsePortal / DT_Answer / DT_Split / DT_FindAns | CoreShell.cpp | nghe di duong + bam thoai cua auto Da Tau |
| g_MoveStation | CoreShell.cpp | toa do Xa Phu tung map |
| SendUiCmdScript(6, "...") | KProtocol.cpp | goi ham Lua phia server (da co st3_quit, st3_goboss) |
| KUiTaskGuide::AutoTraceOnTask | UiTaskGuide.cpp | tu bat khung theo doi khi vua nhan nhiem vu |

## 9. Goi y vong lap cho WAuto

```
moi nhip:
  neu task 1193 >= 8         -> het luot hom nay, dung (cho sang ngay)
  neu task 1082 == 0         -> chua co nhiem vu:
        ve thanh gap NPC 769 (pha 10/11 co san)
        mo thoai -> "Nhiem vu cap 90" -> chon 1 con trong 20 con
  neu task 1082 != 0         -> da co nhiem vu:
        neu KHONG o map boss -> bat GOI_TASKGUIDE_GOTO_SATTHU (tu di)
        neu o map boss       -> tim NPC theo TEN boss (bang muc 7) va danh
        boss chet -> task 1082 tu ve 0, quay lai dau vong
  khi so Sat Thu lenh CUNG CAP >= 5 -> chay quy trinh ghep (muc 5)
```

**Nhip hoi sinh boss**: ReviveFrame 16200, JX1 chia doi (KNpcTemplate.cpp:140)
=> **~7,5 phut/lan**. Toi noi ma boss chua hoi sinh thi CHO TAI CHO, dung huy nhiem vu.

## 10. Config chinh duoc (script/header/cauhinh_hoatdong.lua)

| Khoa | Y nghia |
|---|---|
| HD3_ST_MAX_NGAY | tran luot giet/ngay (goc 8) |
| HD3_ST_HESO_EXP | % kinh nghiem moi lan giet (100 = goc) |
| HD3_ST_SO_LENH | so Sat Thu lenh moi lan giet (goc 1) |
| HD3_ST_EXP | bang exp rieng theo nhom cap (nil = goc) |
| HD3_ST_THUONG | bang thuong hoan thanh nhom 90 (nil = goc) |
| HD3_ST_TIEN_XE | tien thue xe khi F11 dan duong khac map (goc 1000) |
