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

1. `bin\server\CoreServer.dll.moi` — 18.245.120 byte, md5 `f2dcad97` (Server Release x64, 02/09 00:00, gồm đợt b mục 10 + đợt d mục 12.3).
2. `bin\multiserver\Goddess.exe.moi` — 2.386.432 byte, md5 `73f10c62` — **PHẢI swap cùng lúc với CoreServer** (cỡ `TStatData`/`TGAME_STAT_DATA` đổi 11→14; lệch nhau thì bảng xếp hạng/thống kê bị từ chối).
3. `bin\client\CoreClient.dll.moi` — 2.439.680 byte, md5 `45ce8b62` (Client Release Win32, 02/09 00:00, gồm giao thức `s2c_reduceskillcd` — PHẢI đi cùng CoreServer mới).
4. `bin\client\Game.exe.moi` — 1.373.696 byte, md5 `c1b575f8` (Release Win32) — cùng bộ với CoreClient (MAX_FACTION_NUM, bảng kỹ năng).
4b. `bin\client\WAuto.exe.moi` — md5 `ab6f9286` (mục 12.1; tắt WAuto cũ rồi thay).
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


---

## 12. ĐỢT d (02/09 rạng sáng) — WAuto không hiện buff Hoa Sơn · "thiếu hiệu ứng / tác dụng" · thuộc tính chưa đúng Linux

### 12.1 WAuto: combo "skill buff hỗ trợ" không có kỹ năng Hoa Sơn
- Nguồn danh sách: client `KSkillList::GetAllSkillByType` (`IPCSkillInfo`: nStyle, bAlly = !TargetEnemy, bState = `StateSpecialId > 0`, bAura). WAuto (`E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.cpp:2651`) xếp vào "hỗ trợ" khi `bState && bAlly`. Ba buff Hoa Sơn 1358 Huyễn Nhãn Vân Yên, 1364 Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm, 1369 Cửu Kiếm Hợp Nhất là kiểu 2 (trạng thái chủ động, nhắm bản thân) nhưng dữ liệu Linux **và** VLTK đều để `StateSpecialId = 0` → bị loại.
- Sửa trong bộ lọc có sẵn: `(bState || nStyle == 2) && bAlly`. Tác dụng phụ (tốt): 4 buff 120 của phái khác cùng tình trạng nay cũng chọn được (712 Bế Nguyệt Phất Trần, 713 Ngự Tuyết Ẩn, 715 Ma Âm Phệ Phách, 722 Lưỡng Nghi Chân Khí Gia Tốc). Build `WAuto.vcxproj` Release|Win32 → `bin\client\WAuto.exe.moi` (md5 `ab6f9286`). Không đổi engine.

### 12.2 "Thiếu hiệu ứng": rà 83 kỹ năng liên quan (38 Hoa Sơn + kỹ năng con/đạn/sự kiện) — 127 đường dẫn sprite/âm thanh
- Chỉ thiếu 2 tệp phụ trong pak JX1 (không có cả ở VLTK): `\sound\skill\刀剑刺中声.wav`, `\spr\skill\天忍\mag_tr_09_偷天换日.spr` (nhánh Thiên Nhẫn). Hiệu ứng Hoa Sơn không thiếu tệp.
- Nhưng 7 dòng kỹ năng con/đạn mà Hoa Sơn dùng chung **khác bản Linux** trong `skills.txt` JX1 → chép lại theo Linux (server + client, bản cũ `.truoc_hoason_0109b`): 203 Vô Hình Độc (nổ của kiếm khí 419: JX1 thiếu `fastwalkrun_p`, thừa `skill_eventskill`), 64 Bằng Lam Huyền Tinh (+`coldresmax_p`), 69 Vô Hình Độc (+`fastwalkrun_p`), 92/208 Phật Tâm Tự Hựu và 275 Sương Ngạo Côn Luân (+`lifemax_yan_p`), 1420 Kiếm Pháp Thái Nhạc (ReqLevel 80→90 như Linux). Các dòng này thuộc phái khác nên hành vi của họ cũng về đúng Linux.

