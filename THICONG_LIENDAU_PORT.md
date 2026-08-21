# THI CONG - PORT 100% HE LIEN DAU leaguematch/WLLS (Linux) -> DU AN

Phien 20/08/2026. Trang thai cap nhat lien tuc trong phien. Chu truong nguoi dung:
- Dem 100% tinh nang leaguematch ban Linux sang, GO BO han he lien_dau tu viet.
- Giong 100% (uu tien hanh vi giong ban Linux; file Lua chep NGUYEN BYTE, chi va cho nao bat buoc).
- Moi fix phai chay phan bien.

## A. PHAT HIEN MOI (khac / bo sung so voi BANGIAO_LIENDAU.md)

1. **Moi file .lua = MOT Lua state RIENG** (KSortScript.cpp:17 g_ScriptSet[]; KLuaScript.m_LuaState).
   Server nap TOAN BO \script luc boot (g_IniScriptEngine -> LoadAllScript("\\script"), KSortScript.cpp:51-68).
   => Bay #5 cua BANGIAO (va cham FALSE/_M giua 2 head.lua) KHONG TON TAI - moi file mot state.
   => Moi file port dat dung duong dan goc se TU NAP luc boot (top-level tu chay nhu Linux).
2. **Include = lua_dofile MOI LAN GOI, KHONG guard** (ScriptFuns.cpp:1914-1971).
   => Mo 7 include macthtype trong head.lua (dong 302-316, tac gia goc da viet) BAT BUOC kem
   guard WLLS_HEAD (dong 2-6, cung tac gia goc viet san, dang comment) de chan de quy
   head->macthtype->tb_head->normal->head.
3. **Diem ha canh cua "dw ..."**: state cua \script\gmscript.lua - Linux gmscript.lua:7 Include
   wlls_gmscript.lua. Du an: them 1 dong Include vao \script\gmscript.lua cua du an.
4. **KMission::RemovePlayer (KMission.cpp:159-184) goi "OnLeave" TRUOC khi go nguoi choi**
   => GetMSPlayerCount trong OnLeave van dem ca nguoi dang roi => nhanh "doi trong -> doi dich
   thang ngay" (combat/mission.lua:114) KHONG BAO GIO chay, tran boc hoi khong ket qua.
   Fix C7: voi mission id 24/25/26, lat MISSION_PARAM_AVAILABLE cua nguoi roi TRUOC khi goi OnLeave.
5. **IL da co** (alias IncludeLib, ScriptFuns.cpp:12928). IL("ITEM") chua co trong bang module
   -> chi log "bo qua" (ScriptFuns.cpp:2397) - vo hai; se them ITEM->noop.lua cho sach log.
6. **TASKVALUE_STATTASK_RESPECT = 39** (GameDataDef.h:69) + KBuySell da ho tro shop tinh gia
   bang uy danh => GetRespect/AddRespect/SetRespect = wrapper task 39 (mau: Repute ScriptFuns.cpp:372,464).
7. **Lua 4.0 getn = chi so so hoc lon nhat** (lapi.c:458-476) => 2 sentinel roi vao [max+1],[max+2] dung thiet ke.
8. g_MissionTimerCallBackFun -> "OnMissionTimer" KHONG anh huong leaguematch (he chi dung GlbTimer
   cua KJx2League.cpp:996 - da goi "OnTimer" dung chuan Linux). Bo muc A2/OnTimer-rename khoi pham vi.
9. sJX2RemapScriptPath (ScriptFuns.cpp:1883): Include \script\lib\* / \script\tong\* thieu file se
   thu scriptjx2\lib\* / scriptjx2\tong_vn\* -> lib JX2 co the phuc vu, nhung PHAI doi chieu
   noi dung voi lib\common.lua cua Linux JX1.
10. LuaOpenMission: mission dang mo ma Open lai -> _ASSERT(0) + bo qua (ScriptFuns.cpp:10346).
    Luong wlls luon Close truoc Open (schedule.lua:220-221; wlls_setphase:100) - an toan.
11. Ban tham chieu D:\ServerLinux co lop "pgaming" (private server overlay): xem muc F.

## B. KIEN TRUC PORT (chot)

- **1 GS duy nhat, gop relay**: 7 file relay dat NGUYEN XI vao \script\leaguematch\ (path goc).
  GS 38 file dat NGUYEN XI vao \script\missions\leaguematch\. Moi file 1 state -> khong va cham.
- **Task centre relay -> GS driver**: file MOI \script\leaguematch\gsdriver.lua (state rieng):
  Include task.lua; ham WLLS_DriverInit() = TaskShedule() + push pha lan dau (GameSvrReady(0) semantics);
  OnTimer() moi 30s: neu sang quy 15' moi (theo dong ho tuong, date("%M") chia 15, khu trung theo id quy)
  -> TaskContent(). Timer dang ky bang StartGlbMSTimer voi TimerTask.txt row moi tro vao gsdriver.lua.
  Khoi dong tu startgame.lua: DynamicExecute vao state rieng cua gsdriver.
