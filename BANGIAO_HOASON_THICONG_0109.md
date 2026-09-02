# BÀN GIAO THI CÔNG PHÁI HOA SƠN (faction id 10) — 01/09/2026 tối

Phần phân tích nền (3 phái, mổ Linux + client VLTK) ở `BANGIAO_3PHAI_HOASON_VUHON_TIEUDAO_0109.md`. Tài liệu này chỉ ghi phần **đã thi công** cho Hoa Sơn, đợt 1, **chưa nghiệm thu trong game**.

Yêu cầu chủ game: làm từng phái, Hoa Sơn trước; **thuộc tính và chức năng từng kỹ năng phải giống 100% bản Linux** (`D:\ServerLinux\server1`). Vì vậy dữ liệu kỹ năng lấy từ Linux, không lấy bản client VLTK (bản VLTK khác Linux ở 8 dòng skills.txt: 1349, 1352, 1358, 1369, 1376, 1378, 1382, 1384 — xem mục 2.2).

Mọi chỗ sửa đều có marker `[HOASON 01/09]`. Tool sinh/vá idempotent nằm ở `ReverseTools\phai3\hoason_thicong\` (chạy lại được, có chế độ `--kiem`).

---

## 0. Trạng thái

| Hạng mục | Trạng thái |
|---|---|
| Engine 13 phái + đợt b port 4 nhóm thuộc tính Linux (mục 10.2) | build sạch 4 binary, **.moi chờ swap** (mục 8) |
| Dữ liệu server + client (faction, skills, rank, npcs, item, map) | đã ghi vào cây chạy thật `E:\...\bin\{server,client}` |
| Script server (Linux huashan.lua; đợt b: bỏ nhiệm vụ, chỉ còn NPC phát kỹ năng — mục 10.1) | đã ghi, `syncheck` OK, `t71` 0 lỗi |
| Client UI bảng kỹ năng Hoa Sơn | ini + `khung_hs.spr` vẽ mới, chưa xem trong game |
| Vũ Hồn / Tiêu Dao | chỉ có ô engine (id 11/12, icon tổ đội, tên); chưa có dữ liệu/script/UI |

---

## 1. Engine (D:\GAMEDEVNEW\Sources, tool `hs_engine_patch.py`)

| Tệp | Sửa gì | Bằng chứng / lý do |
|---|---|---|
| `Core\Src\GameDataDef.h` | thêm `MAX_FACTION_NUM = 13` vào enum series | trước đây `MAX_FACTION = FACTIONS_PRR_SERIES(2) × series_num(5) = 10` (KFaction.h:16) |
| `Core\Src\KFaction.h/.cpp` | `MAX_FACTION = MAX_FACTION_NUM`; `Init()` id phái = **số mục `[i]`** trong FactionInfo.ini (bỏ công thức hệ×2+k); mục trống → bỏ qua (đọc Name không mặc định); `GetID(series, nNo)` duyệt bảng; `GetID(series, name)` nhận cả `Name=` (GBK) lẫn `ValueName=` (huashan); thêm `GetIDByValueName` | Linux `faction_def.lua` gọi `SetFaction("huashan")`; KIniFile::GetString trả mặc định "Thiếu Lâm/kim" cho mục không tồn tại → phải kiểm Name rỗng |
| `Core\Src\KPlayerFaction.cpp:80` | cửa `AddFaction` kiểm theo bảng (id < 13, cùng hệ, có tên) thay vì khoảng `[hệ×2, hệ×2+2)` | Hoa Sơn hệ Thủy là phái thứ 3 của hệ |
| `Core\Src\KLadder.h/.cpp`, `KProtocol.h:2223-2229` | `MAX_FAC = MAX_FACTION_NUM+1`; `TGAME_STAT_DATA` các mảng `[11]` → `[MAX_FACTION_NUM+1]` (14); 5 chỗ `nFac >= …` | KLadder đọc `[nFac+1]` (KLadder.cpp:30-73) |
| `MultiServer\Goddess\DBBackup.h/.cpp`, `GameStatistic.h`, `Goddess2\src\DBBackup.*` | `TStatData` `[11]`→`[14]` (PHẢI cùng cỡ với Core, `KLadder::Init` kiểm sizeof); mảng đếm `[12]`→`[15]` (13 phái + 13 chưa nhập + 14 xuất sư); 3 switch tên phái thêm case 10/11/12 (华山派/武魂/逍遥派); **sửa luôn lỗi cũ** bounds `nSect<=10 && >=1` → `nSect<=12 && >=0` (Thiếu Lâm id 0 trước đây bị bỏ khỏi thống kê) | DBBackup.cpp:578/592/607 |
| `Core\Src\KNpc.cpp:11774` | bảng `FactionName[]` 13 tên TCVN3; `nFirstFaction <= 9` → `< MAX_FACTION_NUM` | bảng cũ 10 tên bị **hỏng mã hoá** (TCVN3 bị UTF-8 hoá bởi tool sửa trước đây, ví dụ "Thúy Yên" = `Thi C3 B3 y Y C2 AA n`) → viết lại toàn bộ |
| `Core\Src\ScriptFuns.cpp:14335` | `s_szFaction[MAX_FACTION_NUM]` 13 tên; `nNo < 11` → `< MAX_FACTION_NUM`; thêm API Lua `SetLastFactionNumber(n)` (đặt cur/first, UpdateGameTitle) và `GetFactionNumber()` | Linux `hoason.lua:45` gọi `SetLastFactionNumber(10)`. `KPlayer::SendFactionData` là private → không gọi (SetFaction ngay trước đã đồng bộ). `ClearFactionRecord` Linux = `ClearFactionIfnfo` JX1 nhưng hàm đó chỉ có `#ifdef _SERVER` → làm shim Lua |
| `Core\Src\KPlayerBot.cpp:111` | `s_facNpc[MAX_FACTION]` thêm 3 dòng (hoason/vuhon/tieudao); `pb_GiveFactionWeapon` case 10 = kiếm | bot vẫn chỉ xoay trong `hệ×2+k` (KPlayerBot.cpp:1291) → **bot chưa chọn Hoa Sơn** |
| `S3Client\Ui\UiCase\UiSkillsNew.cpp:334` | `g_uIdToIndexMap` +21 mục Hoa Sơn (ô 0-20, xem mục 4); nút đóng dịch cho faction 10 | |
| `S3Client\Ui\UiCase\UiTeamManager2.cpp` | icon tổ đội case 11 `icon_zd_wht.spr`, 12 `icon_zd_xy.spr` (Hoa Sơn case 10 `icon_zd_hsp.spr` đã có sẵn) | pak updatejx15/16 |
| `S3Client\Ui\UiCase\UiTongJX2.cpp:120` | `s_szFaction[MAX_FACTION_NUM+1]` 13 tên | |