### 12.3 Thuộc tính: so handler Linux (bảng `[this+4+8*idx]`) với JX1 cho 27 thuộc tính còn lại
| Thuộc tính | Linux | JX1 | Kết luận |
|---|---|---|---|
| `attackspeed_yan_v` (239) | `[0x1a38] += v0` = chính trường tốc độ đánh (được chép vào gói sync) | `AttackSpeedV` | trùng |
| `lifemax_yan_p` / `manamax_yan_p` (234/236) | `[0x1a18] += LifeMax[0x15ac]*v0/100` (0x1a18 = sinh lực tối đa hiện tại, kẹp bởi [0x1a14]) | `LifeMaxP`/`ManaMaxP` | trùng phần gốc |
| `manatoskill_enhance` (298) — 1379 Khí Quán Trường Hồng {30..100} | handler đặt cờ [0x1398] + giá trị [0x13a0]; lúc tính lại thuộc tính (0x0808C002-4D): `[0x139c] = v × (nội lực × 100 / nội lực tối đa) / 100`; 0x080EA0DE cộng `[0x139c] + skill_enhance[0x1280]` vào % sát thương kỹ năng | **chỉ cộng khi nội lực ĐẦY 100%** (`m_CurrentMana == m_CurrentManaMax`) → gần như vô tác dụng | **SAI → đã sửa** `KNpc::AppendSkillEffect`: `+= v × (mana×100/manaMax)/100` (`[HOASON 01/09d]`) |
| `walkrunshadow` (293) — 1358 | cờ `[0x1394] = v0 > 0` | `m_WalkRun.nTime += v1` (thời gian) | tương đương (Lua cho [2] = thời gian) |
| `manareplenish_p` (248) — kiếm tông −200 | `[0x119c] += v0`, [0x119c] mặc định 100 (%) | `Replenish × (100 + p)/100` | trùng |
| `sorbdamage_yan_p`, `anti_do_hurt_p`, `skill_enhance`, `autoattack/reply/rescueskill` | += v0 (sorb kẹp 500) | tương tự | trùng |
| `deadlystrike_p` → `m_CurrentDeadlyStrikeEnhanceP` | — | theo đợt chí tử `d22b24ca` (Linux có lỗi đấu chéo, JX1 giữ đúng) | giữ |
| 12 thuộc tính gốc (colddamage_v, physicsenhance_p, seriesdamage_p, lifereplenish_v, lifemax_p, coldenhance_p, damage2addmana_p, fastwalkrun_p, addcoldmagic_v, addphysicsdamage_p, deadlystrikeenhance_p, attackspeed_v) | không nằm trong bảng handler Linux (xử lý ở switch gốc) | cùng gốc mã | chưa đối chiếu asm |

Không có lỗi nạp kỹ năng trong log server/client (không dòng "Cap ky nang … da xay ra loi", không lỗi `huashan.lua`).


---

## 13. ĐỢT e (02/09) — "Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm ở Linux bắn nhiều kiếm liên tục"

**Cơ chế Linux:** 1364 (buff tự thân, kiểu 2) mang `autoreplyskill` = {v0 = (1·65536 + 1363)·256 + cấp, v1 = 10 phút, v2 = 15·18·256 + 3}: khi **bị đánh**, 3 % tỷ lệ tự phóng 1363 Thái Nhạc Tam Thanh Phong (đạn 3 kiếm, `MisslesForm 2`, `ChildSkill 418 ×3`) cấp = cấp buff, hồi chiêu 15 s. 1369 Cửu Kiếm Hợp Nhất tương tự với `autoattackskill` → 1368 Độc Cô Cửu Kiếm (9 kiếm, `skill_misslenum_v`). Handler Linux `0x080973D0`: id = (|v0| & 0xFFFFFF) >> 8, cấp = v0 & 0xFF, **loại = |v0| >> 24** (1 = nhắm kẻ đánh), chờ = v2 >> 8, tỷ lệ = v2 & 0xFF. Hàm bắn `0x08188BB0 Fire(list, chủ, kẻ đánh)`: tỷ lệ > Random(100) → lấy skill → `loại == 1 ? mục tiêu = kẻ đánh : chính mình` → `Cast(chủ, -1, mục tiêu)`. Không có cổng cấp.