- **TaskName/TaskTime/TaskInterval/TaskCountLimit**: dang ky C no-op (return 0/1) de task.lua nguyen ban.
- **GlobalExecute** (KTongJX2.cpp:4024): giu nguyen hanh vi cu cho chuoi KHONG tien to;
  chuoi "dw <stmt>" -> HOAN 1 tick, thuc thi <stmt> trong state \script\gmscript.lua;
  chuoi "dwf <path> <stmt>" -> hoan 1 tick, thuc thi trong state <path>.
  Hang doi chay trong KJx2DeferredExec_Breathe() goi canh KJx2GlbMission_Breathe (CoreServerShell.cpp:1172).
  Do tre 1 tick = mo phong do tre mang goc -> giai quyet de quy NewWorld-giua-dialog (BANGIAO 4.3).
- **LG_ApplyDoScript** giu nguyen dong bo (10/10 call site khong dung callback; script dich se
  GlobalExecute nguoc ve = da hoan 1 tick o buoc tren).

## C. DANH SACH SUA C++ (moi muc se co phan bien rieng)

C1. LuaRunMission (ScriptFuns.cpp:10375): path typo "misions\misionNN" -> g_MissionTabFile.GetString(id+1,2)
    nhu Open/Close; g_StrLower; NULL-check pScript; giu SetGlobal SCRIPT_SUBWORLDINDEX; SafeCallBegin/End.
C2. LuaGlobalExecute (KTongJX2.cpp:4024): boc tien to dw/dwf -> hang doi hoan 1 tick (muc B).
C3. CloseGlbMission (moi, KJx2League.cpp): chay "EndMission" cua missions.txt[id] (doi xung OpenGlbMission).
C4. LGM_ApplyRemoveMember (KJx2League.cpp:758): doc doi 6 removelg; sau khi xoa member, neu removelg~=0
    va league rong -> xoa league + save. (league.lua:132 dat removelg=1 khi nguoi cuoi roi doi.)
C5. Nhom ham Lua-API moi (dang ky GameScriptFuns):
    - GetGblInt/SetGblInt -> map LuaGetGlbValue/LuaSetGlbValue (KJx2SharedStore).
    - Random: 1 doi -> [0,n-1]; 2 doi -> [a,b]; 0 doi -> [0,32767]. (joinmatch.lua:95 Random(n)+1.)
    - Number2Int(x) -> (int)(double) truncate.
    - Time2Tm(t) -> bang Lua {sec,min,hour,day,month,year,wday,yday} 1-based nhu tm Linux? XEM call site
      (liendaunew:8 tbTm[4],[5] = gio, phut => thu tu: [1]=nam? PHAI khop; xem muc F3).
    - SyncTaskValueMore(from,to[,flag]) -> lap SyncTaskValueToClient (KTongJX2.cpp:4038 mau).
    - AskClientForString(...) -> wrapper OpenGetString/GetStringFromUI (ScriptFuns.cpp:13322-13325).
    - GetRespect/AddRespect/SetRespect -> task 39.
    - LoadScript(path) -> ReLoadScript (KSortScript.cpp:290).
    - GetGateWayClientID() -> tra 1 (stub, chi dung trong log relay).
    - CloseGlbMission (C3), TaskName/TaskTime/TaskInterval/TaskCountLimit (no-op).
    - CalcItemCount / CalcEquiproomItemCount / ITEM_GetImmediaItemIndex / CountFreeRoomByWH:
      dem item theo (g,d,p[,level]) trong tui / trang bi+o thuoc / o dung ngay / dem cho trong WxH.
      (Chi tiet arity theo wlls_en_check head.lua:838-882 + officer/helper - cho agent B.)
    - DisabledStall / ForbitTrade / ForbitStamina: co moi tren KPlayer + diem chan (dieu tra khi lam).
    - GetSkillState / GetLastAddFaction / GetBoxLockState / CheckGlobalTradeFlag / ST_CheckTextFilter:
      theo call site that (agent B bao cao) - chi lam nhung ham THUC SU duoc goi boi cay leaguematch.
    - ST_StartDamageCounter/ST_StopDamageCounter/ST_GetDamageCounter/ST_IncreaseDamageCounter:
      ha tang moi trong KPlayer (m_nWllsDmgCounter, m_bWllsDmgCounterOn) + hook diem tru mau
      (dem luong mat mau TRUOC hap thu noi luc - normal.lua muc "So lan bi danh trung").
C6. KJx2League.cpp:996 Breathe: khong can set SubWorld (script tu set) - GIU;
    nhung bo sung: khong co gi. (Xac minh lai khi phan bien: cac ham GetGlbValue khong can context ✓.)
C7. KMission::RemovePlayer: lat MISSION_PARAM_AVAILABLE truoc OnLeave rieng mission 24/25/26 (muc A4).
C8. WriteStringToFile: path bat dau '\' -> ghep root server + tu tao thu muc cha (log relay_log).
C9. LuaIncludeLib: them "ITEM" -> noop.lua (sach log).
C10. LuaRANDOM/LuaOpenMission/...: KHONG dung den.