Build: `Core.vcxproj` Server Release x64 + Client Release Win32, `S3Client.vcxproj` Release Win32, `Goddess.vcxproj` Debug Win32 (`-p:OutDir=...\Goddess\Debug\ -p:PostBuildEventUseInBuild=false`). Quét `EF BF BD` trên 18 tệp: sạch. `GameServer.exe` không cần build lại (KSOServer.cpp:2265 chỉ `_ASSERT`, truyền `nDataLen` cho Core).

---

## 2. Dữ liệu (tool `hs_data1.py`, `hs_port2013.py`, `hs_map_pak.py`) — cây `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin`

### 2.1 Môn phái
`server|client\settings\faction\FactionInfo.ini` và `门派设定.ini` (4 tệp) thêm:
```
[10]
Name=华山派
Series=水
Camp=中立
ValueName=huashan
ShowName=Hoa Sơn        (TCVN3)
```
Nguồn: VLTK `faction_settings.ini` mục huashan (series 2, camp 3, rank 89).

### 2.2 Kỹ năng — 100% Linux
- `server|client\settings\skills.txt`: 38 dòng 1347–1384 chép nguyên byte từ `D:\ServerLinux\server1\settings\skills.txt` (37 dòng khác bản JX1 cũ). Khác biệt Linux ↔ client VLTK (đã **không** lấy): 1349 thiếu addcolddamage_v/anti_hitrecover; 1352 VLTK là bản "Hỗ trợ bị động" khác hẳn; 1358 VLTK thêm 5 LvlSetting (huanyan_yunyan); 1369/1384 MaxLevel 26 (Linux) vs 27; 1376 thiếu 3 LvlSetting; 1378 Linux có meleedamagereturnmana_; 1382 AttackRadius 380/ChildSkillNum 3 (Linux) vs 520/4.
- `server|client\script\skill\huashan.lua`: chép nguyên byte Linux (808 dòng, chuỗi TCVN3, đã kiểm). Bản JX1 cũ 818 dòng có `--*` vô hiệu hoá nhiều thuộc tính → bỏ.
- `script\global\skills_table.lua`: thêm `add_hs(lvl)` (Linux :1033-1093).
- 59 tên thuộc tính trong huashan.lua đều có trong `KMagicDesc.cpp` của JX1 (kiểm ở bàn giao trước). **Chưa kiểm từng thuộc tính** hành xử trong engine JX1 có đúng công thức Linux hay không (mục 7).