**JX1 sai ở 3 chỗ (đã sửa, marker `[HOASON 01/09e]`, tool `hs_engine_patch3.py` + `3b`):**
1. `KNpcAttribModify::AutoReplySkill/AutoAttackSkill/AutoRescueSkill`: `nSkillId = nValue[0] / 256` không mặt nạ byte loại → 1364 cho id **66899** (≥ MAX_SKILL) → `ReplySkill()` bỏ qua → **không bao giờ bắn**. Nay `(v0 & 0xFFFFFF) / 256` + lưu `nType = v0 >> 24` (`KMagicAutoSkill` thêm trường).
2. `KNpc::ReplySkill()` bắn `this->Cast(id, cấp)` không mục tiêu; nay `ReplySkill(nAttacker)`: loại 1 → `pSkill->Cast(m_Index, -1, nAttacker)` (3 kiếm bay về kẻ đánh như Linux); loại khác giữ đường cũ (buff lên mình) để 9 phái kia không đổi.
3. Cổng `m_Level >= LEVEL_EXPLOSIVE (120)` trước `ReplySkill/RescueSkill/AttackSkill` (KNpc.cpp:3649/3656): Linux không có → bỏ (1364 là chiêu 90; các auto-skill 120 của phái khác vẫn cần học ở 120 nên không đổi).

Đã kiểm 65 dòng skills.txt Linux dùng auto*skill: chỉ Hoa Sơn 1364 có byte loại; phái khác v0 = id·256 + cấp → sau mặt nạ vẫn đúng.

**Bộ nhị phân đợt e (02/09 00:17, commit `40e3be2e`):** `bin\server\CoreServer.dll.moi` md5 `9d7ae996` (18.245.632 B) + `bin\client\CoreClient.dll.moi` md5 `741b2d5b` — PHẢI swap cùng nhau (giao thức s2c_reduceskillcd đợt d). Game.exe/Goddess.exe `.moi` đợt d đã được chủ swap lúc 00:00 (không còn `.moi`), không cần swap lại.


---

## 14. ĐỢT f (02/09 rạng sáng) — "skill như trên hình (1364) vẫn chưa có tác dụng" → kiểm toán chức năng 38 kỹ năng theo Linux

### 14.1 Bằng chứng log (bin\server\jx_auto_server.log.1, máy chủ pid 27612 khởi động 00:19:05 với CoreServer đợt e md5 9d7ae996)
- Người chơi npc 91434 (plr=1) cast 1364 mười lần (`[S4-CAST] … skill=1364 style=2`), buff áp thành công (`E3_INIT_DAMAGE_RESULT hit=1 … state_num=3`).
- Tự phóng ĐÃ chạy đúng 1 lần: t=614552736 ba đạn 1363 (msl 1747/542/651, `lch=91434`) mà KHÔNG có `[S4-CAST] skill=1363` → do `ReplySkill`; 2 đạn trúng quái 53815 (30000→18825), 1 đạn bay hết 34 khung.
- Sau đó không bắn thêm dù bị đánh liên tục (52 dòng E2-CALC, throttle 1 s) — đúng với dữ liệu Linux: tỷ lệ 3 %/đòn, hồi chiêu 15 s (`15*18*256+3`).
- **Gốc "không thấy tác dụng"**: JX1 KHÔNG đồng bộ từng viên đạn; client chỉ tự mô phỏng đạn khi nhận `s2c_skillcast` (`NetCommandSkill` — bỏ qua chính người chơi qua `ConformIdx`) hoặc `s2c_castskilldirectly` (`s2cDirectlyCastSkill`: nMpsX = -1 → nMpsY là ID mục tiêu, áp cho cả bản thân). `ReplySkill` đợt e chỉ `Cast` trên máy chủ → mọi client (kể cả chủ nhân) không thấy 3 kiếm, chỉ thấy quái mất máu. Linux `Fire 0x08188BB0` sau `Cast` (0x080EA920) phát gói skillcast 0x85 (0x0807A870, 25 byte) cho cả vùng.