## D. SCRIPT & SETTINGS (ke hoach)

D1. Chep nguyen byte 38 file GS -> bin\server\script\missions\leaguematch\ (bo cvs\).
    Va cham 3 diem PHAI va (safe_edit, CRLF, giu TCVN3):
    a) head.lua: mo guard dong 2-6 + mo 7 Include macthtype dong 302-316.
    b) timetable.lua (relay): noi bang mua [124..243] (2026-01..2035-12, mau 2010: 2,1,2,5,3,6,2,1,4,5,3,6).
    c) Cac ID vat pham/danh hieu XUNG DOT -> anh xa (cho agent A/B; bang o muc E).
D2. Chep nguyen byte 7 file relay -> bin\server\script\leaguematch\ + file moi gsdriver.lua.
D3. \script\gmscript.lua (du an): them Include wlls_gmscript.lua (dong y het Linux gmscript.lua:7).
D4. startgame.lua: GO khoi lien_dau cu; THEM: goi wlls_autoexe() (Include wlls_autoexec.lua)
    + DynamicExecute gsdriver WLLS_DriverInit.
D5. settings\task\missions.txt: them dong 24/25/26 tro leaguematch (schedule/combat/glbmission);
    dong 11 cu (lien_dau) xu ly theo khao sat agent A. TimerTask.txt: them 50/51 + dong driver.
    LUU Y: bang tra theo (id+1) = so dong -> phai lap day dong 13..23 (placeholder) neu loader doc theo dong.
    -> XEM ket qua agent A ve semantics GetString(nId+1, 2).
6. settings\maps\championship\champion_gmpos.txt: chep tu D:\ServerLinux\server1\settings\maps\championship\.
D7. map_type.txt: dong LEAGUEMATCH them PKEX LEAGUE (doi chieu kha nang engine - agent A).
D8. Go he cu: xoa hook khoi startgame.lua + timerserver.lua; DOI TEN thu muc script\tinhnang\lien_dau
    ra ngoai \script (vd _go_bo_lien_dau_cu\) de khong nap boot; go dong 11 missions/timer neu la cua lien_dau.
D9. wlls_login: móc vao duong login cua du an (theo agent A: global\login.lua co login_add khong;
    khong co thi goi truc tiep tu file login cua du an bang DynamicExecuteByPlayer/Include).
    wlls_leveupcheck: móc vao duong len cap (server_playerlevelup tuong duong cua du an).
D10. Phu thuoc: lib\common.lua, gb_taskfuncs.lua, maps\newworldscript_default.lua, activitysys\*,
    misc\eventsys\*, tong\tong_award_head.lua, task\task_addplayerexp.lua, huoyuedu\huoyuedu.lua
    -> ton tai/khop den dau theo agent A; thieu thi port them tu Linux (nguyen byte).

## E. BANG ANH XA ID (dien sau khi agent A tra)

- Item thuong: 6/1/4417 (Lien Sai le bao), 6/1/4550; item\*.lua: xuezhanlingqi, zhenjing, hongyin...
- Danh hieu: junior 85-88, senior 81-84, EX 200-207 (type1..7 head.lua:287-299).
- Task nguoi choi: 1715-1735, 2500, 2501, 4125 (kiem bien MAX task - agent A muc 9).
- Ladder id: 10196-10257 (+512 hang: JX2LADDER_TOP hien 10 -> nang 512? xem agent A muc 12).

## F. CON MO (dang cho du lieu)

F1. Agent A (kiem ke du an) + Agent B (doc sau Lua con lai) chua tra ket qua.
F2. ST_* hook point trong duong tru mau KNpc - se dieu tra rieng.
F3. Time2Tm thu tu truong - doc call site helper.lua/liendaunew.
F4. Maps 540-579/560-563/570-573 du lieu server (_srv.fp / region) - agent A muc 11.
F5. Sale(146)/Sale(173)/shop lien dau (yaoshang Sale id?) - agent A muc 6 + agent B yaoshang.
F6. wlls_descript/wlls_get_mems/wlls_npcname o npc\head.lua - agent B.
F7. pgaming overlay (shopliendau, hieuthuoc liendau, status NPC): QUYET sau khi doc noi dung -
    day la lop private-server overlay, KHONG thuoc leaguematch goc; mac dinh KHONG port,
    tru khi user muon (ghi ro trong bao cao cuoi).

## G. NGUYEN TAC PHAN BIEN

Moi cum thay doi: (1) doi chieu nguoc lai voi file Linux goc tung dong; (2) grep tat ca call site
bi anh huong trong CA du an (khong chi leaguematch); (3) sau khi build: check_encoding tung file sua;
(4) tac tu phan bien doc lap doc lai diff; (5) boot server that + doc log; (6) nghiem thu:
1 tran Song dau tron ven (lap doi -> bao danh -> ghep -> danh -> diem -> xep hang -> thuong cuoi mua).
