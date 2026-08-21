# BAN GIAO — THI CONG PORT 100% LIEN DAU leaguematch/WLLS

Phien 20/08/2026 (tiep BANGIAO_LIENDAU.md cua phien phan tich).
**Trang thai: DA THI CONG XONG toan bo engine + script + settings, build sach,
DA CAI vao cay chay that. CHUA restart GameServer** (classifier chan thao tac
kill process — nguoi van hanh bam 3 lenh o muc 1 la xong).

Yeu cau goc cua nguoi dung: dem 100% tinh nang lien dau ban Linux sang, giong
100%, BO he lien_dau tu viet, moi fix phai chay phan bien.

---

## 1. VIEC DUY NHAT CON LAI — RESTART (nguoi van hanh lam)

Script + settings nap luc boot; DLL moi da dat san canh DLL cu. Chi can:

```bat
taskkill /IM GameServer.exe /F
cd /d E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server
move /y CoreServer.dll CoreServer.dll.truoc_wlls
move /y CoreServer.dll.moi_wlls CoreServer.dll
start "GameServer" GameServer.exe
```

(mysqld / Sword3PaySys / Goddess / Bishop / S3Relay GIU NGUYEN — chi GameServer.)

> **DA RESTART 23:12 dem 20/08 (nguoi van hanh tat, Claude swap DLL + bat lai):**
> boot "Khoi dong hoan tat" sach — 0 "Include HONG", ScriptError.log KHONG them
> dong nao (giu nguyen 3787B noise tasklink cu), nguoi choi vao game binh thuong.
> Phat hien them: thu muc `logs\` CHUA TUNG ton tai ==> sJX2_ScriptLog (script_jx2.log)
> xua nay fopen fail IM LANG (benh giong WriteStringToFile da va o fc957e15,
> nhung o ham log C++ khac). Da TAO SAN thu muc 23:16 — khong can restart vi moi
> lan ghi la mot fopen moi. No engine nho: sJX2_ScriptLog chua tu tao thu muc
> (chi anh huong neu ai xoa `logs\`; muon triet de thi them g_CreatePath nhu
> WriteStringToFile o lan build sau).

**Kiem sau boot:**
- `GameServer.log`: khong co dong "[script] Include HONG" tro vao leaguematch.
- `logs\script_jx2.log`: thay "============ League Match Start: Phase=..." va sau
  do moi 15' mot dong "League Match: Phase=... MatchID=... SeasonID=131".
  Hom nay 20/08 la GIUA mua 131 (type 1 - Song dau, 18h00-19h00 hang ngay,
  them 20h00-21h00 Thu 6/7/CN).
- Vao game: 7 thanh (37/176/78 + 80/162/1/11) co "Su gia kiet xuat"/"Su gia
  lien dau" (NPC 308) + "Su gia lien dau" (87, bang tim dong doi).

**Quay lui toan phan:**
`move /y CoreServer.dll.truoc_wlls CoreServer.dll` + chep de nguyen trang tu
`_backup_wlls_20260820\` (7 file du an + missions/TimerTask/magicscript) +
move `_gobo_lien_dau_20260820` ve `script\tinhnang\lien_dau` + xoa
`script\missions\leaguematch`, `script\leaguematch`, `script\misc\eventsys`,
`script\gmscript.lua`, `script\maps\newworldscript_default.lua`,
`script\gb_taskfuncs.lua`, `script\lib\pay.lua`.

---

## 2. KIEN TRUC DA CHON (khac gi so voi ke hoach phan tich)

1. **Kham pha quyet dinh: engine ta = MOI FILE .lua MOT lua_State rieng**
   (KSortScript.cpp g_ScriptSet[5000], LoadAllScript nap TOAN BO \script luc
   boot, moi file tu chay top-level). Engine Linux = MOT state + pack
   (curpack/usepack). He qua:
   - Bay #5 cua BANGIAO (va cham FALSE/_M giua 2 head.lua) TU TAN BIEN.
   - Nguoc lai moi thu "dang ky toan cuc luc boot" cua Linux phai KEO VE TUNG
     STATE: head.lua mo 7 Include macthtype (dong comment san cua tac gia goc,
     kem guard WLLS_HEAD cung cua tac gia goc — vi Include ta la lua_dofile
     KHONG include-once, thieu guard la de quy vo han); head.lua cuoi file
     Include award.lua (handler EventSys "MatchResult" phai nam cung state voi
     noi phat su kien head.lua:763).
2. **Relay gop ve GS**: 7 file relay dat nguyen xi o \script\leaguematch\
   (path goc → 10 call site LG_ApplyDoScript khong phai sua). "Task centre"
   cua relay thay bang **\script\leaguematch\gsdriver.lua** (file MOI duy nhat):
   Include task.lua; WLLS_DriverInit() = TaskShedule() + GameSvrReady(0);
   OnTimer moi 30s bam quy 15' theo dong ho tuong (dung nhu TaskTime+
   TaskInterval(15) goc) goi TaskContent(). TaskName/TaskTime/TaskInterval/
   TaskCountLimit la stub C tra 1 → task.lua giu nguyen byte.
3. **"dw"/"dwf" + do tre mang**: GlobalExecute (KTongJX2.cpp) boc tien to,
   xep hang **KJx2DeferredExec** chay o tick sau (CoreServerShell.cpp:1173),
   trong state cua **\script\gmscript.lua** (file moi, 1 dong Include
   wlls_gmscript.lua — dung diem ha canh goc Linux gmscript.lua:7). Do tre 1
   tick = mo phong do tre mang relay→GS, hoa giai de quy NewWorld-giua-thoai
   (BANGIAO 4.3). Chuoi khong tien to giu nguyen hanh vi cu (chay ngay).
4. **NewWorldScript hoi sinh o tang engine**: MapList.ini co san 46 muc tro
   \script\missions\leaguematch\...\newworld.lua nhung engine Windows xua nay
   KHONG doc khoa nay (khac Linux). Da them: KSubWorld doc NewWorldScript/
   NewWorldParam luc LoadMap; KNpc::ChangeWorld ban OnLeaveWorld(map cu) +
   OnNewWorld(map moi); KPlayerSet::PrepareRemove ban OnLeaveWorld luc thoat.
   Script thieu → bo qua IM LANG (495 muc khac tro newworldscript.lua von
   khong ton tai — khong duoc spam log, khong duoc doi hanh vi map thuong).
5. **Mua giai noi lai**: timetable.lua giu nguyen [40..123], noi [124..243] =
   2026-01..2035-12 (ngay 08→28, 48 tran, lich 18h + 20h cuoi tuan y het),
   the loai xoay theo mau nam 2010 goc (2,1,2,5,3,6,2,1,4,5,3,6 — phu 6/7 the
   loai nhu goc; type 7 mixeddoubles goc cung chua bao gio duoc xep lich).
   sid<=243 giu duoi 255 (GOTAWARD goi sid vao 1 BYTE). 2 sentinel getn+1 giu
   nguyen (Lua4 getn = chi so lon nhat → roi vao [244]/[245] dung thiet ke).
6. **Vat pham — bang song cua du an ≈ bang song Linux TRU 1** (kiem tra bang
   ten, magicscriptTQ 004): honour token 1260-1263→**1259-1262**; Chan Kinh
   2223-2227→**2222-2226**; Hong Anh 2228→**2227**; Can Khon Song Tuyet Boi
   2229→**2228**; Kim Don Bao Ruong 2230→**2229**; Huyet Chien Lenh Ky
   2222→**2221**; Nhat Ky Can Khon Phu 2136→**2135**; le bao thuong hang
   4417(cap 9 Linux)→**4416** (cap 9 cua ta); **4550 giu nguyen** (Doat Ky
   Lenh/Xich Lan Cam Nang trung id 2 ben). Da BIND 17 hang magicscript.txt
   (cot 10) vao script leaguematch; 1259-1262 truoc do tro ib\*.lua (nap 37
   cua he cu) — chuyen sang honour.lua.
7. **Tien vinh du: WLLS_TASKID_HONOUR 2501 → 37** (mot dong trong head.lua).
   Ly do: engine ta co san TASKVALUE_STATTASK_HONOR=37 — KBuySell tinh gia
   moneyunit_honor=37, client UI hien thi 37, shop 93 "Lien Dau" (goods
   671-695) dang ban token bang 37. Dung 37 lam MOI mach earn/spend khep kin:
   thang tran +honor(37) → mua token shop 93 → dung token +honor. officer
   "doi phan thuong danh du" goi **NewSale(1,5,1,93)** (goc Sale(146,11) —
   buysell ta chi 101 shop, row 146 tren ban Linux tham chieu da bi tai su
   dung thanh shop thiep Giang sinh; row 173 uy danh THIEU HAN tren ca ban
   Linux → giu nguyen trang "chet" nhu ban goc: Sale(173,13) van no-op).
8. **Danh hieu KHONG phai anh xa**: playertitle_jx2.txt cua du an DA co dung
   id 81-88 + 200-207 = cac danh hieu lien dau; Title_AddTitle(id, 2, MMDD0000)
   cua KJx2Title khop dinh dang officer truyen.
9. **Map**: du phan tich cua 1 agent noi thieu, DO HASH chi muc pak (thuat
   toan FileNameToId signed-char + ha thuong bytewise) xac nhan: maps.pak cua
   du an CO DU .wor + Region_S.dat (hall 123 region, arena 828 region, GIONG
   HET pak Linux); WorldSet_GameServer.ini DA nap ca 76 map lien dau. _srv.fp
   chi la cache A* bot, khong bat buoc.

## 3. DANH SACH THAY DOI

### C++ (commit 21e570bf, 15 file, build sach Server x64 + Client Win32)
Xem commit message + THICONG_LIENDAU_PORT.md. Diem dang nho:
- LuaRunMission het sap (tra missions.txt + NULL-check) — day la loi "sap GS
  ngay tran dau" cua BANGIAO 4.1.
- OpenMission tu do ham: co InitMission (script Linux) thi goi, khong thi
  BeginMission (mission cu cua du an) — khong dung cham mission nao khac.
- KMission::RemovePlayer: voi mission 24/25/26 lat MISSION_PARAM_AVAILABLE
  cua nguoi roi TRUOC khi goi OnLeave — khong lat thi nhanh "doi trong → doi
  dich thang ngay" (combat\mission.lua:114) khong bao gio chay (nguoi roi van
  bi dem). Mission khac giu nguyen thu tu cu cua Fong Kieu.
- ST_*DamageCounter: dem sat thuong HUNG CHIU, hook 1 diem duy nhat truoc
  khoi chuyen-noi-luc trong ham tru mau cua KNpc (moi duong mat mau — danh
  thuong/phep/doc/phan don — deu qua day; m_CurrentLife -= chi co 1 cho).
- MAX_TASK 3000→4200 (task 4125; dinh dang luu (id,value) thua nen save cu
  tuong thich; con go ke task 3179 citywar).
- GetStringTask/SetStringTask = o chuoi PHIEN trong KPlayer (helper.lua chi
  dung lam con tro trang duyet danh sach) — KHONG dung den dinh dang roledb
  (ne xung dot voi dot MySQL).

### Script/settings da cai vao SRV
- +61 file lua moi (38 GS + 7 relay + 16 eventsys) + gsdriver + gmscript +
  newworldscript_default (ban compat, xem muc 4) + gb_taskfuncs (goc \script)
  + lib\pay.lua.
- Sua: head.lua (4 diem muc 2/5/7), timetable, honour/zhenjing/jindan (id),
  6 macthtype (4417→4416), officer (NewSale 93), eventsys (+OnPlayerEvent).
- Du an: startgame.lua (Include + wlls_autoexe() + DynamicExecute gsdriver
  thay addnpcliendau), playerlogin.lua (goi wlls_login TRONG main()),
  timerserver.lua (bo Include lien_dau), lib\awardtemplet.lua (+Include
  awardtype\item_jx1 — thieu la Give() im lang khong trao do),
  startgame\thon\balanghuyen.lua (comment 2 NPC cu map 53),
  settings\task\missions.txt (26 dong: 11,13-23 placeholder "trong";
  24/25/26 leaguematch — bang tra theo SO DONG nen khong duoc xoa dong),
  settings\TimerTask.txt (bo 11; +50/51/52 — bang nay tra theo KHOA nen thua
  thieu dong OK), settings\item\magicscript.txt (17 bind),
  settings\maps\championship\champion_gmpos.txt (chep, 1127B).
- Go he cu: script\tinhnang\lien_dau → _gobo_lien_dau_20260820 (nguyen ven).
- Backup truoc sua: _backup_wlls_20260820 (41 file, gom ca lien_dau nguyen ban
  va playerlogin.lua).

## 4. LECH CO CHU DICH so voi ban Linux (da can nhac — doc ky truoc khi "sua")

| # | Lech | Vi sao |
|---|---|---|
| 1 | newworldscript_default.lua la ban COMPAT tu viet (CreateTeam_OFF that; PARTNER_OFF/TISHENZHIREN no-op; giu EventSys EnterMap/LeaveMap) | ban goc keo newworldscript_h + tianziyuxi (chuoi nang, dinh 495 map khac); du an khong co he dong hanh/the than |
| 2 | HONOUR = task 37 (goc 2501) | muc 2.7 — mach tien te khep kin voi engine/shop/UI co san |
| 3 | Item id anh xa theo bang song (muc 2.6) + honour.lua them 3 muc 1299/1339/1340 | 3 lenh bai nay magicscript GOC van tro ve honour.lua nhung bang trong script goc bo sot → nguoi choi MAT DO (BANGIAO dot B da canh bao) |
| 4 | officer doi thuong vinh du mo shop 93 (NewSale) thay Sale(146,11); Sale(173,13) uy danh giu nguyen trang CHET nhu ban tham chieu | row 146 ban tham chieu da bi tai dung lam shop Giang sinh; row 173 khong ton tai o ca 2 ben |
| 5 | huoyuedu KHONG port | tbActivity2ID goc khong co khoa "wulinliansai" → lenh cong do hoat bat von la NO-OP tren ban goc (VNG tu ghi "da xoa bo do nang dong"); DynamicExecuteByPlayer cua ta bo qua em khi thieu file |
| 6 | wlls_leveupcheck van TAT (nhu ban goc: server_playerlevelup.lua:217 comment) | giong 100% hien trang ban tham chieu |
| 7 | ST_CheckTextFilter tra 1 (khong loc tu bay) | Core chua noi bo loc FilterText; giu nguyen chinh sach ten hien tai cua du an |
| 8 | GetStringTask con tro trang KHONG luu qua relog | chi la con tro phan trang; ne dung dinh dang luu nhan vat |
| 9 | Hong Anh Bao Ruong (2227) trao 4 vu khi thoi trang {0,536..539} qua item_jx1 → KHONG han dung 24h/khong khoa nhu goc (nExpiredTime/nBindState bi item_jx1 bo qua) | item_jx1 hien chi doc tbProp/nCount; muon dung han thi mo rong item_jx1 sang AddTimeItem (viec treo) |
| 10 | map_type.txt khong sua (dong LEAGUEMATCH thieu 2 co PKEX/LEAGUE so voi goc) | engine Windows KHONG doc file nay (0 tham chieu trong Sources) — cam item da co wlls_en_check + RemoveSkillState phia script |
| 11 | switch.lua giu nguyen goc: KIET XUAT (80-119) **DONG**, VO LAM (>=120) MO | dung hien trang ban tham chieu; mo hang duoi = sua WLLS_SWITCH_JUNIOR=1 trong \script\leaguematch\switch.lua |
| 12 | pgaming overlay (shopliendau 2501, hieuthuoc lien dau, NPC trang thai) KHONG port | do la lop private-server ngoai leaguematch; shopliendau con loi am diem (AskClientForNumber khong kiem lai) |
| 13 | Ladder van TOP 10 (JX2LADDER_TOP), goc xep toi 512 | chi anh huong hien thi top qua npc\head (10 hang dau van dung); nang len 512 = viec treo |
| 14 | GS khong co achievementsys type wlls (registrant thu 2 cua MatchResult tren Linux) | du an chua co achievementsys |
| 15 | Quet do cam mo rong 9 o dung-ngay (goc 3) — head.lua vong i/j 1..9 + engine ITEM_GetImmediaItemIndex nhan 1..9 | IMMEDIACY_ROOM_WIDTH du an = 9; giu 3 la lot do cam o o 4-9 |
| 16 | ST_DamageCounter dem MAU MAT THAT (sau chuyen-noi-luc + khien tinh + clamp overkill), khong phai damage truoc khien | tie-break cong bang; khong co nguon C Linux de doi chieu, chon so do sat nghia "damage hung chiu" nhat |
| 17 | CalcEquiproomItemCount = hanh trang + TUI MO RONG; CalcItemCount(nPos,...) truyen thang ITEM_POSITION (1=tren tay, 3=hanh trang); CountFreeRoomByWH tra SO CHO that (khong phai 1/0) | tui ex la tinh nang du an (Linux 2010 chua co) — khong cong la lot do cam; bang chung pos: songjin_shophead.lua:132/139 cap CalcItemCount(3)+ConsumeItem; composeex.lua:333 can so that |
| 18 | wlls_reload (GM) chi nap lai duoc cac script co that — 2 dong LoadScript global\server_playerlevelup.lua + global\login.lua tro file khong ton tai (giu nguyen van ban goc) | chi anh huong lenh GM reload; LoadScript thieu file chi log |
| 19 | Cac ham moi dang ky ten toan cuc (CalcItemCount/CalcEquiproomItemCount/CountFreeRoomByWH/AskClientForString/DynamicExecute) lam "song day" mot so call site cu von loi-nil (songjin_shophead tetan2mibao, dailogsay, composeex, itemblue, map_management...) | da ra soat: khong co duong exploit (songjin_shophead khong duoc NPC nao include — xac chet; cac cho con lai ngu dong hoac dem dung hon truoc); ConsumeItem trong tetan2mibao lech chu ky tu truoc — KHONG thuoc pham vi port nay |

## 5. PHAN BIEN

- Vong 1 (trong phien): 2 tac tu doc lap (kiem ke du an + doc sau 45 file Lua)
  → phat hien va SUA truoc khi cai: macthtype khong duoc Include (nap kieu
  1-state), OnPlayerEvent do engine bom, NewWorldScript chet tren Windows,
  missions.txt tra theo so dong, MAX_TASK 3000, item id bang song khac id
  trong script goc, Sale 146/173 mo/chet...
- Vong 2 (DA XONG 21/08, moi phat hien deu duoc tu xac minh lai tren ma nguon
  truoc khi sua):
  * Phan bien SCRIPT: 38/46 file giong het tung byte, 7 file lech dung danh
    sach duyet, 0 byte thua; cu phap Lua 4 PASS 46/46; 13 muc lien ket runtime
    PASS 12 — 5 loi that DA SUA: (1) hook wlls_login bi long nham TRONG khoi
    vong sang bang hoi playerlogin.lua (nang nhat — da doi ra ngoai, moi nguoi
    choi deu duoc tra thuong offline); (2) missions.txt 12 dong tro file ma →
    gom ve mission_trong.lua co that; (3) compat CreateTeam_OFF thieu
    LeaveTeam() nhu goc → da them; (4) WriteStringToFile fopen tran →
    \relay_log\<ngay>\ mat log im lang tren Windows → da va g_GetFullPath +
    g_CreatePath (KTongJX2.cpp); (5) backup thieu eventsys.lua goc → da chep.
  * Phan bien C++ (15 muc): 11 PASS; 3 loi NANG + 4 loi VUA DA SUA het:
    A1 ban va "flip AVAILABLE truoc OnLeave" ghi trong commit message 21e570bf
    nhung CHUA HE AP vao KMission.cpp (da ap that, mission 24-26);
    A2 DelMSPlayer(id, 0) la no-op cam (arg2=0 chet o guard) → arg2==0 nay
    hieu la CHINH NGUOI dang chay script nhu Linux — mach "chet/roi san bi go
    khoi mission → doi bi quet sach xu thang ngay" song lai;
    A3 SyncTaskValueMore(2500, 37) range nguoc sau remap HONOUR → tach 2 lenh
    SyncTaskValue tai head.lua:691 (task 2500 truoc do KHONG BAO GIO xuong client);
    B1 quet do cam 3/9 o dung-ngay → 9 (lech #15); B2 3 ham item sua ngu nghia
    (lech #17); B3 doi cho bo dem ST xuong ngay truoc dong tru mau (lech #16);
    B4 ForbitStamina gate them duong Cost the-luc cua chieu thuc.
  * Ghi nhan KHONG sua (co ly do): chuoi nhap rong → wlls_createleague tu lay
    ten nhan vat (guard "if not str_lgname" co san); prompt hop nhap cut 31
    byte (S2C_INPUT_BOX.Value[32] — cam doi wire); DynamicExecute arg chua
    dau nhay kep se pha cau lenh (call site hien tai toan hang so); wlls_gmscript
    :161 return som khong khoi phuc PlayerIndex (bug goc Linux, chep nguyen).
- Cong cu gap: ReverseTools\re_lua_api_gap.py (patch tro cay da cai) → tu 37
  ham thieu xuong 0 (10 canh bao con lai deu la false-positive parser hoac
  guard co y: GetGlbMSRestTime/login_add da co "if ... then").

## 6. NGHIEM THU DE XUAT (sau restart)

1. Boot: log khong Include HONG; script_jx2.log co "League Match Start".
2. 18h00 (hoac doi sang phase 4 gan nhat): NPC "Quan viên hội trường" xuat
   hien o hall (396+/540+); bao danh → duoc keo vao san chuan bi (560+/570+);
   4 phut sau ghep cap vao arena (397+/541+); danh/tha → cong diem; xem
   "Xem chiến tích của đội" o su gia.
3. Cuoi mua (28/08 qua 29/08): pha 1 → nhan thuong xep hang + danh hieu top4.
4. GM nhanh: GetGlbValue(820..826) qua console/script de soi pha.

## 7. BO SUNG 21/08 RANG SANG — chay thu that + lenh bai admin + file cau hinh

Sau khi nguoi van hanh restart va bam NPC, he van hanh dung thiet ke nhung lo 3 viec:

1. **"Su gia kiet xuat khong bam duoc"** = HE DANG DUNG: hang Kiet xuat DONG
   theo switch.lua goc (JUNIOR=0) -> NPC vao nhanh xin loi -> nhung
   `GetNpcName(GetLastDiagNpc())` tra nil -> concat gay (ScriptError 23:33 bat
   nguyen van nho _ALERT moi). Fix goc 3 tang:
   - `_ALERT` dang ky vao moi state: loi runtime Lua tu nay ghi NGUYEN VAN
     (thong diep + traceback) vao ScriptError.log — truoc chi co ma so.
   - `GetLastDiagNpc`: fallback `m_nWllsLastDiagNpc` (ghi tai KPlayer::DialogNpc)
     vi engine khong bom global NpcIndex nhu Linux.
   - `GetNpcName`: idx sai tra "" thay vi nil.
2. **"Chu bi xac nhau + thieu duong dan spr"**: client Text.cpp KHONG parse
   the `<link=image:...spr>` (hien nguyen van) va TEncodeText nuot dau `<`
   khi ngay truoc la chuoi LE byte cao (luat GBK). Fix: bo the link o
   npc\head.lua + helper.lua; them `wlls_lamsach()` tu chen khoang trang
   truoc `<`; WLLS_LEVEL_DESC dat lai "Kiet xuat "/"Vo lam "; viet lai nhan
   menu officer/helper co dau, chu dau hoa.
3. **Thu muc `logs\` chua tung ton tai** -> sJX2_ScriptLog fail im lang. Da tao;
   driver lien dau xac nhan SONG: `[WLLS] Driver lien dau da khoi dong` 23:31.

**LENH BAI ADMIN — bo test lien dau** (`script\item\liendau_admin.lua`, duoc
`lenhbaiadmin.lua` Include; lenh bai dofile lai moi lan mo nen sua file la an
ngay khong can restart): xem trang thai (pha/mua/matchid/dong-mo 2 hang/diem
ca nhan), **Mo tran NGAY (ep pha 4)** de test bat ky luc nao, ve pha that,
khoi tao lai driver, cong danh du (37)/uy danh (39)/tich luy (2500), dat-xoa
chien tich, xem doi, top 10 hai ladder, di chuyen nhanh hall/khu chuan bi.
Duong di lenh ep pha: DynamicExecute -> gsdriver (WLLS_Adm_*) ->
wlls_set_phase -> "dw" (tre ~1 giay, dung mach relay that).

**FILE CAU HINH TAP TRUNG**: `script\leaguematch\wlls_config.lua` — bat/tat
2 hang (Kiet xuat mac dinh MO — lech chu dich so voi goc Linux dong), khung
gio mo tran (GIO_MO), so phut/luot, he so diem thang/hoa, he so nhan
diem/item thuong xep hang. Duoc keo vao moi state qua switch.lua (tang lich)
va missions\leaguematch\head.lua (tang tran + NPC). CHINH XONG PHAI RESTART.

## 8. FILE THAM KHAO

- D:\GAMEDEVNEW\THICONG_LIENDAU_PORT.md — nhat ky quyet dinh chi tiet.
- Commit engine: 21e570bf + commit "Fix 12 loi tu 2 vong phan bien" 21/08
  (nhanh main, D:\GAMEDEVNEW).
- Log cai dat: scratchpad phien (install_log2.txt) — ban chat da tom o muc 3.