### 14.2 Sửa engine đợt f — tool `ReverseTools\phai3\hoason_thicong\hs_engine_patch4.py` + `hs_engine_patch4b.py` (idempotent, `--kiem`), marker `[HOASON 02/09]`, commit `c0d787bd` + `3e7a5c38` (sửa theo phản biện)
| Tệp | Sửa | Linux đối chiếu |
|---|---|---|
| `KNpc.cpp` | thêm `KNpc::CastAutoSkillAt(id, cấp, mục tiêu)`: phát `NPC_SKILL_SYNC s2c_castskilldirectly` (nMpsX=-1, nMpsY=dwID mục tiêu) cho vùng + 8 vùng kề, rồi `pSkill->Cast(m_Index, -1, target)`, đặt hồi chiêu server như `KNpc::Cast(int,int)` (client `s2cDirectlyCastSkill` luôn SetNextCastTime) | Fire 0x08188D4C Cast → 0x0807A870 phát gói |
| `KNpc.cpp` | `ReplySkill(nLauncher)`: loại 1 → `CastAutoSkillAt(kẻ đánh)`, khác → `Cast(id,cấp)` (đã phát castskilldirectly). `RescueSkill(nAttacker)`: loại 1 → kẻ đánh, khác → mình. `AttackSkill(nạn nhân)`: loại 1 → **chính mình**, khác → nạn nhân | Fire: `cmp node->[0x38],1; cmovne ecx, ebx` (0x08188D0B) |
| `KNpc.cpp` | Tự phóng CHUYỂN từ `CalcDamage` (mỗi hệ lạnh/hoả/lôi/độc + mỗi nhịp độc) sang `ReceiveDamage`: MỘT lần mỗi đòn qua cửa trúng (`CheckHitTarget`), chỉ khi quan hệ ĐỊCH, sau khi trừ máu, trước khe choáng; KHÔNG gate theo sát thương từng hệ (phản biện: `[ebp-0x48]` Linux là độ dịch kháng ngũ hành, không phải kết quả đòn) | ReceiveDamage 0x0808AACF `cmp [ebp-0x5c],8` ([ebp+0x24]&0xC), 0x0808B4F9 reply, 0x0808B1D3 attack |
| `KNpc.cpp` | `autorescueskill` chỉ bắn khi ĐÒN NÀY đưa máu từ ≥ 25 % max xuống < 25 % và còn sống (đặt tại khe trừ máu của `CalcDamage` và khe chí tử của `ReceiveDamage`); bản cũ bắn mỗi đòn khi máu đang < 25 % | BeHurt 0x0808A003-0x0808A01A, ReceiveDamage 0x0808B0E3-0x0808B13A (`max/4`, `old >= 25%`) |
| `KNpcAttribModify.cpp` | 4 handler auto*skill dùng chung `HS_AutoSkillModify`: khoá = (|v0| & 0xffffff) = id·256+cấp, loại = |v0|>>24; v2>0 cộng dồn tỷ lệ (v2&0xff), chờ = v2>>8; v2<0 (gỡ trạng thái, giá trị đảo dấu) trừ tỷ lệ, về 0 → xoá ô. Sửa 2 lỗi cũ: gỡ trạng thái tạo ô rác id 64172 chiếm chỗ (3 ô); ô thêm lần đầu không bao giờ bị gỡ → buff hết vẫn tự phóng, buff lại → 2 ô = tỷ lệ đôi | 0x08189000 (map theo khoá, `node->[0x14] += rate`, `== 0` erase) |
| `KNpc.cpp` nhịp hồi | nội lực `+= m_CurrentManaReplenish` (gốc + Σmanareplenish_v) KHÔNG nhân %; `manareplenish_p` chỉ nhân vào thuốc hồi nội lực theo thời gian (`m_ManaState`); sinh lực: % chỉ nhân khi tổng hồi > 0, hồi âm cộng thẳng | 0x0808B6FA `[0x11a0] += [0x11a4]`; 0x0808B826 "AddManaState: %d * %d%%"; 0x0808B65F-6BD "AddLife" (`jle` → cộng thẳng) |
| `KMagicDesc.cpp` | `#lA-` lấy `(v0 & 0xffffff)/256` → mô tả 1364 hiện "[ Thái Nhạc Tam Thanh Phong ]" (trước ra id 66899 → trống, đúng như ảnh chủ gửi) | byte cao = loại (0x080973D0) |
| `KNpc.cpp` | `DeathSkill`: lỗi gõ cũ kiểm `m_ReplySkill[i]` thay vì `m_DeathSkill[i]` | — |