### 2.3 Bảng khác
- `settings\RankSetting.txt` (server+client): +82–105 (VLTK; Linux thiếu rank 98 nên lấy VLTK cho đủ 3 phái).
- `settings\npcs.txt` (server+client): 33 NPC id 2087–2119 (dòng = id+1) chép từ Linux, **ánh xạ theo tên cột** (Linux 103 cột, JX1 87 cột — 16 cột Linux thừa bị bỏ: DropRateFile, *DamageBase, AuraSkill…, PasstSkill…). `NpcName=id.txt` +33.
- `settings\item\magicscript.txt` (server == client): +29 vật phẩm 4938–4966 ghi **nối đuôi** (ParticularType = số dòng − 1), bố cục 14 cột JX1; script `noscript.lua` (VLTK) → `0`. Bảng đổi mã (áp cho toàn bộ script port):

| VLTK/Linux | JX1 | Tên | | VLTK/Linux | JX1 | Tên |
|---|---|---|---|---|---|---|
| 4062 | 4938 | Sách 90 Kiếm Tông | | 3942–3959 | 4945–4962 | 18 vật phẩm nhiệm vụ Hoa Sơn 2013 |
| 4063 | 4939 | Sách 90 Khí Tông | | 2424 | 4963 | Đại Thành Bí Kíp 90 (`dachengmiji_90.lua` chép Linux) |
| 4325–4327 | 4940–4942 | Mật tịch 21/22/23 kỹ năng 150 | | 2425 | 4964 | Đại Thành Bí Kíp 120 |
| 4328 | 4943 | Hoa Sơn Ký Thư | | 30339 | 4965 | Lệnh bài Hoa Sơn Phái |
| 2908 | 4944 | Nguyệt Ca Lệnh | | 30341 | 4966 | Tín Vật Phong Thanh Dương |

  Icon: 2 icon `\spr\vng\item\{lenhbaihoason,tinvathoason}.spr` rút từ VLTK `serverlist.pak` ra đĩa client; 2 icon `\spr\item\zhishujie2011\*.spr` (item 4947/4948) không có ở cả hai pak → dùng `obj_item_lection.spr`.
- Sách: `script\item\skillbook.lua` (+[4938]/[4939], `faction_skill_list[10]`, chặn `nFact > 12`), `lvl120skillbook.lua` (+[10] = 1365).
- `script\header\factionhead.lua`: FACTION_INFO[11] = {2, "华山派", 3, 89, …}; SKILLNORMAL[11] 15 kỹ năng 10–70 theo `add_hs`; SKILL90_ARRAY[11] {1364, 1382}; SKILL120AR +1365; SKILL150_ARRAY[11] {1369, 1384}. `script\global\hocvocong.lua` bảng bản sao + `FACTION_TO_SKILL150[11]=11`. `lib_faction.lua` GetFactionEx/Ex2 + Hoa Sơn.
- `script\item\lenhbaitanthu.lua` chuyển phái: 11 lựa chọn, `nCurFac == 11 → SetSeries(2)`.

### 2.4 Bản đồ 987 (Hoa Sơn phái 2013)
- `bin\server\Pak\maps_hoason2013.pak`: 1 `.wor` + 308 `_region_s.dat` (`\maps\西北南区\华山派2013\v_087..105\074..099`) rút từ `D:\ServerLinux\server1\pak\maps.pak` (UCL NRV2B → RAW), đọc lại khớp 309/309.
- `Maps\WorldSet_GameServer.ini`: `Count=927`, `World926=987`; `package.ini`: `7=maps_hoason2013.pak`.
- Client: pak JX1 sẵn có `.wor` + 376/494 `_region_c` của map 987 (bàn giao trước) → **cần vào map thử**; ô thiếu region_c chỉ mất hình nền.

---

## 3. Script Hoa Sơn 2013 (Linux `script\global\huashan2013`, 79 tệp, tool `hs_port2013.py`)

Chép nguyên byte (TCVN3/GBK), mỗi tệp thêm dòng 1 `Include("\script\global\huashan2013\hs_shim.lua")`, đổi mã vật phẩm theo bảng 2.3 (131 chỗ), bỏ 5 Include thư viện JX1 không có (freshman_match\head, education\knowmagic, lv120skill\head, 150skilltask\g_task, daiyitoushi\toushi_function) — ghi comment tại chỗ.

`hs_shim.lua`: `G_TASK:OnMessage/Talk` (rỗng — nhiệm vụ kỹ năng 150 kiểu Linux không port, JX1 học bằng sách), `LV120_SKILL_ID = 2463` (task JX1 không dùng → nhánh 150 đóng), `daiyitoushi_main` (thông báo dùng Lệnh Bài Tân Thủ), `HaveCommonItem(g,d,p)=GetItemCount(0,g,d,p)`, `DelCommonItem=ConsumeItem(1,0,g,d,p)`, `IsNpcHide=0`, `M2g2Player`, `SetNpcAI→SetNpcAIType`, `ClearFactionRecord→ClearFactionIfnfo`, `no()`.

