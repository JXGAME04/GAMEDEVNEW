# PHU LUC: DANH MUC DICH DANH HAM ENGINE CHO CONG THANH (dot E)

> Lap 14/08/2026 (E4). Nguon: quet TOAN BO call-site cua bo script se deploy o E5
> (citywar_city + citywar_global + citywar_arena + league_cityinfo + task_string +
> objbuffer_head + eventhead + ladder relay-variant + 4 item xe) bang tool
> `scan_funcs.py`, doi chieu tung ten voi binary goc `D:\ServerLinux\server1\jx_linux_y`
> (chuoi dang ky NUL-bounded) va bang GameScriptFuns cua cay ta.
> Phan bien cac giai doan sau TICK THEO BANG NAY.

## 1. DA CO SAN trong cay ta (115 ten - khong lam gi)

AddEventItem AddGlobalNews AddItem AddMSPlayer AddNpc AddOwnExp AddSkillState
AppointChallenger AppointViceroy AskClientForNumber(stub -1) CalcFreeItemCellCount
CloseMission DelMSPlayer DelNpc Describe Earn GetAccount GetByte GetCamp GetCityArea
GetCityAreaName GetCityOwner GetCitySummary GetCityWarBothSides GetCurCamp GetGlbValue
GetLevel GetLocalDate GetMSPlayerCount GetMSRestTime GetMissionS GetMissionV GetName
GetNextPlayer GetNpcName GetSex GetTask GetTaskTemp GetTong GetTongFigure GetTongName
GetWorldPos HaveBeginWar Include IncludeLib IsSigningUp LG_*/LGM_* (28) Ladder_* (3)
LeaveTeam Msg2MSAll Msg2Player NewWorld NotifyWarResult NpcIdx2PIdx OB_* (14)
OpenCityManageUI OpenMission OutputMsg Pay Say SetByte SetCreateTeam SetCurCamp
SetDeathScript SetFightState SetGlbMissionV SetGlbValue SetLogoutRV SetNpcCurCamp
SetNpcScript SetPos SetPunish SetRevPos SetTask SetTaskTemp SetTempRevPos
StartGlbMSTimer StartMissionTimer StopGlbMSTimer StopMissionTimer SubWorldID2Idx
SubWorldIdx2ID TONG_GetExpLevel Talk WriteLog

Ghi chu kiem them o E5/E6 (ten trung nhung chu ky phai khop):
- `GetNextPlayer(nMissionId, idx, nCamp)` phai tra **2 gia tri (idx, pidx)** nhu goc
  (camper.lua:25) - kiem ban JX1.
- `DelNpc(<TEN chuoi>)` - mission.lua:121-122 goi bang TEN; kiem ban JX1 nhan chuoi.
- `AddNpc` 8 doi (camper/mission) - kiem thu tu doi ban JX1.

## 2. VIET MOI o E4 (co trong binary goc, ta chua co)

### 2a. Nhom BT_ (battle framework, 17 ham) - file moi KJx2Battle.cpp
Arity chot tu call site (playerdeath.lua / camper.lua / mission.lua / head.lua):
| Ham | Doi | Tra | Ghi chu |
|---|---|---|---|
| BT_SetType2Task(nType, nTaskId) | 2 | 0 | map type -> PLAYER TASK id (bt_setnormaltask2type: 1->751, 2..50->700+i) |
| BT_GetData(nType) | 1 | 1 so | = GetTask(map[nType]) cua PlayerIndex hien tai |
| BT_SetData(nType, nVal) | 2 | 0 | = SetTask(map[nType], nVal) |
| BT_SetTypeBonus(nType, nCamp, nBonus) | 3 | 0 | bang thuong theo (type, camp 1/2) |
| BT_GetTypeBonus(nType, nCamp) | 2 | 1 so | |
| BT_SetView(nType) | 1 | 0 | cot hien thi UI JX2 - ta LUU nhung khong phat (client JX1 khong co UI nay) |
| BT_SetMissionName(sz) | 1 | 0 | |
| BT_SetGameData(nKey, nVal) | 2 | 0 | GAME_* keys (head.lua:240-253) |
| BT_SetRestTime(n) | 1 | 0 | |
| BT_SortLadder() | 0 | 0 | sap bang diem PL_TOTALPOINT cua nguoi trong tran |
| BT_GetTopTenInfo(nRank 1..10, nType) | 2 | **2**: szName, nValue | camper.lua:51 nhan 2; ngoai dai -> ("",0) |
| BT_UpdateMemberCount() | 0 | 0 | dem lai so nguoi (ta: no-op nhe / dem tu ladder) |
| BT_ClearBattle() | 0 | 0 | xoa toan bo state tran |
| BT_BroadView / BT_BroadGameData / BT_BroadAllLadder / BT_BroadSelf | 0 | 0 | goi tin UI battle JX2 -> client ta KHONG hieu -> **no-op co chu dich** (cung ly do 0xAC) |