Hệ quả cho Kiếm Tông: 1349/1364/1369 mang `manareplenish_v = -10000, manareplenish_p = -200`; công thức cũ `R + R·p/100` = (gốc−10000)·(−1) = **+9950 nội lực mỗi nhịp** (đầy nội lực vĩnh viễn — ngược Linux); nay rút về 0 mỗi nửa giây như Linux (Kiếm Tông không dùng nội lực, chiêu Kiếm Tông `skill_cost_v` bị comment). Các phái khác `p = 0` → không đổi; `lifereplenish_p` chỉ khác khi tổng hồi âm.

### 14.3 Kiểm toán thuộc tính bổ sung (dịch ngược bảng handler `[this+4+8*idx]` đầy đủ 216 mục — scan `mov [edx+off], func` 0x08099000-0x0809B000; đợt b/d chỉ thấy 109)
| Thuộc tính (idx) | Linux | JX1 | Kết luận |
|---|---|---|---|
| steallife_p 66 / stealmana_p 67 | `[0x13dc]/[0x13e0] += v0` | `m_CurrentLifeStolen/ManaStolen += v0` | trùng |
| deadlystrike_p 70 / deadlystrikeenhance_p 146 | CÙNG handler 0x08098BD0 `[0x13fc] += v0` | `DeadlyStrikeP`/`DeadlyStrikeEnhanceP` đều `+= m_CurrentDeadlyStrikeEnhanceP` | trùng |
| attackrating_p 57 / attackratingenhance_p 167 | `[0x1258] += base[0x15c4]·v0/100` | `AttackRatingP` + phần kinh mạch JX1 | trùng gốc |
| castspeed_v 116 | `[0x1a3c] += v0` | `m_CurrentCastSpeed += v0` | trùng |
| addlightingdamage_v 124 / addlightingmagic_v 171 / addcoldmagic_v 169 | `min += v0; max += v0` (KHÔNG đặt thời gian) | JX1 `AddColdMagicV` còn đặt nValue[1] bảng 16 bậc | **lệch** (mục 14.4-2) |
| manareplenish_v 92 | `[0x11a4] += v0` → nhịp cộng thẳng | đã sửa nhịp (14.2) | trùng sau vá |
| lifemax_p 86 → `[0x1a14]`, lifemax_yan_p 234 → `[0x1a18]`; manamax_p 90 → `[0x1a1c]`, manamax_yan_p 236 → `[0x1a20]`; attackspeed_v 115 → `[0x1a34]`, attackspeed_yan_v 239 → `[0x1a38]`; fastwalkrun_p 111 → `[0x1a2c]` vs nhánh yan `[0x1a30]` | mọi nơi dùng lấy **MAX(hai nhánh)** (`cmovge`, 13 chỗ sinh lực, 0x080787A9 tốc độ đánh, handler 0x08098A50) | JX1 một biến cộng dồn (`LifeMaxP`/`ManaMaxP`/`AttackSpeedV` đăng ký cho cả hai tên) | **lệch** (mục 14.4-1) |
| sorbdamage_p 218 `[0x1a24]` / sorbdamage_yan_p 237 `[0x1a28]` kẹp 500 | max hai nhánh | JX1 `max(m_CurrentSorbDamageYanP, nSorbM)` (PF 31/08k) | trùng |
| coldenhance_p 161 | `[0x1430] += v0`; 0x0807C993 cộng vào THỜI GIAN đông (`max(v1, [0x1430]+[0x11e0])`) | `pDes->nValue[1] = v1 + m_CurrentColdEnhance` | trùng |
| damage2addmana_p 134 | `[0x13cc]`; BeHurt 0x0808A9A7 `mana(NẠN NHÂN) += dmg·p/100` | KNpc.cpp:4146 cộng cho `this` (nạn nhân) | trùng |
| addphysicsdamage_p 126 | 0x0809A7F0: |v2| 0..5 → ô vũ khí cận chiến `[0x1440 + 4·map(v2)]`, 6 = ALL, 7 = tay không, 8 = MELEE_ALL, 9 = tầm xa | `AddPhysicsDamageP` cùng bố cục; v2 = 0 → ô 0 = kiếm (EqtLimit 151 Võ Đang Kiếm Pháp = 0, Hoa Sơn = 0) | trùng |
| allres_p 114 | += v0 vào 5 kháng | `AllresP` | trùng |
| anti_do_hurt_p 260, skill_enhance 243, walkrunshadow 293, frozen_action 251, manareplenish_p 248 (`[0x119c] += v0`), lifemax_yan_p (`[0x1a18] += LifeMax·v0/100`) | += v0 / cờ | tương đương | trùng (chi tiết mục 12.3) |
| Cổng "không bắn lại" 0x8fc62a0/0x8fc4360 (trước Fire) | điền từ vector 0x8fbfe04/0x8fbfe10 — KHÔNG có nơi ghi trong toàn ELF (xref chỉ có đoạn init) → luôn 0 | không cần port | trùng |