Thư viện kèm: `script\lib\progressbar.lua` **viết lại** (bỏ `IncludeLib("TIMER"/"FILESYS")`, nhúng `settings\progressconfig.txt` 16 dòng thành bảng; `OpenProgressBar` JX1 = `LuaHD3_OpenProgressBar` KJx2WarInfra.cpp:2103 chỉ dùng title/frame/OnTime), `script\lib\getrectangle_point.lua`, `script\global\repute_head.lua`, `script\item\dachengmiji_90|120.lua`, `settings\progressconfig.txt` chép Linux.

Spawn: `startgame.lua` Include `npc_hoason.lua` + gọi `add_npc_hoason()` trong `OnGame()` (trước `addfullnpc()`); `hs_add_npc` = bản sao Linux `global\vng\add_npc.lua:15`. Bảng Linux: quái luyện kiếm 2090 map 987, cường đạo 2092 map 3, Sơn Phỉ 2097 map 4, Hắc Diệp Linh Hầu 2103 map 167, sát thủ/giáo chúng Thiên Nhẫn 2108/2110 map 90/100, Khúc Vô Hình 2101 map 19, Điêu Tinh Bạch Hổ 2100 map 145; NPC map 987 (Lận Hạo Thiên, Nam Cung Tuyết, Vạn Tư Viễn…); **Hoa Sơn Kiếm Khách (2096) ở 8 thôn** 53/20/99/100/101/121/153/174 chạy `huashan2013\hoason.lua` (luồng gia nhập Linux: `SetFaction("huashan")`, `SetLastFactionNumber(10)`, `add_hs(10)`, task 3481). 3 dòng spawn tự chế ban đầu ở balanghuyen/giangtanthon/longmontran đã gỡ (tránh 2 NPC trùng).
`npcmonphai\hoason.lua` (khuôn conlon.lua, `gianhapmonphai(10)`) chỉ còn dùng cho **bot** và `hockynang` khi chuyển phái.

Rương chứa đồ 987: Linux `changefeature\npc\box.lua` không có → `global\npcchucnang\ruongchua.lua`. NPC Phong Thái Linh (`activitysys\npcdailog.lua`) JX1 không có → spawn nhưng bấm sẽ lỗi script (mục 7).

Task id dùng: 3481/3482/3486/3487/3488 (trống ở JX1), 69/169 (trống), 137/75/2885/1/6 dùng chung đúng nghĩa Linux (skills_table.lua JX1 = Linux; 2885 = trạng thái nhiệm vụ 150 cũng dùng ở metempsychosis).

---

## 4. Client UI bảng kỹ năng (Ui\Ui3)
- `UiSkillNew.ini` `[Main10]`/`[Main101024]` = bản sao `[Main9]` với `khung_hs.spr`; `UiSkillFly.ini` `[RemainPoint_10]`; `UiSkillFlySub.ini` `[Skill_10_i]`/`[SkillBtn_10_i]` (lưới Left 18/88/158/230/299 × Top 15/81/148/216/284/350, nút +24/+39, Image chép từ `[SkillBtn_9_0]`).
- Ô (index → kỹ năng): cột Kiếm Tông 0:1347 1:1351 2:1355 3:1360 4:1364(90) 5:1369(150); cột Khí Tông 6:1372 7:1376 8:1380 9:1382(90) 10:1384(150) 11:1358 (trấn phái); cột Hỗ Trợ 12:1349 13:1350 14:1354 15:1374 16:1375 17:1378; cột 4: 18:1379 19:1365(120) 20:1370 (tiến giai); 30: khinh công.
- `Spr\Ui3\UiSkills\khung_hs.spr` **vẽ mới** (PIL, 366×500, `khung_hs_make.py`, xem `khung_hs_preview.png`). Lý do không sửa từ `khung_cl.spr`: bảng màu 10 tệp `khung_*.spr` là dữ liệu lạ (giải mã ra nhiễu dù cấu trúc/offset đúng header 32 byte; `nut_dong.spr` cùng thư mục giải mã đúng) → không biết màu gốc để vẽ chữ. Bộ đọc/ghi `spr_hs.py` đã kiểm bằng `nut_dong.spr` và `Ui4\common\关闭.spr`.

---