Thiet ke store: ladder = danh sach (szName, nPlayerIdx) cua nguoi da vao tran
(AddMSPlayer/JoinCamp goi BT_SetData lan dau la co mat); diem doc SONG tu task
map[PL_TOTALPOINT] khi SortLadder/GetTopTenInfo (nguoi offline van con ban ghi ten
+ diem cuoi). Reset o BT_ClearBattle.

### 2b. Nhom Arena idle (11 ham) - vao KJx2CityWar.cpp
Nhanh VN: loi dai CHET (khong co SignUpCityWarArena trong binary - da xac minh
count=0). Dang ky DU de script nap + thoai khong loi, tra gia tri "chua bat dau":
IsArenaBegin(0..7)->0 · GetArenaBothSides->("","") · GetArenaCityArea->0 ·
GetArenaLevel->**0 gia tri khi fail** (theo DIEUTRA muc 2: fail -> 0 gia tri; binh
thuong tra 0 = chua bat dau) · GetArenaTargetCity->0 · GetArenaTotalLevel /
GetArenaTotalLevelByCity->0 gia tri fail / 0 · GetArenaSchedule->"" ·
GetArenaInfoByCity->"" · NotifyArenaResult->0 gia tri (nuot) ·
GetCityWarTongCamp(szTong)->so camp: 1 neu la chu thanh dang danh, 2 neu la
khieu chien, nil khi khong thuoc tran nao (theo DIEUTRA: 1 so / nil).

### 2c. ArenaCredits (4 ham) - vao KJx2CityWar.cpp
Get/Set/Add/ReduceArenaCredits = doc/ghi task value **id 2894** cua nguoi choi
(goc id 3179 > MAX_TASK 3000 cua ta -> im lang vo hieu; remap sang 2894 - da quet
`grep -c "2894" toan bo bin\server\script + scriptjx2` = 0 hit, con trong).
0 call site Lua trong bo deploy -> chi can dung ngu nghia.

### 2d. Nhom Title_ (7 ham) - file moi KJx2Title.cpp
Theo DIEUTRA muc 4: bang `settings\playertitle_jx2.txt` (DA chep o E1);
state per-player theo TEN (map<string> C++, KHONG doi save format);
Title_AddTitle(nId, nTimeType 0/1/2, nTime) · Title_ActiveTitle(nId; 0=tat het) ·
Title_RemoveTitle(nId) · Title_GetTitleInfo(nId)->(nTimeType,nTime)/(0,0) ·
Title_GetTitleName(nId)->""/ten · Title_GetActiveTitle()->0/nId ·
Title_GetTitleTab()->BANG cac id / **nil (0 gia tri)** khi rong (Lua 4:
lua_newtable + Lua_SetTable, tien le ScriptFuns.cpp:7781).
Call site trong bo deploy: head.lua:406-407 (title 89-99 rank tran),
mission.lua:199 (Title_ActiveTitle(0) roi tran); E6 them 152+cityid Thai Thu.
KHI DANG KY: comment 2 call site songjin_shophead.lua:227 (2 ban) - nhanh chet
se song day (phan bien F9 ke hoach).