Dữ liệu: so 38 dòng Hoa Sơn + 45 dòng con/sự kiện (418…430, 1410/1411/1420/1421, 203/64/69/92/208/275, 412–417…) Linux ↔ JX1: **0 khác biệt cột chức năng** (chỉ khác dấu cách cuối tên, dấu ngoặc kép mô tả, 1411 tên GBK→Việt). skills.txt client ≠ server chỉ ở 2 dòng 1561/1562 (Túy Tiên Tá Cốt/VIP 90 — không thuộc đợt này). `huashan.lua` server = client = Linux (cmp).

### 14.4 Lệch còn lại — CHỜ CHỦ QUYẾT (đổi cơ chế chung, ảnh hưởng 9 phái cũ nên không tự sửa)
1. **Nhánh "(Dương)" lấy MAX**: Linux giữ hai nhánh (thường / yan) và dùng `max`; JX1 cộng dồn. Với Hoa Sơn: 1349 có CẢ `attackspeed_v` và `attackspeed_yan_v` cùng 6..32 → Linux +32, JX1 +64; 1376/1381 có cả `lifemax_p` 5..30 % và `lifemax_yan_p` 5..20 % → Linux +30 %, JX1 +50 %; 1349/1358 `lifemax_yan_p`, 1374 `manamax_yan_p` 35..200 % ở JX1 cộng thêm vào % trang bị thường (Linux lấy max với nhánh trang bị). Sửa = `LifeMaxP/ManaMaxP/AttackSpeedV/FastWalkRunP` tách hai bộ đếm + `max` khi tính lại (kể cả Phi Phong "(Dương)" đang cộng dồn).
2. **addcoldmagic_v không sinh thời gian đóng băng** ở Linux (1358 Huyễn Nhãn Vân Yên +20..315): JX1 bảng 16 bậc (≤ 64 khung) + `max` vào thời gian đông của chiêu nội công → Khí Tông đóng băng lâu hơn Linux; đổi ảnh hưởng Thúy Yên và mọi phái dùng thuộc tính này.
3. **Kinh nghiệm kỹ năng**: Linux cộng cho ĐÚNG kỹ năng gây sát thương (0x080E5D90 tại 0x0808AA97, kể cả 1363 tự phóng); JX1 `AddSkillExp90` chỉ cộng cho kỹ năng đang chọn, `AddSkillExp120` cộng mọi buff exp khi gây sát thương.
4. Auto-skill: Linux giữ nút tỷ lệ âm khi gỡ quá tay; JX1 xoá khi ≤ 0 (giữ 3 ô trống). Linux có cờ `v1 == 1` (tôn trọng CD kỹ năng thật) — Hoa Sơn không dùng.

### 14.5 CHECKLIST SWAP đợt f (chủ chạy `ChoiGame.bat`/`ChayGameServer.bat`; KHÔNG tự restart)
1. `bin\server\CoreServer.dll.moi` — 18.246.144 byte, md5 `7dde5ff4` (Server Release x64, 02/09 01:29, build từ HEAD `3e7a5c38` = superset bot đợt d `7f041f8d`; bản `.moi` cũ của phiên bot md5 `3bedd3ac` đã lưu thành `CoreServer.dll.moi.bot_3bedd3ac_0021`, không cần dùng nữa).
2. `bin\client\CoreClient.dll.moi` — 2.439.168 byte, md5 `21f02991` (01:30) — swap CÙNG CoreServer (giao thức không đổi so với đợt e; mô tả `#lA-` và nhịp thuốc hồi nội lực nằm ở client).
3. Game.exe / Goddess.exe / dữ liệu / script: không đổi.
4. Nghiệm thu: buff 1364 → để quái ĐỊCH đánh: kỳ vọng thấy 3 kiếm bay về quái kèm hoạt ảnh xuất chiêu (~3 % mỗi đòn trúng, hồi 15 s); mô tả kỹ năng hiện "[ Thái Nhạc Tam Thanh Phong ]"; nội lực Kiếm Tông tụt về 0 sau nửa giây khi có 1349/1364/1369; buff hết 10 phút → không còn tự phóng; 1369 (150) đánh trúng → 9 kiếm 1368 bay về nạn nhân (5 %); 1365 Tử Hà Kiếm Khí chỉ bắn 1366 khi máu VƯỢT XUỐNG dưới 25 %.