## 5. Kiểm tra đã chạy
1. `syncheck.exe` (Lua 4) 100 tệp mới/sửa (server + client huashan.lua): 100 cú pháp OK.
2. `t71_quet_goi_nil_thanchunk.py`: 0 chỗ gọi hàm nil ở thân chunk.
3. Quét `EF BF BD` 18 tệp engine: sạch. Dry-run `--kiem` của 3 tool trước khi ghi.
4. Pak map đọc lại 309/309. magicscript giữ luật "id = số dòng − 1" (assert trong tool).

**Chưa chạy trong game** (chủ swap rồi thử): gia nhập tại Hoa Sơn Kiếm Khách → về map 987 → học kỹ năng qua `add_hs` → bảng kỹ năng F… hiển thị khung Hoa Sơn → đánh thử 1347/1372/1358 → chuyển phái bằng Lệnh Bài Tân Thủ → thống kê Goddess.

---

## 6. Vũ Hồn (11) / Tiêu Dao (12) — chưa làm
Engine đã có chỗ (id, tên TCVN3, icon tổ đội, bot table); FactionInfo chưa có mục [11]/[12] (loader bỏ qua mục trống). Cần: skills.txt (VLTK, Linux không có), skill lua, faction ini, npcs, item/trang bị mới + ảnh (bàn giao trước mục 6-8), UI khung, script gia nhập.

---

## 7. Rủi ro / việc chờ chủ quyết
1. **Thuộc tính kỹ năng**: dữ liệu = Linux 100%, nhưng engine JX1 xử lý từng thuộc tính có đúng công thức Linux không thì chỉ mới kiểm một phần ở các đợt trước (anti_hitrecover, add_damage_p, khiên…). Muốn "chức năng 100%" phải mổ thêm các thuộc tính Hoa Sơn dùng: `attackspeed_yan_v`, `lifemax_yan_p`, `meleedamagereturnmana_*`, `anti_sorbdamage_yan_p`, `enhancehiteffect_rate`, `fatallystrikeres_p`, `skill_misslenum_v`… (mỗi cái 1 phiên đối chiếu asm).
2. Nhiệm vụ kỹ năng 150/120 kiểu Linux (G_TASK, LV120) không port — JX1 dùng sách (4938/4939, 1365 qua lvl120skillbook, 150 qua hocvocong).
3. Đổi phái "Đại Nghệ Đầu Sư" Linux vẫn chờ chủ quyết (bàn giao đổi tên/đổi phái 01/09) → shim thông báo.
4. NPC Phong Thái Linh map 987 thiếu script activitysys; Rương chứa đồ dùng bản JX1.
5. Bot chưa chọn Hoa Sơn (KPlayerBot.cpp:1291 xoay hệ×2+k).
6. Goddess thống kê theo phái nay đếm cả Thiếu Lâm (id 0) — thay đổi số liệu báo cáo so với trước.
7. Client map 987 thiếu 118 region_c trong pak JX1 (chưa rút từ VLTK).
8. Bảng `FactionName` KNpc.cpp trước đây hỏng mã hoá → tên phái trên đầu nhân vật (nếu đang dùng bảng này) sẽ đổi sang chữ đúng.

---

## 8. CHECKLIST SWAP (chủ chạy `ChoiGame.bat` / `ChayGameServer.bat`; KHÔNG tự restart)
Bộ 4 tệp `.moi` build từ cây `D:\GAMEDEVNEW` HEAD `3ea76dc1` + vá Hoa Sơn (superset bộ bot 01/09 đã swap lúc 19:01):

1. `bin\server\CoreServer.dll.moi` — 18.244.608 byte, md5 `eed8dff8` (Server Release x64, 23:15, gồm đợt b mục 10).
2. `bin\multiserver\Goddess.exe.moi` — 2.386.432 byte, md5 `73f10c62` — **PHẢI swap cùng lúc với CoreServer** (cỡ `TStatData`/`TGAME_STAT_DATA` đổi 11→14; lệch nhau thì bảng xếp hạng/thống kê bị từ chối).
3. `bin\client\CoreClient.dll.moi` — 2.439.680 byte, md5 `fa3f667c` (Client Release Win32, 23:15, gồm giao thức `s2c_reduceskillcd` — PHẢI đi cùng CoreServer mới).
4. `bin\client\Game.exe.moi` — 1.373.696 byte, md5 `c1b575f8` (Release Win32) — cùng bộ với CoreClient (MAX_FACTION_NUM, bảng kỹ năng).
5. Dữ liệu/script đã ghi thẳng vào cây chạy thật (server: settings, Maps, package.ini, Pak, script; client: settings, Ui, Spr, script\skill) — có bản sao `.truoc_hoason_0109` cạnh tệp cũ. Server đọc khi khởi động lại; client đọc khi mở lại.
6. Sau swap: xem console GameServer có `ScriptError` không; kiểm `World926=987` nạp (log "Load map 987"); vào game với nhân vật hệ Thủy chưa phái → Ba Lăng Huyện (1632,3191) gặp Hoa Sơn Kiếm Khách.