### 2e. Nhom ha tang tran (file moi KJx2WarInfra.cpp) - co trong binary goc
| Ham | Chu ky + ngu nghia | Mo neo cay ta |
|---|---|---|
| SetNpcDeathScript(nNpcIdx, szPath) | ghi `Npc[i].ActionScript[80]` (+ClearA neu rong) | KNpc.h:450; KNpc::OnDeath():1517-1519 tu chay ExecuteScript2(ActionScript,"OnDeath",m_Index,m_nLastDamageIdx). GIOI HAN: JX1 chi chay khi ke giet LA NGUOI (KNpc.cpp:1512) - xe cong thanh ket lieu tru/cong se KHONG kich script (ghi nhan, xem E6 test) |
| ClearMapNpc(nMapId) | xoa moi NPC (khong phai nguoi) tren MOI instance map | duyet Npc[] theo SubWorldIndex; chi Kind thuong (khong player/pet) |
| ClearMapObj(nMapId) | 0 call site trong bo deploy -> KHONG lam |
| AddObstacleObj(nObjId, nMapId, nX, nY) / ClearObstacleObj(nMapId, nX, nY) | vat can chan o (359) | KObjSet AddObj idiom (LuaAddObj ScriptFuns.cpp:12357) + barrier cell |
| SetPKFlag(0/1) | bat/tat co PK nguoi choi hien tai | duong PK JX1 (KPlayer m_PKSetting?) - tra idiom LuaSetPK neu co |
| ForbidChangePK(0/1) | cam nguoi choi tu doi trang thai PK | **LO GOC**: binary goc KHONG dang ky (xac minh count=0) -> goi la nil-call CAT DUOI ham o goc (OnLeave mission.lua:185). Ta DANG KY THAT de duoi ham chay du (deviation ghi ro) |
| DisabledUseTownP(0/1) | cam dung Tho Dia Phu | **LO GOC** nhu tren (binary chi co IsDisabledUseTownP). Dang ky that |
| GetLoop() | so tick game hien tai (log) | g_SubWorldSet.GetGameTime() |
| GetPlayerRev() | **tra 2 gia tri** (map, x,y? - theo SetRevPos(GetPlayerRev()) camper.lua:8; doi chieu chu ky SetRevPos JX1 khi code) | |
| SearchPlayer(szName) | -> nPlayerIdx / 0 | duyet PlayerSet theo ten |
| RestoreOwnFeature() | tra lai hinh dang goc | ForceClearStateSkillEffect idiom / no-op neu JX1 khong doi feature |
| GetItemCountEx(g,d,p) | dem item trong tui (ca trang bi?) | KItemList idiom LuaGetItemCount:12379 |
| DelItemEx(g,d,p,n?) | xoa n item | citywar_function.lua:45 + infocenter:386 - doc call site khi code |
| GetItemLife(nItemIdx) | thoi han item | Item[].GetTimeBelong? doc khi code |
| GetItemProp(nItemIdx) | -> **3 gia tri** g,d,p (sure_GiveTiaoZhanLing:227) | Item[].GetGenre/GetDetailType/GetParticular |
| RemoveItemByIndex(nItemIdx) | xoa item theo index list | mau LuaRemoveItemIdx:2749 (co the ALIAS thang neu chu ky khop) |
| GiveItemUI(szTitle, szDesc, szOkCb, szCancelCb) | **MAP NATIVE len GiveBox JX1** (phat hien khi thi cong E4): mo cua so giao vat pham co san (khuon LuaOpenGiveBox 4 doi ScriptFuns.cpp:2418-2457, s2c_openaffairbox); nguoi choi tu keo item vao o pos_affairitem; khi xac nhan, hook KProtocolProcess::UiCommandScript case 1 goi KJx2WarInfra_GiveBoxCollect gom item (stack N = N don vi, tran 512) roi callback szOkCb(nCount) - script goc chay nguyen van, item sai loai script tu tu choi nhu goc. Doi 4 (cancel cb) khong dung: hop JX1 dong phia client, item tu tra ve tui |
| GetGiveItemUnit(i) | item index thu i cua give-list (stack khai trien tung don vi de RemoveItemByIndex tru 1 don vi/luot - so luong khop chinh xac) | |
| GetJoinTongTime() | epoch vao bang cua nguoi choi | member field JX2? camper.lua:81-87 - doc call site khi code; fallback 0 |
| GetTongMaster(nTongID/szName?) | ten bang chu | ALIAS sang logic LuaTONG_GetMaster (KTongJX2.cpp:864) - doc call site de biet doi la ID hay ten |
| AddTongExp(n) / (SubTongExp?) | cong exp bang | citywar_arena head.lua:144-148 (arena idle -> uu tien thap, stub duoc) |
| SetSiegeVoitureParam(...) / SetMangonelParam(...) | tham so xe cong thanh / may bran da | doc call site gongchengche.lua:42 / toushiche.lua:45 khi code; giai doan dau: LUU vao store de danhquai/attack dung sau (muc tieu E5/E6) |
| NW_GetSealInfo() | -> chuoi (citybulletin noi chuoi) | stub "" (he quoc chien chua port) |
| PARTNER_GetCurPartner()/PARTNER_GetSettingIdx() | task_string can | stub 0 (ke hoach 1.4) |
| ReplaceString(sz, szFrom, szTo) | -> chuoi thay the | task_string.lua:72-76 |
| GetLastDiagNpc() | npc idx dang thoai | doc global Lua "NpcIndex" cua state goi (SCRIPT_NPCINDEX KPlayerDef.h:15; m_nLastNpcIndex chi co phia client) |
| GetNpcSettingIdx(nNpcIdx) | template id cua npc | Npc[].m_dwTemplateID? doc khi code |
| PushString(sz) / AppendString(sz) / PopString() | bo dung chuoi engine | **PushString/PopString KHONG co trong binary goc** (chi co AppendString) = LO GOC lam CHET duong hien thi top-10/danh sach bao danh; ta lam CA BA that (buffer per-state hoac global don luong, gioi han 8KB) |
| SetMissionV(nId, nVal) | ALIAS -> LuaSetMission (ScriptFuns.cpp:12641 "SetMission") | |
| SetMissionS(nId, sz) | VIET MOI LuaSetMissionString (KMission co GetMissionString:390 - them setter cung khuon) | |