### 14.6 Phản biện (một tác tử độc lập, chỉ đọc, 26 lượt tool) — 0 lỗi cứng, 3 nghi ngờ → xử lý ở commit `3e7a5c38`
| Mục | Kết luận | Xử lý |
|---|---|---|
| `HS_AutoSkillModify`: ngữ nghĩa, memset `KMagicAutoSkill` (POD), v2 = 0, không cộng đôi qua `UpdataCurData` (ZeroMemory) → `ReCalcStateEffect` (áp lại giá trị đảo của node), `SetStateSkillEffect` lên cấp = gỡ khoá cũ + thêm khoá mới | OK | — |
| Luồng `ReceiveDamage`: 5 `CalcDamage` giữ luồng, marker `pTemp++; //stun[14]` đúng hàm (chỗ thứ 2 ở `AppendSkillEffect`), không `return` giữa, chết giữa chừng an toàn, quan hệ `& 0xC == 8` | OK | — |
| Cổng `bHSTrung` (đòn bị né hoàn toàn không kích) | NGHI NGỜ | Kiểm asm: `[ebp-0x48]` = độ dịch kháng ngũ hành (`seriesdamage_p − seriesres_p [0x13e8]`, đảo dấu để hoàn kháng), KHÔNG phải kết quả đòn → Linux không gate theo sát thương → **bỏ `bHSTrung`**, trả 5 lời gọi `CalcDamage` về nguyên bản |
| Khe cứu nguy 25 % chạy cả `bReturn = TRUE` (nhịp độc `m_nLastPoisonDamageIdx` có thể 0, nhịp cháy attacker = mình, phản đòn) | NGHI NGỜ | Kiểm asm: nhịp độc/cháy Linux (0x0808BDF9) và phản đòn đều gọi chung BeHurt 0x08089C90 → khe 25 % trong BeHurt áp cho cả → **giữ**; attacker 0/mình → rơi về tự cast lên mình, an toàn |
| `CastAutoSkillAt` không SetNextCastTime server trong khi client `s2cDirectlyCastSkill` luôn khoá | NGHI NGỜ nhẹ | **Thêm** SetNextCastTime như `KNpc::Cast(int,int)` |
| Gói `NPC_SKILL_SYNC` pack(1) offset 1/5/9/13/17 khớp client; `CURREGION` đã kiểm `m_RegionIndex`; `KSkill::Cast(-1, target)` đường có sẵn | OK | — |
| Nhịp hồi (R > 0 nhân %, R ≤ 0 cộng thẳng; nội lực không %; thuốc × (100+p), p = −200 → âm rồi kẹp 0) | OK | — |
| Chữ ký/`#ifdef _SERVER`/link client; encoding ASCII (0 byte ≥ 0x80 trong dòng +) | OK | — |
| Ghi nhận ngoài phạm vi (không sửa): `SetStateSkillEffect` nhánh `nTime == 0` memset node mà không `ModifyAttrib` gỡ → mọi thuộc tính của trạng thái đó kẹt (pre-existing); `DeathSkill` còn gate cấp 120 (Linux DoDeath không gate); `dwNextCastTime >= now` lệch 1 khung so Linux `jb` → đã đổi thành `>` | — | ghi nhận |


---

## 15. ĐỢT g (02/09 ~04:00) — chủ quyết 4 lệch mục 14.4: "bỏ (Dương) làm 1 · làm 2 · làm 3 · làm 4 · làm xong tôi test luôn"