---

## 9. Tool (ReverseTools\phai3\hoason_thicong)
`hs_engine_patch.py` (engine, `--kiem`), `hs_data1.py` (dữ liệu + script khung), `hs_port2013.py` (79 tệp + shim + item + startgame), `hs_map_pak.py` (pak map 987), `spr_hs.py` + `khung_hs_make.py` (SPR), `hs_scan.py` (quét phụ thuộc), `fix1-4.py` (vá tool), `hs_items_vltk.tsv` (36 cột VLTK của 29 item), `khung_hs_preview.png`.


---

## 10. ĐỢT b (01/09 khuya) — bỏ nhiệm vụ + KIỂM TOÁN 46 THUỘC TÍNH KỸ NĂNG THEO LINUX

Chủ quyết: *"không cần nhiệm vụ, chỉ cần nhận skill như các phái có sẵn"* và *"chủ yếu làm skill đúng chuẩn Linux"*.

### 10.1 Gia nhập / nhận kỹ năng như 9 phái cũ (tool `hs_simplify.py`)
- 74 tệp nhiệm vụ `script\global\huashan2013\*.lua` đưa ra kho `ReverseTools\phai3\hoason_thicong\huashan2013_nhiemvu_linux_khongdung\` (không nạp lúc boot). Còn lại: `npc_hoason.lua` (bảng spawn rút gọn), `hs_shim.lua`, `trap\` (4 tệp bẫy của map 987).
- `npc_hoason.lua`: Hoa Sơn Kiếm Khách (2096) ở 8 thôn (53/20/99/100/101/121/153/174, toạ độ Linux) + Nam Cung Tuyết (2098) và Lận Hạo Thiên (2096) ở map 987 + Rương chứa đồ — tất cả chạy `npcthon
pcmonphai\hoason.lua` (khuôn conlon.lua): vào phái = `gianhapmonphai(10)` → SetFaction/Camp/Rank + `hockynang` (SKILLNORMAL[11] 15 chiêu 10–70), kỹ năng 9x qua `hotrokn` (SKILL90_ARRAY[11]), 120 sách `lvl120skillbook` [10], 150 qua `hocvocong`, xuất sư / trùng phản như cũ. `factionhead.lua hotrokn` nới `nCurFac > 10` → `> 12`.

### 10.2 Kiểm toán thuộc tính (46 tên trong 38 dòng skills.txt Linux 1347–1384)
Phương pháp: tên → enum `KMagicAttrib.h` → đếm chỗ dùng ngoài bảng tên (`KNpcAttribModify.cpp`, `KSkills.cpp`, `KNpc.cpp`…). Kết quả: **32 có mã xử lý**, **12 có tên nhưng KHÔNG có mã** (chết im), **2 không có cả tên** (`skill_skillexp_v`, `addskillexp1`).

Dịch ngược `jx_linux_y` (tool `lin_re.py`): bảng tên thuộc tính Linux = mảng `char*` tại VA `0x830e640` (điền bởi mã `0x80724dd..`), bảng handler `KNpcAttribModify` `[this+4+8*idx]` điền tại `0x8099a36..` (kiểm bằng do_stun_p 261 → `0x080968F0`). Kết quả từng thuộc tính chết:

| Thuộc tính (Linux idx) | Handler Linux | Ngữ nghĩa dịch ngược | JX1 sau đợt b |
|---|---|---|---|
| `meleedamagereturnmana_p` (286) / `rangedamagereturnmana_p` (287) — 1378 Khí Chấn Sơn Hà {10..25, −1} | `0x080963D0`/`0x080963F0`: `[0x137c]`/`[0x1380] += v0` trên NPC mang thuộc tính | Hàm sát thương `0x08089C90` (`0x08089F19–0x08089F5F`, nhánh xa `0x0808A240`): **sau khi trừ máu NẠN NHÂN**, `mana(KẺ ĐÁNH) += sát_thương × p / 100` (cắt lẻ, `[ebp+0x1c]` = bIsMelee chọn melee/range), âm → 0 (`0x0808A390`), không kẹp trần | handler `MeleeDamageReturnManaP`/`RangeDamageReturnManaP` + `KNpc::CalcDamage` (thêm kẹp trần mana tối đa). ⚠️ Với giá trị dương của 1378, **kẻ đánh trúng người Hoa Sơn được cộng nội lực** — đúng như asm Linux dù mô tả kỹ năng nói "tiêu hao nội công đối thủ" |
| `addblockrate` (292) — 1370 Hạo Nhiên Chi Khí {10→3, −1, 1→2} | `0x08096430`: `[0x1388] += v0`, `[0x138c] += v2` | `0x0808C078` (tính lại thuộc tính): nếu v0>0 && v2>0 → `[0x1390] = min(25, Random(256)/v0 × v2)`; `0x0808B2C0` (ReceiveDamage): `nBlock = [0x1390] + block_rate[0x1408] − anti_block_rate(kẻ đánh)` | handler `AddBlockRate` + `KNpc::ReceiveDamage` cộng `nKMAddBlock` vào hoá giải (JX1 tung mỗi lần bị đánh thay vì mỗi lần tính lại thuộc tính — cùng phân bố) |
| `reduceskillcd1/2` (288/289) — 1347/1348/1351/1353/1355/1357/1360 {id kỹ năng, 0, 6..18 khung} | `0x08097250(this, pLauncher, pNpc, attr)` | `KSkillList(NGƯỜI PHÁT)::ReduceCD 0x080E4740`: ô có SkillId: `NextCastTime > f → −= f`; `f < WaitCastTime → −= f`; rồi gửi gói `0xdd` (attr) cho client | `KSkillList::ReduceCoolDown` + chặn trong `KNpc::ModifyAttrib` (có nAttacker) + **giao thức mới `s2c_reduceskillcd`** (`S2C_REDUCE_SKILL_CD {BYTE; WORD skill; WORD frames}`, đặt sau `s2c_syncfusion` — không đổi số các giao thức cũ; handler client `s2cReduceSkillCD`) |
| `candetonate1/2/3` (295–297) — 1352 {323/326/329·256+1, 10..128}, 1356, 1373 {419·256}, 1377 {421·256, 360}, 1381, 1384 {419/428·256, 100..560} | `0x08097110` → `0x08079870(launcher, style=v0>>8, radius=v2, flag=v0&0xff, region)` + 8 vùng kề `[region+0x78+i*4]` | duyệt danh sách đạn của vùng: `m_nMissleId == style`; quan hệ chủ đạn ↔ người phát: cờ 0 → self|ally (`test al,6`), cờ 1 → enemy (`test al,8`); `sqrt(dx²+dy²)` (mps) ≤ radius → `0x08075210(m, 1)` = sự kiện tan của kỹ năng mẹ (`DoEvent(4)` = VanishedEvent, VD 1380 → 1411) + `GWM_MISSLE_DEL` + trạng thái tan | `KNpc::DetonateMissles` + `KMissle::Detonate()` (= `DoVanish`, JX1 đã làm VanishedEvent + GWM_MISSLE_DEL) chặn trong `ModifyAttrib`. Ý nghĩa: 1352 "phá đao kiếm" tan đạn địch 323/326/329; Khí tông kích nổ kiếm khí phe ta (419/421/423/428 — Linux dùng lại 4 dòng "NPC chiến đấu" làm đạn kiếm khí, JX1 có sẵn y hệt) |
| `addskilldamage1-3` (304–306) | không có handler (thuộc tính mức kỹ năng) | `KSkill::Parse 0x080EE1EC` lưu {id, ?, %} vào mảng KSkill+0x120; khi thi triển kỹ năng X cộng % từ các kỹ năng khác có addskilldamage trỏ tới X | **JX1 đã có**: `KSkills.cpp:2510` lưu `m_AddSkillDamage`, `KSkillList::GetAddSkillDamage` cộng `nValue[2]`, dùng ở `KNpc::AppendSkillEffect` |
| `skill_skillexp_v` (8) — 1363/1364/1365/1368/1369/1382/1384 | không có handler | `KSkill::Parse 0x080EE278` lưu vào KSkill+0x11c; `KSkillList 0x080E4F9D/0x080E5F21` dùng làm **ngưỡng exp mỗi cấp** (tiến độ = exp×1024/ngưỡng) | JX1 dùng bảng `settings
pc\player\magic_level_exp.txt` (`KMagicLevelExp::GetNextExp`, `IsExpSkill=1` đã có trong dòng skills.txt) → **+5 dòng 1364/1365/1369/1382/1384 tính từ Lua** (`hs_exp_table.py`; 5 dòng cùng id trong tệp Linux có giá trị KHÁC Lua và Linux không đọc tệp đó cho các kỹ năng này) |
| `addskillexp1` (73) — 1382 {0,1,0} | **không có handler ở Linux** | chết ở cả Linux | bỏ (đúng Linux) |

32 thuộc tính còn lại: JX1 có handler (`KNpcAttribModify` / `KSkills`) — chưa đối chiếu công thức từng cái với asm trong đợt này (các đợt trước đã kiểm `anti_hitrecover`, `add_damage_p`, khiên, Khí Doanh…).

### 10.3 Engine đợt b (`hs_engine_patch2.py`, marker `[HOASON 01/09b]`)
`Headers\KProtocolDef.h` (+`s2c_reduceskillcd`), `KProtocol.cpp` (kích thước), `KProtocol.h` (struct), `KProtocolProcess.h/.cpp` (đăng ký + handler client), `KSkillList.h/.cpp` (`ReduceCoolDown`), `KMissle.h` (`Detonate`), `KNpc.h` (4 trường + `DetonateMissles`), `KNpcAttribModify.h/.cpp` (3 handler + đăng ký), `KNpc.cpp` (reset ×2, hoá giải, hồi nội lực trong `CalcDamage`, `DetonateMissles`, `ModifyAttrib`). Kiểm lưới giao thức (`kiem_luoi_giaothuc.py`): 7 chỗ lệch **như trước** (chú thích cũ), không lệch mới.


---

## 11. ĐỢT c (01/09 khuya) — "đổi phái chỉ có 9x/12x, không có 1x–6x"

**Nguyên nhân (bằng chứng `bin\server\ScriptError.log` 23:23:55):** `hockynang` tại `global\hocvocong.lua:774` lỗi `getn(nil)`, gọi từ `hvccl` (NPC Hỗ Trợ Test → "Học võ công"). `hocvocong.lua` có bảng `SKILLNORMAL` **riêng** (10 phái) khác bảng trong `factionhead.lua`; đợt 1 tôi chỉ nối `SKILL90/120/150` ở tệp này, bỏ sót `SKILLNORMAL`. Ngoài ra, vì `lenhbaitanthu.lua` Include `hocvocong.lua` SAU `factionhead.lua`, hàm `hockynang` của `hocvocong.lua` **đè** bản `factionhead.lua` trong luồng đổi phái (`doiphai1`) — bản này chỉ phát 9x (cấp 20) + 12x + khinh công qua `show_kynang90` (vòng phát 1x–6x vốn bị comment). Chủ game đã tự bỏ comment vòng đó và đổi `hvccl` sang `hockynang(11)` (23:23:40) → chạm bảng thiếu → lỗi. **Không liên quan mã bot**: `PB_OnRoleData` (xoá kỹ năng mẫu) chỉ chạy cho gói DB của bot (`CoreServerShell.cpp:1046 SSOI_PBOT_ROLEDATA_RES`), `PB_IsBot` so cả `dwID`; lưu/nạp DB (`UpdateDBSkillList`/`LoadPlayerFightSkillList`) giữ cả kỹ năng cấp 0.

**Đã sửa (tool `hs_hocvocong.py`, `hs_hocvocong_fix.py`, `hs_script2.py`, marker `[HOASON 01/09c]`):**
- `global\hocvocong.lua`: `SKILLNORMAL[11]` 15 chiêu 10–70; `hvccl` trả về `hockynang(10)` (Côn Lôn); thêm `hvchs` + mục menu "Học võ công môn phái Hoa Sơn"; `tbAllSkill2.huashan`; menu test 90 thêm "Hoa Sơn 90/skillhoason" (`add_hs(90)`).
- `skill\skillfaction.lua` (NPC nâng trấn phái): tên GBK 华山派 → 11, `mainskill[11] = {1358}`.
- `skill
angskillkieumoi5x|9x|tp.lua` (NPC lĩnh ngộ): hàng 11 Hoa Sơn ({1349,1374}→{1355,1379}; 9x {1364,1382}; trấn phái {1347,1372}→1358) + nhánh tên phái.
- `item\daithanhbk90.lua` `[10] = {1364, 1382}`, `daithanhbk120.lua` `[10] = {1365}` (sách Đại Thành 2433/2434).
- `lib\lib_faction.lua`: `tbSkillBase[11]`, `tbSkill120` +1365, `tbSkill150` +{1369,1384}, nhánh 2 kỹ năng (`add_skill_90`/`add_skill_150`) nhận id 11.
- `npcthonalanghuyen\hotrotest.lua` (PUBG): hệ Thủy có thể trúng Hoa Sơn khi chưa phái.
- Không sửa (đúng với Linux / đã hỏng sẵn): `skills_table.lua check_faction/update_*` (Linux cũng không có nhánh Hoa Sơn), `FactionHelper.lua skill_help` (`fact_num = 1` cứng), `simcity_admin.lua` bot chọn phái 0–9.

Kiểm: `syncheck` 9 tệp OK, `t71` 0 lỗi. Không cần build lại binary.