## 3. KHONG DANG KY (giu nguyen hanh vi goc / nhanh chet)

| Ten | Ly do |
|---|---|
| SignUpCityWarArena | binary goc KHONG co (count=0) - nhanh VN loi dai chet; de nil nhu goc |
| safeshow | khong ton tai trong goc; chi camper arena goi (idle) |
| get_maincity_deadline | chi nam trong nhanh GetGlbValue==1 (khoa relay) - callback dong bo cua ta lam nhanh nay KHONG BAO GIO chay (DIEUTRA_LEAGUE muc 4) |
| add_citybonus_task | SHIM LUA o E5 (get+set qua LG_) - khong phai ham C |
| GetCityCount | TRUNG ten JX1 (tram dich ScriptFuns.cpp:8869) - 0 call site, giu JX1 |
| GetCity | JX2 von stub rong - giu ban JX1 |
| clone/split/strfill_center | ham LIB LUA (lib\common.lua:56,98,144 + string.lua) - E5 kiem cay ta co chua, thieu thi chep common.lua goc |
| BT_LeaveBattle | bi comment o playerdeath (khong goi) |
| DisabledChatCity/IsDisabledChatCity | DA lam stub o E3 (KJx2CityWar) |

## 4. LO THUNG CO SAN CUA BAN GOC (bang chung de khoi tranh cai)

Binary `jx_linux_y` (va ca 4 .so) KHONG dang ky: `ForbidChangePK`,
`DisabledUseTownP`, `PushString`, `PopString`, `SignUpCityWarArena` - trong khi
script GOI that (mission.lua:185/187, camper.lua:13/149, infocenter:133/137...).
Khong co settagmethod fallback nao (chi Tgettable/Tsettable cua class lib).
=> O ban goc VN, cac loi goi nay la "attempt to call a nil value" - CAT DUOI ham
dang chay (vd OnLeave khong bao gio chay het: Title_ActiveTitle(0)/AddSkillState
661 khong duoc don khi roi tran). Ta chon DANG KY THAT 4 ten dau (muc 2e) de
script chay tron ven - day la deviation CO CHU DICH so voi binary (nhung DUNG voi
y do script); rieng SignUpCityWarArena giu nil dung nhanh VN.