Commit `d715746b`. Tool `hs_data_boyan.py` (dữ liệu) + `hs_engine_patch5.py` (engine, marker `[HOASON 02/09g]`).

| Mục | Quyết | Đã làm |
|---|---|---|
| 1 "(Dương)" | **bỏ nhánh (Dương)** ở Hoa Sơn (không đưa cơ chế MAX hai nhánh vào engine) | Dữ liệu server + client (`skills.txt` cột LvlSetting theo tên, `huashan.lua` khoá bảng; bản lưu `.truoc_hoason_0209`): 1349 bỏ `attackspeed_yan_v` (trùng giá trị `attackspeed_v` → hiệu dụng = MAX Linux), `lifemax_yan_p`→`lifemax_p`; 1358 `lifemax_yan_p`→`lifemax_p`; 1374 `manamax_yan_p`→`manamax_p`; 1376/1381 bỏ `lifemax_yan_p` (`lifemax_p` ≥ yan mọi cấp → = MAX Linux). **Giữ `sorbdamage_yan_p`** (1376/1381): JX1 đã `max()` + đơn vị phần nghìn đúng Linux, còn `sorbdamage_p` của JX1 là phần trăm (×10) → đổi tên sẽ mạnh gấp 10. |
| 2 addcoldmagic_v | làm chuẩn Linux | `AddColdMagicV` chỉ cộng min/max (Linux 0x08097AD0), bỏ bảng 16 bậc; `AppendSkillEffect` nhánh nội công không `max()` thời gian đông từ `m_CurrentColdMagic` (Linux 0x0807C9C0). Ảnh hưởng mọi phái dùng thuộc tính này (chủ duyệt). |
| 3 exp kỹ năng | làm | **KHÔNG đổi — đính chính:** Linux cộng exp qua mảng 9 thuộc tính `addskillexpN` của chiêu (vòng 0x0808AA38: 60 % mỗi đòn, nạn nhân kind 1/2, v0 = id chiêu nhận, v1 = exp, `KSkillList::AddExp 0x080E5D90` có trần ngày `MAXEXP_PERDAY` 33.000.000); Hoa Sơn chỉ 1382 mang `addskillexp1` với v0 = 0 → chết ở Linux (đúng như đợt b ghi). Cơ chế "đánh quái lên exp 90/120" (`AddSkillExp90/120`, `g_Skill90ExpRate`) là của JX1 và đã áp cho 6 chiêu exp Hoa Sơn — làm "chuẩn Linux" nghĩa là Hoa Sơn **mất** exp khi đánh → giữ nguyên. |
| 4 auto-skill âm | làm | `HS_AutoSkillModify`: xoá ô CHỈ khi tổng về đúng 0 (Linux 0x0818908F), ô âm giữ lại (vô tác dụng); gỡ mà chưa có ô → tạo ô âm (Linux 0x08189169); hết 3 ô → thay ô trơ (tỷ lệ ≤ 0) vì Linux dùng map không giới hạn. |

Kiểm: `huashan.lua` server = client (cmp); `skills.txt` server ≠ client chỉ 2 dòng 1561/1562 cũ; 0 byte ≥ 0x80 trong dòng thêm của engine; build Server Release x64 + Client Release Win32 sạch.

### 15.1 CHECKLIST SWAP đợt g (chủ chạy `ChayGameServer.bat`/`ChoiGame.bat`)
1. `bin\server\CoreServer.dll.moi` — 18.246.144 byte, md5 `71664443` (03:58, HEAD `d715746b`, superset đợt f + bot đợt d).
2. `bin\client\CoreClient.dll.moi` — 2.438.656 byte, md5 `c083d5a7` — swap **cùng** CoreServer.
3. Dữ liệu đã ghi thẳng (server + client): `settings\skills.txt` (5 dòng 1349/1358/1374/1376/1381), `script\skill\huashan.lua` (6 chỗ) — server đọc lại khi khởi động, client khi mở lại. Game.exe/Goddess không đổi.
4. Nghiệm thu thêm so với 14.5: mô tả 1349/1358/1374/1376/1381 không còn chữ "(Dương)", tốc độ đánh 1349 chỉ +1 lần; Huyễn Nhãn Vân Yên không còn làm chiêu nội công đóng băng lâu hơn.
